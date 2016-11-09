/*
	This class builds gameobjects
*/

#ifndef EAE6320_CGAMEOBJECTBUILDER_H
#define EAE6320_CGAMEOBJECTBUILDER_H

// Header Files
//=============

#include "../AssetBuildLibrary/cbBuilder.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace AssetBuild
	{
		class cGameobjectBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			virtual bool Build( const std::vector<std::string>& i_arguments );
		};
	}
}

#endif	// EAE6320_CGAMEOBJECTBUILDER_H
