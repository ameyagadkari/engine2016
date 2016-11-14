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

namespace eae6320
{
	namespace Graphics
	{
		class Effect
		{
		public:
			static bool LoadEffect(const char * const relativePath, Effect&effect);
			bool InitializeEffect(const char * const relativeVertexShaderPath, const char * const relativeFragmentShaderPath);
			bool CleanUpEffect();
			void BindEffect();

		private:
#if defined( EAE6320_PLATFORM_D3D )
			ID3D11VertexShader* s_vertexShader = NULL;
			ID3D11PixelShader* s_fragmentShader = NULL;
			ID3D11InputLayout* s_vertexLayout = NULL;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint s_programId = 0;
#endif
		};
	}
}

#endif	// EAE6320_EFFECT_H