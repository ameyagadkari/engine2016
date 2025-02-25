/*
	A texture is an n-dimensional array of data
	(usually a 2-dimensional array of colors)
	that can be sampled in a shader
*/

#ifndef EAE6320_GRAPHICS_CTEXTURE_H
#define EAE6320_GRAPHICS_CTEXTURE_H

// Header Files
//=============

//#include "Configuration.h"
#include <cstdint>

#ifdef EAE6320_PLATFORM_GL
typedef unsigned int GLuint;
#endif

// Forward Declarations
//=====================

namespace eae6320
{
	namespace Graphics
	{
		struct sDdsInfo;
	}

	namespace Platform
	{
		struct sDataFromFile;
	}
}
namespace DirectX
{
	struct DDS_HEADER;
}

#ifdef EAE6320_PLATFORM_D3D
	struct ID3D11ShaderResourceView;
#endif

// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cTexture
		{
			// Interface
			//==========

		public:

			// Render
			//-------

			// The ID is the HLSL register or GLSL layout ID
			// defined in the shader that uses this texture.
			// The texture doesn't know what this is
			// (a single texture could be used by many different effects)
			// and so this is the caller's responsibility to pass in.
			void Bind( const unsigned int i_id ) const;

			// Access
			//-------

			uint16_t GetWidth() const;
			uint16_t GetHeight() const;

			// Initialization / Clean Up
			//--------------------------

			static bool Load( const char* const i_relativePath, cTexture& o_texture );
			bool CleanUp();

			cTexture();
			~cTexture();

			// Data
			//=====

		private:

#if defined( EAE6320_PLATFORM_D3D )
			ID3D11ShaderResourceView* m_textureView;
#elif defined( EAE6320_PLATFORM_GL )
			GLuint m_textureId;
#endif
			uint16_t m_width;
			uint16_t m_height;
			// Implementation
			//===============

		private:

			// Initialization / Clean Up
			//--------------------------

			bool Initialize( const char* const i_path, const sDdsInfo& i_ddsInfo );
		};
	}
}

#include "cTexture/cTexture.inl"

#endif	// EAE6320_GRAPHICS_CTEXTURE_H
