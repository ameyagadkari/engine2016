/*
	The main() function is where the program starts execution
*/

// Header Files
//=============

#include "cMeshBuilder.h"
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>

// Entry Point
//============

int main( int i_argumentCount, char** i_arguments )
{
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	return eae6320::AssetBuild::Build<eae6320::AssetBuild::cMeshBuilder>( i_arguments, i_argumentCount );
}
