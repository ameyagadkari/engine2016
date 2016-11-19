// Header Files
//=============

#include "UtilityFunctions.h"

#include <iostream>
#include <sstream>
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"

// Interface
//==========

bool eae6320::AssetBuild::CheckIfColorIsInCorrectFormat(float *rgba)
{
	for (size_t i = 0; i < 4; i++)
	{
		if (rgba[i]<0.0f || rgba[i]>1.0f)
		{
			return false;
		}
	}
	return true;
}

bool eae6320::AssetBuild::WriteCStringToFile(const char * const cString, FILE * outputFile)
{
	bool wereThereErrors = false;
	const uint8_t length = static_cast<uint8_t>(std::char_traits<char>::length(cString) + 1);
	const uint8_t zero = 0;
	// Writing length of the path 
	fwrite(&length, sizeof(uint8_t), 1, outputFile);
	if (ferror(outputFile))
	{
		fprintf_s(stderr, "Error writing length of %s to output file \n", cString);
		wereThereErrors = true;
	}
	// Writing the actual path
	fwrite(cString, sizeof(uint8_t), length - 1, outputFile);
	if (ferror(outputFile))
	{
		fprintf_s(stderr, "Error writing %s to output file \n", cString);
		wereThereErrors = true;
	}
	// Writing a null at the end
	fwrite(&zero, sizeof(uint8_t), 1, outputFile);
	if (ferror(outputFile))
	{
		fprintf_s(stderr, "Error null to output file \n");
		wereThereErrors = true;
	}
	return !wereThereErrors;
}

bool eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath( const char* const i_sourceRelativePath, const char* const i_assetType,
	std::string& o_builtRelativePath, std::string* const o_errorMessage )
{
	// Use the global function defined in AssetBuildSystem.lua
	const char* const functionKey = "ConvertSourceRelativePathToBuiltRelativePath";
	// Initialize a private Lua state for this function
	// and load the AssetBuildSystem.lua file for access to the Asset Type Infos
	static lua_State* luaState = NULL;
	if ( !luaState )
	{
		bool wereThereErrors = false;

		std::string path_assetBuildSystem;

		// Create a new Lua state
		{
			luaState = luaL_newstate();
			if ( luaState )
			{
				luaL_openlibs( luaState );
			}
			else
			{
				wereThereErrors = true;
				if ( o_errorMessage )
				{
					*o_errorMessage = "Lua state wasn't created";
				}
				goto OnInitialized;
			}
		}
		// Load AssetBuildSystem.lua
		{
			if ( GetAssetBuildSystemPath( path_assetBuildSystem, o_errorMessage ) )
			{
				// Load and execute it
				const int luaResult = luaL_dofile( luaState, path_assetBuildSystem.c_str() );
				if ( luaResult != LUA_OK )
				{
					wereThereErrors = true;
					if ( o_errorMessage )
					{
						*o_errorMessage = lua_tostring( luaState, -1 );
					}
					lua_pop( luaState, 1 );
					goto OnInitialized;
				}
			}
			else
			{
				wereThereErrors = true;
				goto OnInitialized;
			}
		}
		// Make sure that it actually exposes the expected function
		{
			lua_getglobal( luaState, functionKey );
			if ( !lua_isfunction( luaState, -1 ) )
			{
				wereThereErrors = true;
				if ( o_errorMessage )
				{
					std::ostringstream errorMessage;
					errorMessage << path_assetBuildSystem << " must define a global function named \""
						<< functionKey << " (not a " << luaL_typename( luaState, -1 ) << ")";
					*o_errorMessage = errorMessage.str();
				}
				goto OnInitialized;
			}
		}

	OnInitialized:

		if ( wereThereErrors )
		{
			// The function can't be in an invalid state on subsequent calls
			lua_close( luaState );
			luaState = NULL;
			return false;
		}
	}

	// The global function should be at the top of the stack.
	// Make a copy so that it can be called but the original will still be at the top of the stack
	// the next time that this function is called.
	lua_pushvalue( luaState, -1 );
	const int returnValueCount = 2;
	{
		const int argumentCount = 2;
		{
			lua_pushstring( luaState, i_sourceRelativePath );
			lua_pushstring( luaState, i_assetType );
		}
		const int noErrorHandler = 0;
		const int luaResult = lua_pcall( luaState, argumentCount, returnValueCount, noErrorHandler );
		if ( luaResult != LUA_OK )
		{
			if ( o_errorMessage )
			{
				*o_errorMessage = lua_tostring( luaState, -1 );
			}
			lua_pop( luaState, 1 );
			return false;
		}
	}
	bool wereThereErrors = false;
	if ( lua_isboolean( luaState, -2 ) )
	{
		wereThereErrors = !lua_toboolean( luaState, -2 );
		if ( !wereThereErrors )
		{
			if ( lua_isstring( luaState, -1 ) )
			{
				o_builtRelativePath = lua_tostring( luaState, -1 );
			}
			else
			{
				wereThereErrors = true;
				std::ostringstream errorMessage;
				errorMessage << functionKey << "() returned success but then a " << luaL_typename( luaState, -1 )
					<< " (instead of a string)";
				*o_errorMessage = errorMessage.str();
			}
		}
		else
		{
			if ( o_errorMessage )
			{
				if ( lua_isstring( luaState, -1 ) )
				{
					*o_errorMessage = lua_tostring( luaState, -1 );
				}
				else
				{
					std::ostringstream errorMessage;
					errorMessage << functionKey << "() failed, but return value #2 is a " << luaL_typename( luaState, -1 )
						<< ", not a string";
					*o_errorMessage = errorMessage.str();
				}
			}
		}
	}
	else
	{
		wereThereErrors = true;
		if ( o_errorMessage )
		{
			std::ostringstream errorMessage;
			errorMessage << functionKey << "() should return a boolean as return value #1, not a " << luaL_typename( luaState, -2 );
			*o_errorMessage = errorMessage.str();
		}
	}
	// Pop the returned values
	lua_pop( luaState, returnValueCount );
	return !wereThereErrors;
}

bool eae6320::AssetBuild::GetAssetBuildSystemPath( std::string& o_path, std::string* const o_errorMessage )
{
	std::string scriptDir;
	if ( Platform::GetEnvironmentVariable( "ScriptDir", scriptDir, o_errorMessage ) )
	{
		o_path = scriptDir + "AssetBuildSystem.lua";
		return true;
	}
	else
	{
		return false;
	}
}

void eae6320::AssetBuild::OutputErrorMessage( const char* const i_errorMessage, const char* const i_optionalFileName )
{
	// This formatting causes the errors to show up in Visual Studio's "Error List" tab
	std::cerr << ( i_optionalFileName ? i_optionalFileName : "Asset Build" ) << ": error: " <<
		i_errorMessage << "\n";
}
