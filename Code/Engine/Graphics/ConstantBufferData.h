#ifndef EAE6320_CONSTANT_BUFFER_DATA_H
#define EAE6320_CONSTANT_BUFFER_DATA_H

namespace eae6320
{
	namespace Graphics
	{
		struct ConstantBufferData
		{
			struct sFrame
			{
				union
				{
					float g_elapsedSecondCount_total;
					float register0[4];
				};
			};
			struct sDrawCall
			{
				union
				{
					float g_objectPosition_screen[2];
					float register0[4];
				};
			};
		};		
	}
}

#endif // !EAE6320_CONSTANT_BUFFER_DATA_H
