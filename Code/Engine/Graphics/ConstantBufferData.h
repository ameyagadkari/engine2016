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
			struct sMaterial
			{
				struct sColor
				{
					float r, g, b, a;
					sColor() :r(1.0f), g(1.0f), b(1.0f), a(1.0f)
					{}
				} g_color;
				sMaterial() :g_color()
				{}
			};
		};
	}
}

#endif // !EAE6320_CONSTANT_BUFFER_DATA_H
