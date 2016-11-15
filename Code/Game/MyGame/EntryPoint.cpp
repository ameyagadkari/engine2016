/*
	WinMain() is the entry point of a Windows program
	(instead of the familiar main() in a console program)
*/

// Header Files
//=============

#include "cMyGame.h"

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#endif // _DEBUG
#include <crtdbg.h>
// Entry Point
//============

int WINAPI WinMain( HINSTANCE i_thisInstanceOfTheApplication, HINSTANCE, char* i_commandLineArguments, int i_initialWindowDisplayState )
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	return eae6320::Application::Run<eae6320::cMyGame>( i_thisInstanceOfTheApplication, i_commandLineArguments, i_initialWindowDisplayState );
}
