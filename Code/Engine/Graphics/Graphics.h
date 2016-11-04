/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============


#include "Configuration.h"
#include "../../Game/Gameplay/GameObject.h"
#include "ConstantBufferData.h"
#include "../Camera/cCamera.h"
#include <vector>

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "../Windows/Includes.h"
#endif


// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{	
		// Render
		//-------

		void SetGameObject(Gameplay::GameObject*gameObject);
		void SetCamera(Camera::cCamera* camera);
		void RenderFrame();

		// Initialization / Clean Up
		//--------------------------

		struct sInitializationParameters
		{
#if defined( EAE6320_PLATFORM_WINDOWS )
			HWND mainWindow;
	#if defined( EAE6320_PLATFORM_D3D )
			unsigned int resolutionWidth, resolutionHeight;
	#elif defined( EAE6320_PLATFORM_GL )
			HINSTANCE thisInstanceOfTheApplication;
	#endif
#endif
		};

		bool Initialize( const sInitializationParameters& i_initializationParameters );
		bool CleanUp();
	}
}

#endif	// EAE6320_GRAPHICS_H
