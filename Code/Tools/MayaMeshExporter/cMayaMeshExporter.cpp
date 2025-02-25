// Header Files
//=============

#include "cMayaMeshExporter.h"

#include <algorithm>
#include <climits>
#include <cstdint>
#include <fstream>
#include <map>
#include <maya/MDagPath.h>
#include <maya/MFloatVector.h>
#include <maya/MFloatVectorArray.h>
#include <maya/MFnMesh.h>
#include <maya/MGlobal.h>
#include <maya/MIntArray.h>
#include <maya/MItDag.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MItSelectionList.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPoint.h>
#include <maya/MPointArray.h>
#include <maya/MSelectionList.h>
#include <sstream>
#include <string>
#include <vector>


// Vertex Definition
//==================

namespace
{
	// This is the vertex struct that will hold the Maya data.
	// Note that this is independent of the "sVertex" struct
	// used at build-time and run-time to deal with binary mesh data.
	// This struct is used to store Maya-specific vertex information
	// and then output it to the human-readable source mesh file.
	struct sVertex_maya
	{
		// Position
		float x, y, z;
		// Normal
		float nx, ny, nz;
		// Tangent
		float tx, ty, tz;
		// Bitangent
		float btx, bty, btz;
		// Texture coordinates
		float u, v;
		// Color
		float r, g, b, a;

		sVertex_maya(const MPoint& i_position, const MFloatVector& i_normal,
			const MFloatVector& i_tangent, const MFloatVector& i_bitangent,
			const float i_texcoordU, const float i_texcoordV,
			const MColor& i_vertexColor)
			:
			x(static_cast<float>(i_position.x)), y(static_cast<float>(i_position.y)), z(static_cast<float>(i_position.z)),
			nx(i_normal.x), ny(i_normal.y), nz(i_normal.z),
			tx(i_tangent.x), ty(i_tangent.y), tz(i_tangent.z),
			btx(i_bitangent.x), bty(i_bitangent.y), btz(i_bitangent.z),
			u(i_texcoordU), v(i_texcoordV),
			r(i_vertexColor.r), g(i_vertexColor.g), b(i_vertexColor.b), a(i_vertexColor.a)
		{

		}
	};

	// This stores any information associated with a vertex that is necessary to export the mesh.
	// Most of the information is in the sVertex_maya itself,
	// but there are a few other things needed during processing that shouldn't be exported.
	struct sVertexInfo
	{
		sVertex_maya vertex;

		// A Maya "shading group" is similar to what we call a "material" in our class
		size_t shadingGroup;
		// This unique key is calculated in order to decide whether a new vertex should be created or not,
		// and that calculated key is assigned to the vertex so that it can be sorted uniquely
		const std::string uniqueKey;

		sVertexInfo(const MPoint& i_position, const MFloatVector& i_normal,
			const MFloatVector& i_tangent, const MFloatVector& i_bitangent,
			const float i_texcoordU, const float i_texcoordV,
			const MColor& i_vertexColor,
			const size_t i_shadingGroup,
			const std::string& i_uniqueKey)
			:
			vertex(i_position, i_normal, i_tangent, i_bitangent, i_texcoordU, i_texcoordV, i_vertexColor),
			shadingGroup(i_shadingGroup),
			uniqueKey(i_uniqueKey)
		{

		}
	};
}

// Static Data Initialization
//===========================

namespace
{
	const size_t s_vertexCountPerTriangle = 3;

	struct sTriangle
	{
		std::string vertexKeys[s_vertexCountPerTriangle];
		size_t shadingGroup;

		static bool CompareTriangles(const sTriangle& i_lhs, const sTriangle& i_rhs)
		{
			// Sort the triangles by shading group
			// (so that a single draw call can work with a single contiguous block of vertex and index data)
			if (i_lhs.shadingGroup != i_rhs.shadingGroup)
			{
				return i_lhs.shadingGroup < i_rhs.shadingGroup;
			}
			else
			{
				// If two triangles use the same shading group the order doesn't matter,
				// but it's nice to have the exported files be deterministic
				for (size_t i = 0; i < s_vertexCountPerTriangle; ++i)
				{
					if (i_lhs.vertexKeys[i] != i_rhs.vertexKeys[i])
					{
						return i_lhs.vertexKeys[i] < i_rhs.vertexKeys[i];
					}
				}
			}
			// If there are two identical triangles it means that i_lhs isn't less than i_rhs
			return false;
		}
	};

	struct sMaterialInfo
	{
		// As an example, the material node's name (which is useless) is currently stored
		MString nodeName;

		// Keep track of the the range of vertices and indices that use this material
		// (the stored indices are for the final vertex buffer and index buffer vectors)
		struct
		{
			size_t first, last;
		} vertexRange;
		struct
		{
			size_t first, last;
		} indexRange;

		sMaterialInfo()
		{
			vertexRange.first = indexRange.first = SIZE_MAX;
			vertexRange.last = indexRange.last = 0;
		}
	};
}

// Helper Function Declarations
//=============================

