/*
This file manages effect-related functionality
*/

#ifndef EAE6320_EFFECT_H
#define EAE6320_EFFECT_H

#if defined( EAE6320_PLATFORM_D3D )
#include <D3D11.h>
#elif defined( EAE6320_PLATFORM_GL )
#include "OpenGL\Includes.h"
#endif	

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
#if defined( EAE6320_PLATFORM_D3D )
			ID3D11VertexShader* m_vertexShader;
			ID3D11PixelShader* m_fragmentShader;
			ID3D11InputLayout* m_vertexLayout;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint m_programId;
#endif
		};
	}
}

#endif	// EAE6320_EFFECT_H