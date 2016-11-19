#ifndef EAE6320_CONSTANT_BUFFER_H
#define EAE6320_CONSTANT_BUFFER_H

#if defined( EAE6320_PLATFORM_D3D )
#include <D3D11.h>
#elif defined( EAE6320_PLATFORM_GL )
#include "OpenGL\Includes.h"
#endif	
namespace eae6320
{
	namespace Graphics
	{
		enum struct ConstantBufferType { FRAME, DRAWCALL, MATERIAL };
		enum struct BindMode { VS_ONLY, PS_ONLY, VS_PS_BOTH };
		class ConstantBuffer
		{
		public:
			bool InitializeConstantBuffer(ConstantBufferType constantBufferType, size_t sizeOfConstantBuffer, void* initialBufferData = NULL);
			void BindingConstantBuffer(BindMode bindMode = BindMode::VS_PS_BOTH);
			void UpdateConstantBuffer(void* bufferData, size_t sizeOfConstantBuffer);
			bool CleanUpConstantBuffer();

		private:
			bool CreateConstantBuffer(void* initialBufferData);
#if defined( EAE6320_PLATFORM_D3D )
			ID3D11Buffer* s_constantBuffer;
#elif defined( EAE6320_PLATFORM_GL ) 
			GLuint s_constantBufferId;
#endif
			ConstantBufferType constantBufferType;
			size_t sizeOfConstantBuffer;		
		};
	}
}


#endif // !EAE6320_CONSTANT_BUFFER_H