namespace
{
	std::string CreateUniqueVertexKey(const int i_positionIndex, const int i_normalIndex, const int i_tangentIndex,
		const int i_texcoordIndex, const int i_vertexColorIndex, const size_t i_shadingGroupIndex, const char* i_transformName);
	MStatus FillVertexAndIndexBuffer(const std::map<std::string, sVertexInfo>& i_uniqueVertices, const std::vector<MObject>& i_shadingGroups,
		std::vector<sTriangle>& io_triangles,
		std::vector<sVertexInfo>& o_vertexBuffer, std::vector<size_t>& o_indexBuffer,
		std::vector<sMaterialInfo>& o_materialInfo);
	MStatus ProcessAllMeshes(std::map<std::string, sVertexInfo>& o_uniqueVertices, std::vector<sTriangle>& o_triangles,
		std::vector<MObject>& o_shadingGroups);
	MStatus ProcessSelectedMeshes(std::map<std::string, sVertexInfo>& o_uniqueVertices, std::vector<sTriangle>& o_triangles,
		std::vector<MObject>& o_shadingGroups);
	MStatus ProcessSingleDagNode(const MDagPath& i_dagPath,
		std::map<std::string, sVertexInfo>& io_uniqueVertices, std::vector<sTriangle>& io_triangles,
		std::vector<MObject>& io_shadingGroups, std::map<std::string, size_t>& io_map_shadingGroupNamesToIndices);
	MStatus WriteMeshToFile(const MString& i_fileName, const std::vector<sVertexInfo>& i_vertexBuffer, const std::vector<size_t>& i_indexBuffer,
		const std::vector<sMaterialInfo>& i_materialInfo);
}

// Inherited Interface
//====================

MStatus eae6320::cMayaMeshExporter::writer(const MFileObject& i_file, const MString& i_options, FileAccessMode i_mode)
{
	MStatus status;

	// Gather the vertex and index buffer information
	std::map<std::string, sVertexInfo> uniqueVertices;
	std::vector<sTriangle> triangles;
	std::vector<MObject> shadingGroups;
	{
		// The user decides whether to export the entire scene or just a selection
		if (i_mode == MPxFileTranslator::kExportAccessMode)
		{
			status = ProcessAllMeshes(uniqueVertices, triangles, shadingGroups);
			if (!status)
			{
				return status;
			}
		}
		else if (i_mode == MPxFileTranslator::kExportActiveAccessMode)
		{
			status = ProcessSelectedMeshes(uniqueVertices, triangles, shadingGroups);
			if (!status)
			{
				return status;
			}
		}
		else
		{
			MGlobal::displayError("Unexpected file access mode");
			return MStatus::kFailure;
		}
	}

	// Convert the mesh information to vertex and index buffers
	std::vector<sVertexInfo> vertexBuffer;
	std::vector<size_t> indexBuffer;
	std::vector<sMaterialInfo> materialInfo;
	{
		status = FillVertexAndIndexBuffer(uniqueVertices, shadingGroups, triangles, vertexBuffer, indexBuffer, materialInfo);
		if (!status)
		{
			return status;
		}
	}

	// Write the mesh to the requested file
	{
		const MString filePath = i_file.fullName();
		return WriteMeshToFile(filePath, vertexBuffer, indexBuffer, materialInfo);
	}
}

// Helper Function Definitions
//============================

namespace
{
	std::string CreateUniqueVertexKey(const int i_positionIndex, const int i_normalIndex, const int i_tangentIndex,
		const int i_texcoordIndex, const int i_vertexColorIndex, const size_t i_shadingGroupIndex, const char* i_transformName)
	{
		std::ostringstream vertexKey;
		vertexKey << "_" << i_positionIndex << "_" << i_normalIndex << "_" << i_tangentIndex << "_" << i_texcoordIndex << "_" << i_vertexColorIndex << "_" << i_shadingGroupIndex;
		if (i_transformName)
		{
			vertexKey << "_" << i_transformName;
		}
		return vertexKey.str();
	}

