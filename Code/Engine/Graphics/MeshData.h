#ifndef EAE6320_MESHDATA_H
#define EAE6320_MESHDATA_H

#include<cstdint>
#include<cmath>

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
				void AddVertexData(const float i_x, const float i_y, const float i_z, const float i_r, const float i_g, const float i_b)
				{
					x = i_x;
					y = i_y;
					z = i_z;
					r = static_cast<uint8_t>(roundf(i_r * 255.0f));
					g = static_cast<uint8_t>(roundf(i_g * 255.0f));
					b = static_cast<uint8_t>(roundf(i_b * 255.0f));
					a = 255;
					u = 0;
					v = 0;
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
					free(vertexData);
				if (indexData)
					free(indexData);
			}
		};
	}
}
#endif // !EAE6320_MESHDATA_H
