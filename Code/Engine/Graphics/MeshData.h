#ifndef EAE6320_MESHDATA_H
#define EAE6320_MESHDATA_H

#include<cstdint>

namespace eae6320
{
	namespace Graphics
	{
		struct MeshData
		{
			struct Vertex
			{
				float x, y, z;
				float u, v;
				uint8_t r, g, b, a;
			};
			Vertex *vertexData;
			void *indexData;
			uint32_t numberOfVertices;
			uint32_t numberOfIndices;
			uint32_t typeOfIndexData;
			MeshData() :
				vertexData(NULL),
				indexData(NULL),
				numberOfVertices(0),
				numberOfIndices(0),
				typeOfIndexData(0)
			{}
		};
	}
}
#endif // !EAE6320_MESHDATA_H
