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
				uint8_t r, g, b, a;
			};
			Vertex *vertexData = NULL;
			void *indexData = NULL;
			uint32_t numberOfVertices = 0;
			uint32_t numberOfIndices = 0;
			uint32_t typeOfIndexData = 0;
		};
	}
}
#endif // !EAE6320_MESHDATA_H
