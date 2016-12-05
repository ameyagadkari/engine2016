#ifndef EAE6320_GRAPHICS_CTEXTURE_INL
#define EAE6320_GRAPHICS_CTEXTURE_INL

// Header Files
//=============

//#include "../cTexture.h"

// Interface
//==========

// Access
//-------

inline uint16_t eae6320::Graphics::cTexture::GetWidth() const
{
	return m_width;
}

inline uint16_t eae6320::Graphics::cTexture::GetHeight() const
{
	return m_height;
}

#endif	// EAE6320_GRAPHICS_CTEXTURE_INL
