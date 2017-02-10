/*
	A sprite is a quad (two triangle square) drawn in 2D
*/

#ifndef EAE6320_GRAPHICS_CSPRITE_H
#define EAE6320_GRAPHICS_CSPRITE_H

// Header Files
//=============

#include "SpriteData.h"

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

			void SetScreenPosition(const Sprite::ScreenPosition i_position_screen);
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
