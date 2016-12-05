// Header Files
//=============

#include "cGameobject2DBuilder.h"

#include <sstream>
#include <iostream>
#include "../../Engine/Asserts/Asserts.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Graphics/cSprite.h"
#include "../../Engine/Math/cHalf.h"

// Inherited Implementation
//=========================

// Build
//------
namespace
{
	eae6320::Graphics::Sprite::ScreenPosition screenPosition;
	eae6320::Graphics::Sprite::TextureCoordinates textureCoordinates;
	const char * materialPath = NULL;

	FILE * outputFile = NULL;

	bool LoadGameobject2DTable(lua_State& io_luaState);
	bool LoadLeftValue(lua_State& io_luaState, bool doesItSupportDefault);
	bool LoadRightValue(lua_State& io_luaState, bool doesItSupportDefault);
	bool LoadTopValue(lua_State& io_luaState, bool doesItSupportDefault);
	bool LoadBottomValue(lua_State& io_luaState, bool doesItSupportDefault);

}

bool eae6320::AssetBuild::cGameobject2DBuilder::Build(const std::vector<std::string>& i_arguments)
{
	bool wereThereErrors = false;
	std::string errorMessage;

	// Create a new Lua state
	lua_State* luaState = NULL;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			wereThereErrors = true;
			OutputErrorMessage("Failed to create a new Lua state", __FILE__);
			goto OnExit;
		}
	}

	// Creating/Opening file to write in binary mode
	outputFile = fopen(m_path_target, "wb");
	if (!outputFile)
	{
		perror("The target file cannot be opened for writing out data");
		wereThereErrors = true;
		goto OnExit;
	}

	// Open the standard libraries
	luaL_openlibs(luaState);

	// Load and execute it
	{
		if (Platform::DoesFileExist(m_path_source, &errorMessage))
		{

			// Load the asset file as a "chunk",
			// meaning there will be a callable function at the top of the stack
			const int stackTopBeforeLoad = lua_gettop(luaState);
			{
				const int luaResult = luaL_loadfile(luaState, m_path_source);
				if (luaResult != LUA_OK)
				{
					wereThereErrors = true;
					std::cerr << lua_tostring(luaState, -1) << std::endl;
					// Pop the error message
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
			// Execute the "chunk", which should load the asset
			// into a table at the top of the stack
			{
				const int argumentCount = 0;
				const int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
				const int noErrorHandler = 0;
				const int luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noErrorHandler);
				if (luaResult == LUA_OK)
				{
					// A well-behaved asset file will only return a single value
					const int returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
					if (returnedValueCount == 1)
					{
						// A correct asset file _must_ return a table
						if (!lua_istable(luaState, -1))
						{
							wereThereErrors = true;
							std::cerr << "Asset files must return a table (instead of a "
								<< luaL_typename(luaState, -1) << ")" << std::endl;
							// Pop the returned non-table value
							lua_pop(luaState, 1);
							goto OnExit;
						}
					}
					else
					{
						wereThereErrors = true;
						std::cerr << "Asset files must return a single table (instead of "
							<< returnedValueCount << " values)" << std::endl;
						// Pop every value that was returned
						lua_pop(luaState, returnedValueCount);
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					std::cerr << lua_tostring(luaState, -1) << std::endl;
					// Pop the error message
					lua_pop(luaState, 1);
					goto OnExit;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			OutputErrorMessage(errorMessage.c_str(), m_path_source);
			goto OnExit;
		}
	}

	//If this code is reached the asset file was loaded successfully,
	//and its table is now at index -1

	if (!LoadGameobject2DTable(*luaState))
	{
		wereThereErrors = true;
		OutputErrorMessage("Failed to load gameobject table", __FILE__);
		goto OnExit;
	}
	lua_pop(luaState, 1);

	// Writing data to file 
	{
		// Writing Screen Position struct
		fwrite(&screenPosition, sizeof(Graphics::Sprite::ScreenPosition), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing screen position struct to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Texture Coordinates struct
		fwrite(&textureCoordinates, sizeof(Graphics::Sprite::TextureCoordinates), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing texture coordinates struct to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing the Material File Path
		if (!WriteCStringToFile(materialPath, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write material file path to %s file", m_path_target);
			goto OnExit;
		}
	}

OnExit:
	// Closing file only if it got opened
	if (outputFile)
	{
		if (fclose(outputFile))
		{
			wereThereErrors = true;
			OutputErrorMessage("Failed to close target file", __FILE__);
		}
	}

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack
		// regardless of any errors
		EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = NULL;
	}

	if (materialPath)
	{
		delete materialPath;
	}

	return !wereThereErrors;
}

// Helper Function Definitions
//=============================

namespace
{
	bool LoadGameobject2DTable(lua_State & io_luaState)
	{
		bool wereThereErrors = false;

		//Loading material path
		{
			const char* key = "material_filepath";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_isnil(&io_luaState, -1))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TSTRING)
			{
				std::string materialFilePathString;
				std::string errorMessage;
				const char * const sourceRelativePath = lua_tostring(&io_luaState, -1);
				const char * const assetType = "materials";
				if (!eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(sourceRelativePath, assetType, materialFilePathString, &errorMessage))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Cannot convert Convert Source Relative Path %s To Built Relative Path for Asset Type %s....Error: %s", sourceRelativePath, assetType, errorMessage.c_str());
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				materialPath = _strdup(materialFilePathString.c_str());
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a string (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}

		// Loading screen positon table
		{
			const char* key = "screen_position";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_isnil(&io_luaState, -1))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TTABLE)
			{
				const bool defaultNotSupported = false;
				if (!LoadLeftValue(io_luaState, defaultNotSupported))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load left value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadRightValue(io_luaState, defaultNotSupported))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load right value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadTopValue(io_luaState, defaultNotSupported))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load top value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadBottomValue(io_luaState, defaultNotSupported))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load bottom value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}

		// Loading texture coordinates table
		{
			const char* key = "texture_coordinates";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_isnil(&io_luaState, -1))
			{
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TTABLE)
			{
				const bool defaultSupported = true;
				if (!LoadLeftValue(io_luaState, defaultSupported))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load left value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadRightValue(io_luaState, defaultSupported))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load right value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadTopValue(io_luaState, defaultSupported))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load top value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadBottomValue(io_luaState, defaultSupported))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load bottom value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		return !wereThereErrors;
	}

	bool LoadLeftValue(lua_State& io_luaState, bool doesItSupportDefault)
	{
		bool wereThereErrors = false;
		const char* key = "left";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (doesItSupportDefault)
		{
			if (lua_isnil(&io_luaState, -1))
			{
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
			{
				textureCoordinates.left = eae6320::Math::cHalf::MakeHalfFromFloat(static_cast<float>(lua_tonumber(&io_luaState, -1)));
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		else
		{
			if (lua_isnil(&io_luaState, -1))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
			{
				screenPosition.left = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		return !wereThereErrors;
	}
	bool LoadRightValue(lua_State& io_luaState, bool doesItSupportDefault)
	{
		bool wereThereErrors = false;
		const char* key = "right";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (doesItSupportDefault)
		{
			if (lua_isnil(&io_luaState, -1))
			{
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
			{
				textureCoordinates.right = eae6320::Math::cHalf::MakeHalfFromFloat(static_cast<float>(lua_tonumber(&io_luaState, -1)));
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		else
		{
			if (lua_isnil(&io_luaState, -1))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
			{
				screenPosition.right = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		return !wereThereErrors;
	}
	bool LoadTopValue(lua_State& io_luaState, bool doesItSupportDefault)
	{
		bool wereThereErrors = false;
		const char* key = "top";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (doesItSupportDefault)
		{
			if (lua_isnil(&io_luaState, -1))
			{
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
			{
				textureCoordinates.top =
#if defined( EAE6320_PLATFORM_D3D )
					eae6320::Math::cHalf::MakeHalfFromFloat(1.0f - static_cast<float>(lua_tonumber(&io_luaState, -1)));
#elif defined( EAE6320_PLATFORM_GL )
					eae6320::Math::cHalf::MakeHalfFromFloat(static_cast<float>(lua_tonumber(&io_luaState, -1)));
#endif
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		else
		{
			if (lua_isnil(&io_luaState, -1))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
			{
				screenPosition.top = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		return !wereThereErrors;
	}
	bool LoadBottomValue(lua_State& io_luaState, bool doesItSupportDefault)
	{
		bool wereThereErrors = false;
		const char* key = "bottom";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (doesItSupportDefault)
		{
			if (lua_isnil(&io_luaState, -1))
			{
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
			{
				textureCoordinates.bottom =
#if defined( EAE6320_PLATFORM_D3D )
					eae6320::Math::cHalf::MakeHalfFromFloat(1.0f - static_cast<float>(lua_tonumber(&io_luaState, -1)));
#elif defined( EAE6320_PLATFORM_GL )
					eae6320::Math::cHalf::MakeHalfFromFloat(static_cast<float>(lua_tonumber(&io_luaState, -1)));
#endif
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		else
		{
			if (lua_isnil(&io_luaState, -1))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
			{
				screenPosition.bottom = static_cast<float>(lua_tonumber(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		return !wereThereErrors;
	}
}