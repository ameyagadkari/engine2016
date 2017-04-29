// Header Files
//=============

#include "cMeshBuilder.h"

#include <sstream>
#include <iostream>
#include <climits>
//#include <chrono>

#include "../../Engine/Asserts/Asserts.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Graphics/MeshData.h"
#include "../../Engine/Math/cHalf.h"


// Inherited Implementation
//=========================

// Build
//------

// Helper Function Declarations
//=============================

namespace
{
	using namespace eae6320::Graphics;

	bool LoadInitialTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadVerticesTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadPositionsTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadXYZTable(lua_State& io_luaState, MeshData&meshData, size_t index);
	bool LoadColorsTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadRGBATable(lua_State& io_luaState, MeshData&meshData, size_t index);
	bool LoadIndicesTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadTextureCoordinatesTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadUVTable(lua_State& io_luaState, MeshData&meshData, size_t index);
	bool LoadNormalsTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadNXNYNZTable(lua_State& io_luaState, MeshData&meshData, size_t index);
}

bool eae6320::AssetBuild::cMeshBuilder::Build(const std::vector<std::string>& i_arguments)
{
	//auto begin = std::chrono::high_resolution_clock::now();

	bool wereThereErrors = false;
	std::string errorMessage;
	MeshData *meshData = NULL;
	FILE * outputFile = NULL;

	// Create a new Lua state
	lua_State* luaState = NULL;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			wereThereErrors = true;
			OutputErrorMessage("Failed to create a new Lua state", __FILE__);
			goto OnExit;
		}
	}

	// Creating/Opening file to write in binary mode
	outputFile = fopen(m_path_target, "wb");
	if (!outputFile)
	{
		perror("The target file cannot be opened for writing out data");
		wereThereErrors = true;
		goto OnExit;
	}

	// Open the standard libraries
	luaL_openlibs(luaState);

	// Load and execute it
	{
		if (Platform::DoesFileExist(m_path_source, &errorMessage))
		{

			// Load the asset file as a "chunk",
			// meaning there will be a callable function at the top of the stack
			const int stackTopBeforeLoad = lua_gettop(luaState);
			{
				const int luaResult = luaL_loadfile(luaState, m_path_source);
				if (luaResult != LUA_OK)
				{
					wereThereErrors = true;
					std::cerr << lua_tostring(luaState, -1) << std::endl;
					// Pop the error message
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			// Execute the "chunk", which should load the asset
			// into a table at the top of the stack
			{
				const int argumentCount = 0;
				const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
				const int noErrorHandler = 0;
				const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noErrorHandler);
				if (luaResult == LUA_OK)
				{
					// A well-behaved asset file will only return a single value
					const int returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
					if (returnedValueCount == 1)
					{
						// A correct asset file _must_ return a table
						if (!lua_istable(luaState, -1))
						{
							wereThereErrors = true;
							std::cerr << "Asset files must return a table (instead of a "
								<< luaL_typename(luaState, -1) << ")" << std::endl;
							// Pop the returned non-table value
							lua_pop(luaState, 1);
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::cerr << "Asset files must return a single table (instead of "
							<< returnedValueCount << " values)" << std::endl;
						// Pop every value that was returned
						lua_pop(luaState, returnedValueCount);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::cerr << lua_tostring(luaState, -1) << std::endl;
					// Pop the error message
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			OutputErrorMessage(errorMessage.c_str(), m_path_source);
			goto OnExit;
		}
	}

	//If this code is reached the asset file was loaded successfully,
	//and its table is now at index -1

	meshData = reinterpret_cast<Graphics::MeshData*>(malloc(sizeof(Graphics::MeshData)));
	if (!LoadInitialTable(*luaState, *meshData))
	{
		wereThereErrors = true;
		OutputErrorMessage("Failed to load initial table", __FILE__);
		goto OnExit;
	}
	lua_pop(luaState, 1);

	// Writing data to file 
	{
		// Writing Type of index array to file	
		fwrite(&meshData->typeOfIndexData, sizeof(uint32_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing type of index array to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Number Of Vertices
		fwrite(&meshData->numberOfVertices, sizeof(uint32_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing number of indices to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Number Of Indices
		fwrite(&meshData->numberOfIndices, sizeof(uint32_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing number of indices to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Vertex Array
		fwrite(meshData->vertexData, sizeof(MeshData::Vertex), meshData->numberOfVertices, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing vertex array to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Index Array
		if (meshData->typeOfIndexData == 16)
		{
			fwrite(meshData->indexData, sizeof(uint16_t), meshData->numberOfIndices, outputFile);
			if (ferror(outputFile))
			{
				fprintf_s(stderr, "Error writing index array to %s \n", m_path_target);
				wereThereErrors = true;
				goto OnExit;
			}
		}
		else
		{
			fwrite(meshData->indexData, sizeof(uint32_t), meshData->numberOfIndices, outputFile);
			if (ferror(outputFile))
			{
				fprintf_s(stderr, "Error writing index array to %s \n", m_path_target);
				wereThereErrors = true;
				goto OnExit;
			}
		}
	}

OnExit:
	// Closing file only if it got opened
	if (outputFile)
	{
		if (fclose(outputFile))
		{
			wereThereErrors = true;
			OutputErrorMessage("Failed to close target file", __FILE__);
		}
	}

	if (meshData)
	{
		if (meshData->indexData)
		{
			free(meshData->indexData);
		}
		if (meshData->vertexData)
		{
			free(meshData->vertexData);
		}
		free(meshData);
	}

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack
		// regardless of any errors
		EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = NULL;
	}
	//auto end = std::chrono::high_resolution_clock::now();
	//auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	//std::cerr << ms << std::endl;
	return !wereThereErrors;
}

// Helper Function Definitions
//=============================

namespace
{
	bool LoadInitialTable(lua_State& io_luaState, MeshData&meshData)
	{
		if (!LoadVerticesTable(io_luaState, meshData))
		{
			return false;
		}
		if (!LoadIndicesTable(io_luaState, meshData))
		{
			return false;
		}
		return true;
	}
	bool LoadVerticesTable(lua_State& io_luaState, MeshData&meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "vertices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			if (!LoadPositionsTable(io_luaState, meshData))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!LoadColorsTable(io_luaState, meshData))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!LoadTextureCoordinatesTable(io_luaState, meshData))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!LoadNormalsTable(io_luaState, meshData))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:

		// Pop the Vertices table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;

	}
	bool LoadPositionsTable(lua_State& io_luaState, MeshData&meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "positions";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const int positions = luaL_len(&io_luaState, -1);
			meshData.numberOfVertices = positions;
			meshData.vertexData = reinterpret_cast<MeshData::Vertex*>(malloc(meshData.numberOfVertices * sizeof(MeshData::Vertex)));
			if (!meshData.vertexData)
			{
				wereThereErrors = true;
				fprintf_s(stderr, "Could not initialize the Vertex Data");
				goto OnExit;
			}
			for (size_t i = 1; i <= meshData.numberOfVertices; ++i)
			{
				if (!LoadXYZTable(io_luaState, meshData, (i - 1)))
				{
					wereThereErrors = true;
					goto OnExit;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:

		// Pop the Positions table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadXYZTable(lua_State& io_luaState, MeshData&meshData, size_t index)
	{
		bool wereThereErrors = false;
		lua_pushinteger(&io_luaState, index + 1);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:%zu was found in the table", (index + 1));
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
			float xyz[3] = { 0.0f,0.0f,0.0f };
			if (arrayLength == 3)
			{
				for (size_t i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						fprintf_s(stderr, "No value for key:%zu was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
					{
						xyz[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
					}
					else
					{
						wereThereErrors = true;
						fprintf_s(stderr, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
				meshData.vertexData[index].x = xyz[0];
				meshData.vertexData[index].y = xyz[1];
				meshData.vertexData[index].z = xyz[2];
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "There are %zu coordinates instead of 3", arrayLength);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%zu\" must be a table (instead of a %s) ", (index + 1), luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the XY table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadNormalsTable(lua_State& io_luaState, MeshData&meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "normals";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const int normals = luaL_len(&io_luaState, -1);
			if (normals != meshData.numberOfVertices)
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The number of normals tables and position tables do not match");
				goto OnExit;
			}
			else
			{
				for (size_t i = 1; i <= meshData.numberOfVertices; ++i)
				{
					if (!LoadNXNYNZTable(io_luaState, meshData, (i - 1)))
					{
						wereThereErrors = true;
						goto OnExit;
					}
				}
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:

		// Pop the Normals table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadNXNYNZTable(lua_State& io_luaState, MeshData&meshData, size_t index)
	{
		bool wereThereErrors = false;
		lua_pushinteger(&io_luaState, index + 1);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:%zu was found in the table", (index + 1));
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
			float nxnynz[3] = { 0.0f,0.0f,0.0f };
			if (arrayLength == 3)
			{
				for (size_t i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						fprintf_s(stderr, "No value for key:%zu was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
					{
						nxnynz[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
					}
					else
					{
						wereThereErrors = true;
						fprintf_s(stderr, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
				meshData.vertexData[index].nx = nxnynz[0];
				meshData.vertexData[index].ny = nxnynz[1];
				meshData.vertexData[index].nz = nxnynz[2];
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "There are %zu coordinates instead of 3", arrayLength);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%zu\" must be a table (instead of a %s) ", (index + 1), luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the NXNYNZ table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadColorsTable(lua_State& io_luaState, MeshData&meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "colors";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const int colors = luaL_len(&io_luaState, -1);
			if (colors != meshData.numberOfVertices)
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The number of color tables and position tables do not match");
				goto OnExit;
			}
			else
			{
				for (size_t i = 1; i <= meshData.numberOfVertices; ++i)
				{
					if (!LoadRGBATable(io_luaState, meshData, (i - 1)))
					{
						wereThereErrors = true;
						goto OnExit;
					}
				}
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the Colors table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadRGBATable(lua_State& io_luaState, MeshData&meshData, size_t index)
	{
		bool wereThereErrors = false;
		lua_pushinteger(&io_luaState, index + 1);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:%zu was found in the table", (index + 1));
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
			float rgba[4] = { 0.0f,0.0f,0.0f,1.0f };
			if ((arrayLength == 3) || (arrayLength == 4))
			{
				for (size_t i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						fprintf_s(stderr, "No value for key:%zu was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
					{
						rgba[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
					}
					else
					{
						wereThereErrors = true;
						fprintf_s(stderr, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}

				if (eae6320::AssetBuild::CheckIfNumberIsNormalized(rgba, 4))
				{
					meshData.vertexData[index].r = static_cast<uint8_t>(roundf(rgba[0] * 255.0f));
					meshData.vertexData[index].g = static_cast<uint8_t>(roundf(rgba[1] * 255.0f));
					meshData.vertexData[index].b = static_cast<uint8_t>(roundf(rgba[2] * 255.0f));
					meshData.vertexData[index].a = static_cast<uint8_t>(roundf(rgba[3] * 255.0f));
				}
				else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "The color values were not normalized in range [0,1]");
					goto OnExit;
				}

			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "There are %zu channels instead of 3 or 4", arrayLength);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%zu\" must be a table (instead of a %s)", (index + 1), luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the RGBA table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadIndicesTable(lua_State& io_luaState, MeshData&meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "indices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
			if (arrayLength % 3 == 0)
			{
				meshData.numberOfIndices = static_cast<uint32_t>(arrayLength);
				if (meshData.numberOfIndices > USHRT_MAX)
				{
					meshData.typeOfIndexData = 32;
					meshData.indexData = malloc(meshData.numberOfIndices * sizeof(uint32_t));
				}
				else
				{
					meshData.typeOfIndexData = 16;
					meshData.indexData = malloc(meshData.numberOfIndices * sizeof(uint16_t));
				}
				// Remember that Lua arrays are 1-based and not 0-based!
				for (size_t i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						fprintf_s(stderr, "No value for key:%zu was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
					{
#if defined( EAE6320_PLATFORM_D3D )
						if (meshData.typeOfIndexData == 16)
						{
							reinterpret_cast<uint16_t*>(meshData.indexData)[meshData.numberOfIndices - i] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
						}
						else
						{
							reinterpret_cast<uint32_t*>(meshData.indexData)[meshData.numberOfIndices - i] = static_cast<uint32_t>(lua_tonumber(&io_luaState, -1));
						}
#elif defined( EAE6320_PLATFORM_GL )
						if (meshData.typeOfIndexData == 16)
						{
							reinterpret_cast<uint16_t*>(meshData.indexData)[i - 1] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
						}
						else
						{
							reinterpret_cast<uint32_t*>(meshData.indexData)[i - 1] = static_cast<uint32_t>(lua_tonumber(&io_luaState, -1));
						}
#endif
						lua_pop(&io_luaState, 1);
					}
					else
					{
						wereThereErrors = true;
						fprintf_s(stderr, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "There are %zu indices which is incorrect as we are drawing triangles", arrayLength);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the Indices table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadTextureCoordinatesTable(lua_State & io_luaState, MeshData & meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "texture_coordinates";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const int UVs = luaL_len(&io_luaState, -1);
			if (UVs != meshData.numberOfVertices)
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The number of texture coordinates(UVs) do not match number of vertices");
				goto OnExit;
			}
			else
			{
				for (size_t i = 1; i <= meshData.numberOfVertices; ++i)
				{
					if (!LoadUVTable(io_luaState, meshData, (i - 1)))
					{
						wereThereErrors = true;
						goto OnExit;
					}
				}
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:

		// Pop the Texture Coordinates table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadUVTable(lua_State & io_luaState, MeshData & meshData, size_t index)
	{
		bool wereThereErrors = false;
		lua_pushinteger(&io_luaState, index + 1);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:%zu was found in the table", (index + 1));
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
			float uv[2] = { 0.0f,0.0f };
			if (arrayLength == 2)
			{
				for (size_t i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						fprintf_s(stderr, "No value for key:%zu was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
					{
						uv[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
					}
					else
					{
						wereThereErrors = true;
						fprintf_s(stderr, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}

				//if (eae6320::AssetBuild::CheckIfNumberIsNormalized(uv, 2))
				{
					meshData.vertexData[index].u = eae6320::Math::cHalf::MakeHalfFromFloat(uv[0]);
#if defined( EAE6320_PLATFORM_D3D )
					meshData.vertexData[index].v = eae6320::Math::cHalf::MakeHalfFromFloat(1.0f - uv[1]);
#elif defined( EAE6320_PLATFORM_GL )
					meshData.vertexData[index].v = eae6320::Math::cHalf::MakeHalfFromFloat(uv[1]);
#endif
				}
				/*else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "The UVs were not normalized in range [0,1]");
					goto OnExit;
				}*/

			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "here are %zu coordinates instead of 2", arrayLength);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%zu\" must be a table (instead of a %s)", (index + 1), luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the UV table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
}