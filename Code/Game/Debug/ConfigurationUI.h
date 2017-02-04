/*
This file provides configurable settings
that can be used to control debug ui behavior
*/

#ifndef EAE6320_DEBUG_UI_CONFIGURATION_H
#define EAE6320_DEBUG_UI_CONFIGURATION_H

// By default debug shapes are only enabled for debug builds,
// but you can #define it differently as necessary
#ifdef _DEBUG
#define EAE6320_DEBUG_UI_AREENABLED
#endif

#endif	// EAE6320_DEBUG_UI_CONFIGURATION_H
