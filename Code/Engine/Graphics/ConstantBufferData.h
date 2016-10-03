#ifndef EAE6320_CONSTANT_BUFFER_DATA_H
#define EAE6320_CONSTANT_BUFFER_DATA_H

#include "../Math/cMatrix_transformation.h"

namespace eae6320
{
	namespace Graphics
	{
		struct ConstantBufferData
		{
			struct sFrame
			{
				Math::cMatrix_transformation g_transform_worldToCamera;
				Math::cMatrix_transformation g_transform_cameraToScreen;
				union
				{
					float g_elapsedSecondCount_total;
					float register8[4];
				};
			};
			struct sDrawCall
			{
				Math::cMatrix_transformation g_transform_localToWorld;
			};
		};		
	}
}

#endif // !EAE6320_CONSTANT_BUFFER_DATA_H
