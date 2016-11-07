#ifndef EAE6320_MESHDATA_H
#define EAE6320_MESHDATA_H

#include<cstdint>

namespace eae6320
{
	namespace Graphics
	{
		template<typename T>struct MeshData
		{
			struct Vertex
			{
				float x, y, z;
				uint8_t r, g, b, a;
			};

			Vertex *vertexData = NULL;
			T *indexData = NULL;
			T numberOfVertices = 0;
			T numberOfIndices = 0;
		};
	}
}
#endif // !EAE6320_MESHDATA_H
