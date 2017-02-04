#ifndef EAE6320_SPRITEDATA_H
#define EAE6320_SPRITEDATA_H

#include "../Math/cHalf.h"

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

#endif // !EAE6320_SPRITEDATA_H
