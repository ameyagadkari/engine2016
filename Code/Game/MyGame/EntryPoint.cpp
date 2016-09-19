/*
	WinMain() is the entry point of a Windows program
	(instead of the familiar main() in a console program)
*/

// Header Files
//=============

#include "cMyGame.h"

#ifdef _DEBUG
#include  "crtdbg.h"
#endif // _DEBUG

// Entry Point
//============

int WINAPI WinMain( HINSTANCE i_thisInstanceOfTheApplication, HINSTANCE, char* i_commandLineArguments, int i_initialWindowDisplayState )
{
#ifdef _DEBUG

	// get current dbg flag (report it)
	int flag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

	// logically OR leak check bit
	flag |= _CRTDBG_LEAK_CHECK_DF;

	// set the flags again
	_CrtSetDbgFlag(flag);

#endif // _DEBUG
	return eae6320::Application::Run<eae6320::cMyGame>( i_thisInstanceOfTheApplication, i_commandLineArguments, i_initialWindowDisplayState );
}
