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

bool eae6320::AssetBuild::cMeshBuilder::Build( const std::vector<std::string>& i_arguments)
{
	bool wereThereErrors = false;

	// Copy the source to the target
	{
		std::string errorMessage;
		//EAE6320_TODO	// Copy m_path_source to m_path_target
		// There are many reasons that a source should be rebuilt,
		// and so even if the target already exists it should just be written over
		const bool noErrorIfTargetAlreadyExists = false;
		// Since we rely on timestamps to determine when a target was built
		// its file time should be updated when the source gets copied
		const bool updateTheTargetFileTime = true;
		if (!(eae6320::Platform::CopyFile(m_path_source, m_path_target, noErrorIfTargetAlreadyExists, updateTheTargetFileTime, &errorMessage)))
		{
			wereThereErrors = true;
			std::ostringstream errorMessage;
			errorMessage << "Failed to copy \"" << m_path_source << "\" to \"" << m_path_target << "\": " << errorMessage.str();
			eae6320::AssetBuild::OutputErrorMessage(errorMessage.str().c_str(), __FILE__);
		}
	}
	
	return !wereThereErrors;
}
