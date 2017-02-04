/*
	This file contains the function declarations for graphics
*/

#ifndef EAE6320_GRAPHICS_H
#define EAE6320_GRAPHICS_H

// Header Files
//=============


#include "Configuration.h"
#include "../../Game/Debug/DebugObject.h"

#if defined( EAE6320_PLATFORM_WINDOWS )
	#include "../Windows/Includes.h"
#endif

// Forward Declarations
//=====================
namespace eae6320
{
	namespace Camera
	{
		class cCamera;
	}
	namespace Gameplay
	{
		class GameObject;
		class GameObject2D;
	}
	namespace Debug
	{
		namespace Shapes
		{
			class DebugObject;
		}
	}
}

// Interface
//==========

namespace eae6320
{
	namespace Graphics
	{	
		// Render
		//-------
		void SetGameObject(Gameplay::GameObject*gameObject);
		void SetGameObject2D(Gameplay::GameObject2D*gameObject2d);
		void SetDebugObject(Debug::Shapes::DebugObject*debugObject);
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
