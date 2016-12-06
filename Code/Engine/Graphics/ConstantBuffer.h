#ifndef EAE6320_CONSTANT_BUFFER_H
#define EAE6320_CONSTANT_BUFFER_H

#if defined( EAE6320_PLATFORM_D3D )
struct ID3D11Buffer;
#elif defined( EAE6320_PLATFORM_GL )
typedef unsigned int GLuint;
#endif	

#ifndef NULL
#define NULL 0
#endif // !NULL

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
			void BindingConstantBuffer(BindMode bindMode = BindMode::VS_PS_BOTH)const;
			void UpdateConstantBuffer(void* bufferData, size_t sizeOfConstantBuffer);
			bool CleanUpConstantBuffer();

			ConstantBuffer();
			~ConstantBuffer();

		private:
			bool CreateConstantBuffer(void* initialBufferData);
#if defined( EAE6320_PLATFORM_D3D )
			ID3D11Buffer* m_constantBuffer;
#elif defined( EAE6320_PLATFORM_GL ) 
			GLuint m_constantBufferId;
#endif
			ConstantBufferType m_constantBufferType;
			size_t m_sizeOfConstantBuffer;		
		};
	}
}


#endif // !EAE6320_CONSTANT_BUFFER_H