	MStatus FillVertexAndIndexBuffer(const std::map<std::string, sVertexInfo>& i_uniqueVertices, const std::vector<MObject>& i_shadingGroups,
		std::vector<sTriangle>& io_triangles,
		std::vector<sVertexInfo>& o_vertexBuffer, std::vector<size_t>& o_indexBuffer,
		std::vector<sMaterialInfo>& o_materialInfo)
	{
		MStatus status;

		// Fill in the material info
		{
			const size_t shadingGroupCount = i_shadingGroups.size();
			o_materialInfo.resize(shadingGroupCount);
			for (size_t i = 0; i < shadingGroupCount; ++i)
			{
				const MObject& shadingGroup = i_shadingGroups[i];
				MPlug surfaceShaderPlug = MFnDependencyNode(shadingGroup).findPlug("surfaceShader", &status);
				if (status)
				{
					MPlugArray connections;
					{
						const bool getConnectionsWithThisAsDestination = true;
						const bool dontGetConnectionsWithThisAsSource = false;
						surfaceShaderPlug.connectedTo(connections, getConnectionsWithThisAsDestination, dontGetConnectionsWithThisAsSource, &status);
						if (!status)
						{
							MGlobal::displayError(status.errorString());
							return status;
						}
					}
					if (connections.length() == 1)
					{
						// This is where you would put code to extract relevant information from the material
						sMaterialInfo& o_material = o_materialInfo[i];

						// For now this just gets the material node's name (which is useless),
						// but this could be made more sophisticated
						MFnDependencyNode materialNode(connections[0].node());
						o_material.nodeName = materialNode.name();
					}
					else if (connections.length() == 0)
					{
						// This can happen if a material was assigned to a mesh,
						// but then the material was deleted (while the shading group remained).
						// This example code will still work with a missing material,
						// but if you make the material handling more sophisticated
						// you should make sure to handle this case.
						o_materialInfo[i].nodeName = "UNASSIGNED";
					}
					else
					{
						MGlobal::displayError(MString("A shading group's surface shader had ") + connections.length() + " connections");
						return MStatus::kFailure;
					}
				}
				else
				{
					MGlobal::displayError(status.errorString());
					return status;
				}
			}
		}

		// Fill the vertex buffer with the vertices
		// and create a map from the unique key to the assigned index in the vertex buffer
		std::map<std::string, size_t> vertexKeyToIndexMap;
		{
			// Create a reverse map with a custom sorting order for the vertices
			struct CompareVertices
			{
				bool operator()(const sVertexInfo& i_lhs, const sVertexInfo& i_rhs)
				{
					// Sort the vertices by shading group
					// (so that a single draw call can work with a single contiguous block of vertex data)
					if (i_lhs.shadingGroup != i_rhs.shadingGroup)
					{
						return i_lhs.shadingGroup < i_rhs.shadingGroup;
					}
					else
					{
						// If two vertices use the same shading group the order doesn't matter
						return i_lhs.uniqueKey < i_rhs.uniqueKey;
					}
				}
			};
			std::map<sVertexInfo, std::string, CompareVertices> sortedVertices;
			for (std::map<std::string, sVertexInfo>::const_iterator i = i_uniqueVertices.begin(); i != i_uniqueVertices.end(); ++i)
			{
				sortedVertices.insert(std::make_pair(i->second, i->first));
			}
			// Assign the sorted vertices to the buffer
			size_t vertexIndex = 0;
			for (std::map<sVertexInfo, std::string>::iterator i = sortedVertices.begin(); i != sortedVertices.end(); ++i, ++vertexIndex)
			{
				const sVertexInfo& vertex = i->first;
				o_vertexBuffer.push_back(vertex);
				vertexKeyToIndexMap.insert(std::make_pair(i->second, vertexIndex));
				// Update the vertex range for the shading group that this material uses
				if (vertex.shadingGroup < o_materialInfo.size())
				{
					sMaterialInfo& materialInfo = o_materialInfo[vertex.shadingGroup];
					materialInfo.vertexRange.first = std::min(vertexIndex, materialInfo.vertexRange.first);
					materialInfo.vertexRange.last = std::max(vertexIndex, materialInfo.vertexRange.last);
				}
			}
		}

		// Fill the index buffer with the indices
		{
			// Sort the triangles by shading group
			std::sort(io_triangles.begin(), io_triangles.end(), sTriangle::CompareTriangles);
			// Assign the triangle indices to the index buffer
			const size_t triangleCount = io_triangles.size();
			const size_t indexCount = triangleCount * s_vertexCountPerTriangle;
			o_indexBuffer.resize(indexCount);
			for (size_t i = 0; i < triangleCount; ++i)
			{
				const sTriangle& triangle = io_triangles[i];
				for (size_t j = 0; j < s_vertexCountPerTriangle; ++j)
				{
					const std::string& vertexKey = triangle.vertexKeys[j];
					const size_t triangleIndex = vertexKeyToIndexMap.find(vertexKey)->second;
					const size_t indexBufferIndex = (i * s_vertexCountPerTriangle) + j;
					o_indexBuffer[indexBufferIndex] = triangleIndex;
					// Update the index range for the shading group that this material uses
					if (triangle.shadingGroup < o_materialInfo.size())
					{
						sMaterialInfo& materialInfo = o_materialInfo[triangle.shadingGroup];
						materialInfo.indexRange.first = std::min(indexBufferIndex, materialInfo.indexRange.first);
						materialInfo.indexRange.last = std::max(indexBufferIndex, materialInfo.indexRange.last);
					}
				}
			}
		}

		return MStatus::kSuccess;
	}

	MStatus ProcessAllMeshes(std::map<std::string, sVertexInfo>& o_uniqueVertices, std::vector<sTriangle>& o_triangles,
		std::vector<MObject>& o_shadingGroups)
	{
		std::map<std::string, size_t> map_shadingGroupNamesToIndices;
		for (MItDag i(MItDag::kDepthFirst, MFn::kMesh); !i.isDone(); i.next())
		{
			MDagPath dagPath;
			i.getPath(dagPath);
			if (!ProcessSingleDagNode(dagPath, o_uniqueVertices, o_triangles, o_shadingGroups, map_shadingGroupNamesToIndices))
			{
				return MStatus::kFailure;
			}
		}

		return MStatus::kSuccess;
	}

	MStatus ProcessSelectedMeshes(std::map<std::string, sVertexInfo>& o_uniqueVertices, std::vector<sTriangle>& o_triangles,
		std::vector<MObject>& o_shadingGroups)
	{
		// Iterate through each selected mesh
		MSelectionList selectionList;
		MStatus status = MGlobal::getActiveSelectionList(selectionList);
		if (status)
		{
			std::map<std::string, size_t> map_shadingGroupNamesToIndices;
			for (MItSelectionList i(selectionList, MFn::kMesh); !i.isDone(); i.next())
			{
				MDagPath dagPath;
				i.getDagPath(dagPath);
				if (!ProcessSingleDagNode(dagPath, o_uniqueVertices, o_triangles, o_shadingGroups, map_shadingGroupNamesToIndices))
				{
					return MStatus::kFailure;
				}
			}
		}
		else
		{
			MGlobal::displayError(MString("Failed to get active selection list: ") + status.errorString());
			return MStatus::kFailure;
		}

		return MStatus::kSuccess;
	}

