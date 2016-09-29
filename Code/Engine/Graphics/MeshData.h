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
				float x, y;
				uint8_t r, g, b, a;
			};

			Vertex *vertexData = nullptr;
			uint16_t *indexData = nullptr;
			uint16_t numberOfVertices = 0;
			uint16_t numberOfIndices = 0;
		};
	}
}
#endif // !EAE6320_MESHDATA_H
