// Header Files
//=============

#include "../cShaderBuilder.h"

#include <sstream>
#include "../../AssetBuildLibrary/UtilityFunctions.h"
#include "../../../Engine/Platform/Platform.h"

// Interface
//==========

// Build
//------

bool eae6320::AssetBuild::cShaderBuilder::Build( const Graphics::ShaderTypes::eShaderType i_shaderType, const std::vector<std::string>& i_arguments )
{
	// Get the path to the shader compiler
	std::string path_fxc;
	{
		// Get the path to the Windows SDK
		std::string path_sdk;
		{
			std::string errorMessage;
			if ( !Platform::GetEnvironmentVariable( "WindowsSDKDir", path_sdk, &errorMessage ) )
			{
				std::ostringstream decoratedErrorMessage;
				decoratedErrorMessage << "Failed to get the path to the Windows SDK: " << errorMessage;
				OutputErrorMessage( decoratedErrorMessage.str().c_str(), __FILE__ );
				return false;
			}
		}

		// Get the Windows SDK version in use
		std::string target_platform_version;
		{
			std::string errorMessage;
			if (!Platform::GetEnvironmentVariable("TargetPlatformVersion", target_platform_version, &errorMessage))
			{
				std::ostringstream decoratedErrorMessage;
				decoratedErrorMessage << "Failed to get the Windows SDK version: " << errorMessage;
				OutputErrorMessage(decoratedErrorMessage.str().c_str(), __FILE__);
				return false;
			}
		}

		path_fxc = path_sdk + "bin/" + target_platform_version + "/" +
#ifndef _WIN64
			"x86"
#else
			"x64"
#endif
			+ "/fxc.exe";
	}
	// Create the command to run
	std::string command;
	{
		std::ostringstream commandToBuild;
		commandToBuild << "\"" << path_fxc << "\"";
		// Target profile
		switch ( i_shaderType )
		{
		case Graphics::ShaderTypes::eShaderType::Vertex:
			commandToBuild << " /Tvs_4_0";
			break;
		case Graphics::ShaderTypes::eShaderType::Fragment:
			commandToBuild << " /Tps_4_0";
			break;
		}
		// Entry point
		commandToBuild << " /Emain"
			// #define the platform
			<< " /DEAE6320_PLATFORM_D3D"
#ifdef EAE6320_GRAPHICS_AREDEBUGSHADERSENABLED
			// Disable optimizations so that debugging is easier
			<< " /Od"
			// Enable debugging
			<< " /Zi"
#endif
			// Target file
			<< " /Fo\"" << m_path_target << "\""
			// Don't output the logo
			<< " /nologo"
			// Source file
			<< " \"" << m_path_source << "\""
		;
		command = commandToBuild.str();
	}
	// Execute the command
	{
		int exitCode;
		std::string errorMessage;
		if ( Platform::ExecuteCommand( command.c_str(), &exitCode, &errorMessage ) )
		{
			return exitCode == EXIT_SUCCESS;
		}
		else
		{
			OutputErrorMessage( errorMessage.c_str(), m_path_source );
			return false;
		}
	}
}