	MStatus ProcessSingleDagNode(const MDagPath& i_dagPath,
		std::map<std::string, sVertexInfo>& io_uniqueVertices, std::vector<sTriangle>& io_triangles,
		std::vector<MObject>& io_shadingGroups, std::map<std::string, size_t>& io_map_shadingGroupNamesToIndices)/*
	{
		MStatus status;

		// Get the mesh from the DAG path
		MFnMesh mesh(i_dagPath);
		if (mesh.isIntermediateObject())
		{
			return MStatus::kSuccess;
		}

		// Get a list of the positions
		MPointArray positions;
		{
			status = mesh.getPoints(positions, MSpace::kWorld);
			if (!status)
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Get a list of the normals
		MFloatVectorArray normals;
		{
			status = mesh.getNormals(normals, MSpace::kWorld);
			if (!status)
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Get a list of tangents
		MFloatVectorArray tangents;
		{
			status = mesh.getTangents(tangents, MSpace::kWorld);
			if (!status)
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Get a list of bitangents
		MFloatVectorArray bitangents;
		{
			status = mesh.getBinormals(bitangents, MSpace::kWorld);
			if (!status)
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Get a list of the texture coordinates
		MFloatArray texcoordUs, texcoordVs;
		{
			status = mesh.getUVs(texcoordUs, texcoordVs);
			if (!status)
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Get a list of the vertex colors
		MColorArray vertexColors;
		{
			int colorSetCount = mesh.numColorSets();
			if (colorSetCount > 0)
			{
				MString* useDefaultColorSet = NULL;	// If more than one color set exists this code will only get the "default" one (as chosen by Maya)
				MColor defaultColor(1.0f, 1.0f, 1.0f, 1.0f);
				status = mesh.getColors(vertexColors, useDefaultColorSet, &defaultColor);
				if (!status)
				{
					MGlobal::displayError(status.errorString());
					return status;
				}
			}
		}

		// A single mesh (i.e. geometric data)
		// can be used by multiple DAG nodes in a Maya scene.
		// (For example, a single sphere mesh could be instanced many times
		// but at different positions, with different orientations, scales, and materials.)
		// An instance ID identifies the specific node that should be processed by this function.
		unsigned int instanceId = 0;
		if (i_dagPath.isInstanced())
		{
			instanceId = i_dagPath.instanceNumber(&status);
			if (!status)
			{
				MGlobal::displayError(MString("Failed to get the DAG path's instance number: ") + status.errorString());
				return MStatus::kFailure;
			}
		}

		// Get a list of the shading groups (i.e. materials)
		std::vector<size_t> polygonShadingGroupIndices;
		{
			MObjectArray shadingGroups;
			MIntArray localIndices;
			status = mesh.getConnectedShaders(instanceId, shadingGroups, localIndices);
			if (status)
			{
				// Remap each local shading group index (i.e. that applies to the array returned by getConnectedShaders())
				// to an index into our static list
				std::vector<size_t> shadingGroupIndices;
				{
					shadingGroupIndices.resize(shadingGroups.length());
					for (unsigned int i = 0; i < shadingGroups.length(); ++i)
					{
						size_t shadingGroupIndex;
						{
							MObject shadingGroup = shadingGroups[i];
							std::string shadingGroupName = MFnDependencyNode(shadingGroup).name().asChar();
							std::map<std::string, size_t>::iterator mapLookUp = io_map_shadingGroupNamesToIndices.find(shadingGroupName);
							if (mapLookUp != io_map_shadingGroupNamesToIndices.end())
							{
								shadingGroupIndex = mapLookUp->second;
							}
							else
							{
								const size_t newIndex = io_shadingGroups.size();
								io_shadingGroups.push_back(shadingGroup);
								io_map_shadingGroupNamesToIndices.insert(std::make_pair(shadingGroupName, newIndex));
								shadingGroupIndex = newIndex;
							}
						}
						shadingGroupIndices[i] = shadingGroupIndex;
					}
				}
				// Convert each polygon shading group index
				{
					const unsigned int polygonCount = localIndices.length();
					if (polygonCount == mesh.numPolygons())
					{
						polygonShadingGroupIndices.resize(polygonCount);
						for (unsigned int i = 0; i < polygonCount; ++i)
						{
							const int localIndex = localIndices[i];
							if (localIndex >= 0)
							{
								polygonShadingGroupIndices[i] = shadingGroupIndices[static_cast<size_t>(localIndex)];
							}
							else
							{
								// If a polygon doesn't have a shading group the index will be -1
								polygonShadingGroupIndices[i] = static_cast<size_t>(localIndex);
							}
						}
					}
					else
					{
						MGlobal::displayError(MString("mesh.numPolygons() returned ") + mesh.numPolygons()
							+ " but mesh.getConnectedShaders() returned " + polygonCount
							+ " indices! According to my understanding of the Maya API this should never happen");
					}
				}
			}
			else
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Gather vertex and triangle information
		{
			// Use the name of the transform to ensure uniqueness
			// (This is necessary because uniqueness is otherwise determined by indices within a given mesh.
			// If the actual data (like the position coordinates) was used instead then this could be ignored
			// and two identical vertices from two completely different meshes could be saved as a single one.
			// This should happen rarely in practice, but a production-quality exporter
			// should probably be more strict about testing equivalence to try and save as much memory as possible.)
			const char* transformName = NULL;
			{
				transformName = MFnDependencyNode(mesh.parent(instanceId)).name().asChar();
			}

			MPointArray trianglePositions;
			MIntArray positionIndices;
			size_t polygonIndex = 0;
			for (MItMeshPolygon i(mesh.object()); !i.isDone(); i.next(), ++polygonIndex)
			{
				if (i.hasValidTriangulation())
				{
					const size_t shadingGroup = polygonShadingGroupIndices[polygonIndex];

					// Store information for each vertex in the polygon
					std::map<int, const std::string> indexToKeyMap;
					{
						MIntArray vertices;
						status = i.getVertices(vertices);
						if (status)
						{
							for (unsigned int j = 0; j < vertices.length(); ++j)
							{
								const int positionIndex = vertices[j];
								const int normalIndex = i.normalIndex(j);
								const int tangentIndex = i.tangentIndex(j);
								int texcoordIndex;
								{
									status = i.getUVIndex(j, texcoordIndex);
									if (!status)
									{
										MGlobal::displayError(status.errorString());
										return status;
									}
								}
								int vertexColorIndex = -1;
								MColor vertexColor(1.0f, 1.0f, 1.0f, 1.0f);
								{
									int colorSetCount = mesh.numColorSets();
									if (colorSetCount > 0)
									{
										status = i.getColorIndex(j, vertexColorIndex);
										if (status)
										{
											if (vertexColorIndex >= 0)
											{
												vertexColor = vertexColors[vertexColorIndex];
											}
										}
										else
										{
											MGlobal::displayError(status.errorString());
											return status;
										}
									}
								}
								const std::string vertexKey = CreateUniqueVertexKey(positionIndex, normalIndex, tangentIndex,
									texcoordIndex, vertexColorIndex, shadingGroup, transformName);
								indexToKeyMap.insert(std::make_pair(positionIndex, vertexKey));
								io_uniqueVertices.insert(std::make_pair(vertexKey,
									sVertexInfo(positions[positionIndex], normals[normalIndex],
										tangents[tangentIndex], bitangents[tangentIndex],
										texcoordUs[texcoordIndex], texcoordVs[texcoordIndex],
										vertexColor,
										shadingGroup, vertexKey)
								));
							}
						}
						else
						{
							MGlobal::displayError(status.errorString());
							return status;
						}
					}
					// Store information for each individual triangle in the polygon
					{
						int triangleCount = 0;
						i.numTriangles(triangleCount);
						for (int j = 0; j < triangleCount; ++j)
						{
							i.getTriangle(j, trianglePositions, positionIndices);
							if (static_cast<size_t>(positionIndices.length()) == s_vertexCountPerTriangle)
							{
								sTriangle triangle;
								for (unsigned int k = 0; k < static_cast<unsigned int>(s_vertexCountPerTriangle); ++k)
								{
									const int positionIndex = positionIndices[k];
									std::map<int, const std::string>::iterator mapLookUp = indexToKeyMap.find(positionIndex);
									if (mapLookUp != indexToKeyMap.end())
									{
										triangle.vertexKeys[k] = mapLookUp->second;
									}
									else
									{
										MGlobal::displayError("A triangle gave a different vertex index than the polygon gave");
										return MStatus::kFailure;
									}
								}
								triangle.shadingGroup = shadingGroup;
								io_triangles.push_back(triangle);
							}
							else
							{
								MGlobal::displayError(MString("Triangle #") + j + " reports that it has " +
									positionIndices.length() + "! According to my understanding of Maya this should never happen");
								return MStatus::kFailure;
							}
						}
					}
				}
				else
				{
					MGlobal::displayError("This mesh has an invalid triangulation");
					return MStatus::kFailure;
				}
			}
		}

		return MStatus::kSuccess;
	}*/
	{
		MStatus status;

		// Get the mesh from the DAG path
		MFnMesh mesh(i_dagPath);
		if (mesh.isIntermediateObject())
		{
			return MStatus::kSuccess;
		}

		// Get a list of the positions
		MPointArray positions;
		{
			status = mesh.getPoints(positions, MSpace::kWorld);
			if (!status)
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Get a list of the normals
		MFloatVectorArray normals;
		{
			status = mesh.getNormals(normals, MSpace::kWorld);
			if (!status)
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Get a list of tangents and bitangents
		MFloatVectorArray tangents, bitangents;
		{
			const MString* const useDefaultUvSet = NULL;	// If more than one UV set exists this code will use the "default" one (as chosen by Maya)
			status = mesh.getTangents(tangents, MSpace::kWorld, useDefaultUvSet);
			if (!status
				// Tangents may not exist if there are no UVs
				&& (status != MS::kInvalidParameter))
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
			status = mesh.getBinormals(bitangents, MSpace::kWorld, useDefaultUvSet);
			if (!status
				// Bitngents may not exist if there are no UVs
				&& (status != MS::kInvalidParameter))
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Get a list of the texture coordinates
		MFloatArray texcoordUs, texcoordVs;
		{
			const int uvSetCount = mesh.numUVSets(&status);
			if (status)
			{
				if (uvSetCount > 0)
				{
					const MString* const useDefaultUvSet = NULL;	// If more than one UV set exists this code will use the "default" one (as chosen by Maya)
					status = mesh.getUVs(texcoordUs, texcoordVs, useDefaultUvSet);
					if (!status)
					{
						MGlobal::displayError(status.errorString());
						return status;
					}
				}
			}
			else
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Get a list of the vertex colors
		MColorArray vertexColors;
		{
			const int colorSetCount = mesh.numColorSets(&status);
			if (status)
			{
				if (colorSetCount > 0)
				{
					const MString* const useDefaultColorSet = NULL;	// If more than one color set exists this code will use the "default" one (as chosen by Maya)
					MColor defaultColor(1.0f, 1.0f, 1.0f, 1.0f);
					status = mesh.getColors(vertexColors, useDefaultColorSet, &defaultColor);
					if (!status)
					{
						MGlobal::displayError(status.errorString());
						return status;
					}
				}
			}
			else
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// A single mesh (i.e. geometric data)
		// can be used by multiple DAG nodes in a Maya scene.
		// (For example, a single sphere mesh could be instanced many times
		// but at different positions, with different orientations, scales, and materials.)
		// An instance ID identifies the specific node that should be processed by this function.
		unsigned int instanceId = 0;
		if (i_dagPath.isInstanced())
		{
			instanceId = i_dagPath.instanceNumber(&status);
			if (!status)
			{
				MGlobal::displayError(MString("Failed to get the DAG path's instance number: ") + status.errorString());
				return MStatus::kFailure;
			}
		}

		// Get a list of the shading groups (i.e. materials)
		std::vector<size_t> polygonShadingGroupIndices;
		{
			MObjectArray shadingGroups;
			MIntArray localIndices;
			status = mesh.getConnectedShaders(instanceId, shadingGroups, localIndices);
			if (status)
			{
				// Remap each local shading group index (i.e. that applies to the array returned by getConnectedShaders())
				// to an index into our static list
				std::vector<size_t> shadingGroupIndices;
				{
					shadingGroupIndices.resize(shadingGroups.length());
					for (unsigned int i = 0; i < shadingGroups.length(); ++i)
					{
						size_t shadingGroupIndex;
						{
							MObject shadingGroup = shadingGroups[i];
							std::string shadingGroupName = MFnDependencyNode(shadingGroup).name().asChar();
							std::map<std::string, size_t>::iterator mapLookUp = io_map_shadingGroupNamesToIndices.find(shadingGroupName);
							if (mapLookUp != io_map_shadingGroupNamesToIndices.end())
							{
								shadingGroupIndex = mapLookUp->second;
							}
							else
							{
								const size_t newIndex = io_shadingGroups.size();
								io_shadingGroups.push_back(shadingGroup);
								io_map_shadingGroupNamesToIndices.insert(std::make_pair(shadingGroupName, newIndex));
								shadingGroupIndex = newIndex;
							}
						}
						shadingGroupIndices[i] = shadingGroupIndex;
					}
				}
				// Convert each polygon shading group index
				{
					const unsigned int polygonCount = localIndices.length();
					if (polygonCount == mesh.numPolygons())
					{
						polygonShadingGroupIndices.resize(polygonCount);
						for (unsigned int i = 0; i < polygonCount; ++i)
						{
							const int localIndex = localIndices[i];
							if (localIndex >= 0)
							{
								polygonShadingGroupIndices[i] = shadingGroupIndices[static_cast<size_t>(localIndex)];
							}
							else
							{
								// If a polygon doesn't have a shading group the index will be -1
								polygonShadingGroupIndices[i] = static_cast<size_t>(localIndex);
							}
						}
					}
					else
					{
						MGlobal::displayError(MString("mesh.numPolygons() returned ") + mesh.numPolygons()
							+ " but mesh.getConnectedShaders() returned " + polygonCount
							+ " indices! According to my understanding of the Maya API this should never happen");
					}
				}
			}
			else
			{
				MGlobal::displayError(status.errorString());
				return status;
			}
		}

		// Gather vertex and triangle information
		{
			// Use the name of the transform to ensure uniqueness
			// (This is necessary because uniqueness is otherwise determined by indices within a given mesh.
			// If the actual data (like the position coordinates) was used instead then this could be ignored
			// and two identical vertices from two completely different meshes could be saved as a single one.
			// This should happen rarely in practice, but a production-quality exporter
			// should probably be more strict about testing equivalence to try and save as much memory as possible.)
			const char* transformName = NULL;
			{
				transformName = MFnDependencyNode(mesh.parent(instanceId)).name().asChar();
			}

			MPointArray trianglePositions;
			MIntArray positionIndices;
			size_t polygonIndex = 0;
			for (MItMeshPolygon i(mesh.object()); !i.isDone(); i.next(), ++polygonIndex)
			{
				if (i.hasValidTriangulation())
				{
					const size_t shadingGroup = polygonShadingGroupIndices[polygonIndex];

					// Store information for each vertex in the polygon
					std::map<int, const std::string> indexToKeyMap;
					{
						MIntArray vertices;
						status = i.getVertices(vertices);
						if (status)
						{
							for (unsigned int j = 0; j < vertices.length(); ++j)
							{
								const int positionIndex = vertices[j];
								const int normalIndex = i.normalIndex(j);
								int texcoordIndex = -1;
								struct { float u, v; } texcoord = { 0.0f, 0.0f };
								{
									int potentialTexcoordIndex;
									const MString* const useDefaultUvSet = NULL;
									status = i.getUVIndex(j, potentialTexcoordIndex, useDefaultUvSet);
									if (status && (potentialTexcoordIndex >= 0)
										&& (static_cast<unsigned int>(potentialTexcoordIndex) < texcoordUs.length())
										&& (static_cast<unsigned int>(potentialTexcoordIndex) < texcoordVs.length()))
									{
										texcoordIndex = potentialTexcoordIndex;
										texcoord.u = texcoordUs[texcoordIndex];
										texcoord.v = texcoordVs[texcoordIndex];
									}
								}
								int tangentIndex = -1;
								MFloatVector tangent(0.0f, 0.0f, 0.0f);
								MFloatVector bitangent(0.0f, 0.0f, 0.0f);
								{
									const int potentialTangentIndex = i.tangentIndex(j, &status);
									if (status && (potentialTangentIndex >= 0)
										&& (static_cast<unsigned int>(potentialTangentIndex) < tangents.length())
										&& (static_cast<unsigned int>(potentialTangentIndex) < bitangents.length()))
									{
										tangentIndex = potentialTangentIndex;
										tangent = tangents[tangentIndex];
										bitangent = bitangents[tangentIndex];
									}
								}
								int vertexColorIndex = -1;
								MColor vertexColor(1.0f, 1.0f, 1.0f, 1.0f);
								{
									int potentialColorIndex;
									const MString* const useDefaultColorSet = NULL;
									status = i.getColorIndex(j, potentialColorIndex, useDefaultColorSet);
									if (status && (potentialColorIndex >= 0)
										&& (static_cast<unsigned int>(potentialColorIndex) < vertexColors.length()))
									{
										vertexColorIndex = potentialColorIndex;
										vertexColor = vertexColors[vertexColorIndex];
									}
								}
								const std::string vertexKey = CreateUniqueVertexKey(positionIndex, normalIndex, tangentIndex,
									texcoordIndex, vertexColorIndex, shadingGroup, transformName);
								indexToKeyMap.insert(std::make_pair(positionIndex, vertexKey));
								io_uniqueVertices.insert(std::make_pair(vertexKey,
									sVertexInfo(positions[positionIndex], normals[normalIndex],
										tangent, bitangent,
										texcoord.u, texcoord.v,
										vertexColor,
										shadingGroup, vertexKey)
								));
							}
						}
						else
						{
							MGlobal::displayError(status.errorString());
							return status;
						}
					}
					// Store information for each individual triangle in the polygon
					{
						int triangleCount = 0;
						i.numTriangles(triangleCount);
						for (int j = 0; j < triangleCount; ++j)
						{
							i.getTriangle(j, trianglePositions, positionIndices);
							if (static_cast<size_t>(positionIndices.length()) == s_vertexCountPerTriangle)
							{
								sTriangle triangle;
								for (unsigned int k = 0; k < static_cast<unsigned int>(s_vertexCountPerTriangle); ++k)
								{
									const int positionIndex = positionIndices[k];
									std::map<int, const std::string>::iterator mapLookUp = indexToKeyMap.find(positionIndex);
									if (mapLookUp != indexToKeyMap.end())
									{
										triangle.vertexKeys[k] = mapLookUp->second;
									}
									else
									{
										MGlobal::displayError("A triangle gave a different vertex index than the polygon gave");
										return MStatus::kFailure;
									}
								}
								triangle.shadingGroup = shadingGroup;
								io_triangles.push_back(triangle);
							}
							else
							{
								MGlobal::displayError(MString("Triangle #") + j + " reports that it has " +
									positionIndices.length() + "! According to my understanding of Maya this should never happen");
								return MStatus::kFailure;
							}
						}
					}
				}
				else
				{
					MGlobal::displayError("This mesh has an invalid triangulation");
					return MStatus::kFailure;
				}
			}
		}

		return MStatus::kSuccess;
	}

	MStatus WriteMeshToFile(const MString& i_fileName, const std::vector<sVertexInfo>& i_vertexBuffer, const std::vector<size_t>& i_indexBuffer,
		const std::vector<sMaterialInfo>& i_materialInfo)
	{
		// Maya's coordinate system is different than the default Direct3D behavior
		// (it is right handed and UVs have (0,0) at the lower left corner).
		// For our class I advise keeping things the Maya way (which matches the default OpenGL behavior),
		// but if you wanted to convert everything in the exported file to match the standard Direct3D behavior
		// one way to do it would be to make the following conversions:
		//	* POSITION	-> x, y, -z
		//	* NORMAL	-> nx, ny, -nz
		//	* TANGENT	-> tx, ty, -tz
		//	* BITANGENT	-> -btx, -bty, btz
		//	* TEXCOORD	-> u, 1 - v
		//
		//	* triangle index order	-> index_0, index_2, index_1

		size_t numberOfVertices = i_vertexBuffer.size();
		size_t numberOfIndices = i_indexBuffer.size();

		std::ofstream fout(i_fileName.asChar());

		// Making the file stream to output only fixed floating point numbers with 5 decimal places
		fout.precision(5);
		fout << std::fixed;

		if (fout.is_open())
		{
			fout <<
				"--Mesh Data\n"
				"return\n"

				// Opening Main Table			
				"{\n"
				"\t-- This table contains vertices\n"
				"\t-- Total number of vertices: " << numberOfVertices << "\n"
				"\tvertices =\n"

				// Opening Vertices Table			
				"\t{\n"
				"\t\t-- This table contains positions(x,y,z)\n"
				"\t\tpositions =\n"

				// Opening Positions Table				
				"\t\t{\n";
			for (size_t i = 0; i < numberOfVertices; i++)
			{
				fout << "\t\t\t{" << i_vertexBuffer[i].vertex.x << "," << i_vertexBuffer[i].vertex.y << "," << i_vertexBuffer[i].vertex.z << "},\n";
			}
			fout <<
				// Closing Position Table
				"\t\t},\n"

				"\t\t-- This table contains colors(r,g,b,a)(a has a default value of 1.0 if you don't specify)\n"
				"\t\tcolors =\n"

				// Opening Colors Table
				"\t\t{\n";
			for (size_t i = 0; i < numberOfVertices; i++)
			{
				fout << "\t\t\t{" << i_vertexBuffer[i].vertex.r << "," << i_vertexBuffer[i].vertex.g << "," << i_vertexBuffer[i].vertex.b << "," << i_vertexBuffer[i].vertex.a << "},\n";
			}
			fout <<
				// Closing Colors Table				
				"\t\t},\n"

				"\t\t-- This table contains normals(nx,ny,nz)\n"
				"\t\tnormals =\n"

				// Opening Normals Table
				"\t\t{\n";
			for (size_t i = 0; i < numberOfVertices; i++)
			{
				fout << "\t\t\t{" << i_vertexBuffer[i].vertex.nx << "," << i_vertexBuffer[i].vertex.ny << "," << i_vertexBuffer[i].vertex.nz << "},\n";
			}
			fout <<
				// Closing Normals Table
				"\t\t},\n"

				"\t\t-- This table contains tangents(tx,ty,tz)\n"
				"\t\ttangents =\n"

				// Opening Tangents Table
				"\t\t{\n";
			for (size_t i = 0; i < numberOfVertices; i++)
			{
				fout << "\t\t\t{" << i_vertexBuffer[i].vertex.tx << "," << i_vertexBuffer[i].vertex.ty << "," << i_vertexBuffer[i].vertex.tz << "},\n";
			}
			fout <<
				// Closing Tangents Table
				"\t\t},\n"

				"\t\t-- This table contains bitangents(btx,bty,btz)\n"
				"\t\tbitangents =\n"

				// Opening Bitangents Table
				"\t\t{\n";
			for (size_t i = 0; i < numberOfVertices; i++)
			{
				fout << "\t\t\t{" << i_vertexBuffer[i].vertex.btx << "," << i_vertexBuffer[i].vertex.bty << "," << i_vertexBuffer[i].vertex.btz << "},\n";
			}
			fout <<
				// Closing Bitangents Table
				"\t\t},\n"

				"\t\t-- This table contains texture coordinates(u,v)\n"
				"\t\ttexture_coordinates =\n"

				// Opening Texture Coordinates Table
				"\t\t{\n";
			for (size_t i = 0; i < numberOfVertices; i++)
			{
				fout << "\t\t\t{" << i_vertexBuffer[i].vertex.u << "," << i_vertexBuffer[i].vertex.v << "},\n";
			}
			fout <<
				// Closing Texture Coordinates Table
				"\t\t},\n"

				/*"\t\t-- This table contains shading groups\n"
				"\t\tshading_groups = \n"

				// Opening Shading Groups Table
				"\t\t{\n"
				"\t\t\t";
			for (size_t i = 0; i < numberOfVertices; i++)
			{
				fout << i_vertexBuffer[i].shadingGroup << ",";
			}
			fout <<
				// Closing Shading Groups Table
				"\n\t\t},\n"

				"\t\t-- This table contains unique keys\n"
				"\t\tunique_keys = \n"

				// Opening Unique Keys Table
				"\t\t{\n";
			for (size_t i = 0; i < numberOfVertices; i++)
			{
				fout << "\t\t\t" << i_vertexBuffer[i].uniqueKey << ",\n";
			}
			fout <<
				// Closing Unique Keys Table
				"\t\t},\n"*/

				// Closing Vertices Table
				"\t},\n"

				"\t-- This table contains indices(triangles)\n"
				"\t-- Total number of indices: " << numberOfIndices << "\n"
				"\t-- Total number of triangles: " << static_cast<int>(numberOfIndices / 3) << "\n"
				"\tindices =\n"

				// Opening Indices Table
				"\t{\n";
			for (size_t i = 0; i < numberOfIndices;)
			{
				fout << "\t\t" << i_indexBuffer[i] << "," << i_indexBuffer[i + 1] << "," << i_indexBuffer[i + 2] << ",\n";
				i += 3;
			}
			fout <<
				// Closing Indices Table
				"\t},\n"

				// Closing Main Table
				"}\n";
			fout.close();

			return MStatus::kSuccess;
		}
		else
		{
			MGlobal::displayError(MString("Couldn't open ") + i_fileName + " for writing");
			return MStatus::kFailure;
		}
	}
}
