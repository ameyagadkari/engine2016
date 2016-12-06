/*
	A sprite is a quad (two triangle square) drawn in 2D
*/

#ifndef EAE6320_GRAPHICS_CSPRITE_H
#define EAE6320_GRAPHICS_CSPRITE_H

// Header Files
//=============

#include <cstdint>
#include "../Math/cHalf.h"

// Forward Declarations
//=====================

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11Buffer;
#elif defined( EAE6320_PLATFORM_GL )
typedef unsigned int GLuint;
//#include "Configuration.h"
#endif

// Helper Structs for Sprites
//===========================
namespace eae6320
{
	namespace Graphics
	{
		namespace Sprite
		{
			// The positions and texture coordinates are stored as a quad
			// (meaning that only rectangular sprites can be made),
			// and are converted into an sVertex when the draw call is made
			struct ScreenPosition
			{
				// Left of screen = -1
				// Right of screen = 1
				// Top of screen = 1
				// Bottom of screen = -1
				float left, right, top, bottom;
			};
			struct TextureCoordinates
			{
				// Left of texture = 0
				// Right of texture = 1
				// Top of texture = 1 (OpenGL-style)
				// Bottom of texture = 0 (OpenGL-style)
				uint16_t left, right, top, bottom;
				TextureCoordinates(float left = 0.0f, float right = 1.0f, float top = 1.0f, float bottom = 0.0f) :
					left(Math::cHalf::MakeHalfFromFloat(left)),
					right(Math::cHalf::MakeHalfFromFloat(right)),
#if defined( EAE6320_PLATFORM_D3D )
					top(Math::cHalf::MakeHalfFromFloat(1.0f - top)),
					bottom(Math::cHalf::MakeHalfFromFloat(1.0f - bottom))
#elif defined( EAE6320_PLATFORM_GL )
					top(Math::cHalf::MakeHalfFromFloat(top)),
					bottom(Math::cHalf::MakeHalfFromFloat(bottom))
#endif
				{}
			};
		}
	}
}


// Class Declaration
//==================

namespace eae6320
{
	namespace Graphics
	{
		class cSprite
		{
			// Interface
			//==========

		public:

			// Render
			//-------

			void Draw() const;

			// Initialization / Clean Up
			//--------------------------

			cSprite(const Sprite::ScreenPosition i_screenPosition, const Sprite::TextureCoordinates i_textureCoordinates);
			~cSprite();

			// Data
			//=====

		private:
			static bool iscSpriteInitialized;
			static bool iscSpriteCleanedUp;

			static bool Initialize();
			static bool CleanUp();

			static uint16_t zeroAsHalf;
			static uint16_t oneAsHalf;

			Sprite::ScreenPosition m_position_screen;
			Sprite::TextureCoordinates m_textureCoordinates;

			// A single dynamic vertex buffer is created for _all_ sprites,
			// and its contents are changed every draw call
#if defined( EAE6320_PLATFORM_D3D )
			// A vertex buffer holds the data for each vertex
			static ID3D11Buffer* ms_vertexBuffer;
#elif defined( EAE6320_PLATFORM_GL )
			// A vertex array encapsulates the vertex data and the vertex input layout
			static GLuint ms_vertexArrayId;
			static GLuint ms_vertexBufferId;
#endif
		};
	}
}

#endif	// EAE6320_GRAPHICS_CSPRITE_H
