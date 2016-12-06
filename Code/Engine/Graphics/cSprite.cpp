// Header Files
//=============

#include "cSprite.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"

// Static Data Initialization
//===========================

bool eae6320::Graphics::cSprite::iscSpriteInitialized = false;
bool eae6320::Graphics::cSprite::iscSpriteCleanedUp = false;
uint16_t eae6320::Graphics::cSprite::zeroAsHalf = eae6320::Math::cHalf::MakeHalfFromFloat(0.0f);
uint16_t eae6320::Graphics::cSprite::oneAsHalf = eae6320::Math::cHalf::MakeHalfFromFloat(1.0f);

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::Graphics::cSprite::cSprite(const Sprite::ScreenPosition i_screenPosition, const Sprite::TextureCoordinates i_textureCoordinates) :
	m_position_screen(i_screenPosition),
	m_textureCoordinates(i_textureCoordinates)
{
	//m_position_screen.left = i_screenPosition.left;
	//m_position_screen.right = i_position_right;
	//m_position_screen.top = i_position_top;
	//m_position_screen.bottom = i_position_bottom;

	//m_textureCoordinates.left = i_texcoord_left;
	//m_textureCoordinates.right = i_texcoord_right;
	//m_textureCoordinates.top = i_texcoord_top;
	//m_textureCoordinates.bottom = i_texcoord_bottom;

	if (!iscSpriteInitialized)
	{
		if (!Initialize())
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("Sprite initialization failed");
		}
		iscSpriteInitialized = true;
	}
}

eae6320::Graphics::cSprite::~cSprite()
{
	if (!iscSpriteCleanedUp && iscSpriteInitialized)
	{
		if (!CleanUp())
		{
			EAE6320_ASSERT(false);
			Logging::OutputError("Sprite cleanup failed");
		}
		iscSpriteCleanedUp = true;
	}
}