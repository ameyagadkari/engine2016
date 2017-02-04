#ifndef EAE6320_MESHDATA_H
#define EAE6320_MESHDATA_H

#include <cstdint>
#include <cstdlib>

namespace eae6320
{
	namespace Graphics
	{
		struct MeshData
		{
			struct Vertex
			{
				float x, y, z;
				uint16_t u, v;
				uint8_t r, g, b, a;
				void AddVertexData(const float i_x, const float i_y, const float i_z)
				{
					x = i_x;
					y = i_y;
					z = i_z;
					r = 255;
					g = 255;
					b = 255;
					a = 255;
					u = 0;
					v = 0;
				}
				void AddVertexData(const float i_x, const float i_y, const float i_z, uint16_t i_u, uint16_t i_v)
				{
					x = i_x;
					y = i_y;
					z = i_z;
					r = 255;
					g = 255;
					b = 255;
					a = 255;
					u = i_u;
					v = i_v;
				}
			};
			Vertex *vertexData;
			void *indexData;
			uint32_t numberOfVertices;
			uint32_t numberOfIndices;
			uint32_t typeOfIndexData;
			MeshData() :
				vertexData(nullptr),
				indexData(nullptr),
				numberOfVertices(0),
				numberOfIndices(0),
				typeOfIndexData(0)
			{}
			MeshData(const uint32_t i_typeOfIndexData, const uint32_t i_numberOfVertices, const uint32_t i_numberOfIndices) :
				vertexData(reinterpret_cast<Vertex*>(malloc(i_numberOfVertices * sizeof(Vertex)))),
				numberOfVertices(i_numberOfVertices),
				numberOfIndices(i_numberOfIndices),
				typeOfIndexData(i_typeOfIndexData)
			{
				if (i_typeOfIndexData == 16)
				{
					indexData = reinterpret_cast<uint16_t*>(malloc(i_numberOfIndices * sizeof(uint16_t)));
				}
				else
				{
					indexData = reinterpret_cast<uint32_t*>(malloc(i_numberOfIndices * sizeof(uint32_t)));
				}
			}
			~MeshData()
			{
				if (vertexData)
				{
					free(vertexData);
					vertexData = nullptr;
				}
				if (indexData)
				{
					free(indexData);
					indexData = nullptr;
				}
			}
		};
	}
}
#endif // !EAE6320_MESHDATA_H
