/*
	This class builds materials
*/

#ifndef EAE6320_CMATERIALBUILDER_H
#define EAE6320_CMATERIALBUILDER_H

// Header Files
//=============

#include "../AssetBuildLibrary/cbBuilder.h"

// Class Declaration
//==================

namespace eae6320
{
	namespace AssetBuild
	{
		class cMaterialBuilder : public cbBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			bool Build( const std::vector<std::string>& i_arguments ) override;
		};
	}
}

#endif	// EAE6320_CMATERIALBUILDER_H
