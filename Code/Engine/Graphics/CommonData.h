/*
This file manages common functionality
*/

#ifndef EAE6320_COMMONDATA_H
#define EAE6320_COMMONDATA_H

namespace
{

#if defined( EAE6320_PLATFORM_D3D )
#include <D3D11.h>
#endif	
	class CommonData
	{
	public:
		static CommonData* GetCommonData();
		static void CleanUp();
		inline virtual ~CommonData();
#if defined( EAE6320_PLATFORM_D3D )
		ID3D11Device* s_direct3dDevice = NULL;
		ID3D11DeviceContext* s_direct3dImmediateContext = NULL;
		ID3D11InputLayout* s_vertexLayout = NULL;
		ID3D10Blob* compiledVertexShader = NULL;
#endif	
		// This struct determines the layout of the geometric data that the CPU will send to the GPU
		struct sVertex
		{
			// POSITION
			// 2 floats == 8 bytes
			// Offset = 0
			float x, y;
		};

	private:
		inline CommonData();
		static CommonData *commonData;
	};
}

#endif // EAE6320_COMMONDATA_H
