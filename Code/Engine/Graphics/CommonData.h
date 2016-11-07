/*
This file manages common functionality
*/

#ifndef EAE6320_COMMON_DATA_H
#define EAE6320_COMMON_DATA_H


#if defined( EAE6320_PLATFORM_D3D )
#include <D3D11.h>
#endif	

namespace eae6320
{
	namespace Graphics
	{
		class CommonData
		{
		public:
			static CommonData* GetCommonData();
			static bool Initialize();
			static bool CleanUp();
			inline virtual ~CommonData();
#if defined( EAE6320_PLATFORM_D3D )
			ID3D11Device* s_direct3dDevice = NULL;
			ID3D11DeviceContext* s_direct3dImmediateContext = NULL;
#endif	
		private:
			inline CommonData();
			static CommonData *commonData;
		};
	}
}
#endif // EAE6320_COMMON_DATA_H
