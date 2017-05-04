/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include "cAudioBuilder.h"
#include <crtdbg.h>

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	return eae6320::AssetBuild::Build<eae6320::AssetBuild::cAudioBuilder>( i_arguments, i_argumentCount );
}
