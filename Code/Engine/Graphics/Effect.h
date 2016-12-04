/*
This file manages effect-related functionality
*/

#ifndef EAE6320_EFFECT_H
#define EAE6320_EFFECT_H

#if defined( EAE6320_PLATFORM_D3D )
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
#elif defined( EAE6320_PLATFORM_GL )
typedef unsigned int GLuint;
#endif	

#include "cRenderState.h"
#include <cstdint>
namespace eae6320
{
	namespace Graphics
	{
		class Effect
		{
		public:
			static bool LoadEffect(const char * const i_relativePath, Effect& o_effect);
			
			bool CleanUpEffect();
			void BindEffect()const;

			uint32_t GetEffectUUID()const;

			Effect();
			~Effect();

		private:
			uint32_t effectUUID;
			bool LoadShaders(const char * const i_relativeVertexShaderPath, const char * const i_relativeFragmentShaderPath);
			cRenderState m_renderState;
#if defined( EAE6320_PLATFORM_D3D )
			ID3D11VertexShader* m_vertexShader;
			ID3D11PixelShader* m_fragmentShader;
			//Create only one as the vertex layout is same for all shaders
			static ID3D11InputLayout* ms_vertexLayout;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint m_programId;
#endif
		};
	}
}

#endif	// EAE6320_EFFECT_H