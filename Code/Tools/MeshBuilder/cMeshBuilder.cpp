// Header Files
//=============

#include "cMeshBuilder.h"

#include <sstream>
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"

// Inherited Implementation
//=========================

// Build
//------

bool eae6320::AssetBuild::cMeshBuilder::Build( const std::vector<std::string>& )
{
	bool wereThereErrors = false;

	// Copy the source to the target
	{
		std::string errorMessage;
		EAE6320_TODO	// Copy m_path_source to m_path_target
		if ( failed )
		{
			wereThereErrors = true;
			std::ostringstream errorMessage;
			errorMessage << "Failed to copy \"" << m_path_source << "\" to \"" << m_path_target << "\": " << errorMessage;
			eae6320::AssetBuild::OutputErrorMessage( errorMessage.str().c_str(), __FILE__ );
		}
	}
	
	return !wereThereErrors;
}
