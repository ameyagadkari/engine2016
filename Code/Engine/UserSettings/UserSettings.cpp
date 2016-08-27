// Header Files
//=============

#include "UserSettings.h"

// Static Data Initialization
//===========================

namespace
{
	unsigned int s_resolutionHeight = 512;
	unsigned int s_resolutionWidth = 512;
}

// Interface
//==========

unsigned int eae6320::UserSettings::GetResolutionHeight()
{
	return s_resolutionHeight;
}

unsigned int eae6320::UserSettings::GetResolutionWidth()
{
	return s_resolutionWidth;
}
