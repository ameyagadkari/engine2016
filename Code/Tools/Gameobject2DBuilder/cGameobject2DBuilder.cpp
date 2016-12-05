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
#include "../../Game/Gameplay/GameObject2D.h"
#include "../../Engine/Math/cHalf.h"

// Inherited Implementation
//=========================

// Build
//------
namespace
{
	eae6320::Graphics::Sprite::TextureCoordinates textureCoordinates;
	eae6320::Gameplay::RectTransform rectTranform;
	uint8_t anchorEnumNumber = 0;
	const char * materialPath = NULL;

	FILE * outputFile = NULL;

	bool LoadGameobject2DTable(lua_State& io_luaState);
	bool LoadLeftValue(lua_State& io_luaState);
	bool LoadRightValue(lua_State& io_luaState);
	bool LoadTopValue(lua_State& io_luaState);
	bool LoadBottomValue(lua_State& io_luaState);
	bool LoadPixelCoordinates(lua_State& io_luaState);
	bool LoadWidth(lua_State& io_luaState);
	bool LoadHeight(lua_State& io_luaState);
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
		// Writing Texture Coordinates struct
		fwrite(&textureCoordinates, sizeof(Graphics::Sprite::TextureCoordinates), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing texture coordinates struct to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Pixel Coordinates struct
		fwrite(&rectTranform, sizeof(Gameplay::RectTransform), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing pixel coordinates struct to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing anchor enum number
		fwrite(&anchorEnumNumber, sizeof(uint8_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing anchor enum number to %s \n", m_path_target);
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

		// Loading rect transform table
		{
			const char* key = "rect_transform";
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
				if (!LoadPixelCoordinates(io_luaState))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load pixel coordinates");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadWidth(io_luaState))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load width");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadHeight(io_luaState))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load height");
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

		//Loading anchor
		{
			const char* key = "anchor";
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
				const char * const anchor = lua_tostring(&io_luaState, -1);
				if (strcmp(anchor, "top_left") == 0)
				{
					anchorEnumNumber = static_cast<uint8_t>(eae6320::Gameplay::Anchor::TOP_LEFT);
				}
				else if (strcmp(anchor, "bottom_left") == 0)
				{
					anchorEnumNumber = static_cast<uint8_t>(eae6320::Gameplay::Anchor::BOTTOM_LEFT);
				}
				else if (strcmp(anchor, "top_right") == 0)
				{
					anchorEnumNumber = static_cast<uint8_t>(eae6320::Gameplay::Anchor::TOP_RIGHT);
				}
				else if (strcmp(anchor, "bottom_right") == 0)
				{
					anchorEnumNumber = static_cast<uint8_t>(eae6320::Gameplay::Anchor::BOTTOM_RIGHT);
				}
				else if (strcmp(anchor, "center") == 0)
				{
					anchorEnumNumber = static_cast<uint8_t>(eae6320::Gameplay::Anchor::CENTER);
				}
				else if (strcmp(anchor, "left_center") == 0)
				{
					anchorEnumNumber = static_cast<uint8_t>(eae6320::Gameplay::Anchor::LEFT_CENTER);
				}
				else if (strcmp(anchor, "bottom_center") == 0)
				{
					anchorEnumNumber = static_cast<uint8_t>(eae6320::Gameplay::Anchor::BOTTOM_CENTER);
				}
				else if (strcmp(anchor, "top_center") == 0)
				{
					anchorEnumNumber = static_cast<uint8_t>(eae6320::Gameplay::Anchor::TOP_CENTER);
				}
				else if (strcmp(anchor, "right_center") == 0)
				{
					anchorEnumNumber = static_cast<uint8_t>(eae6320::Gameplay::Anchor::RIGHT_CENTER);
				}
				else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Invalid anchor type: %s", anchor);
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
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
				if (!LoadLeftValue(io_luaState))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load left value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadRightValue(io_luaState))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load right value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadTopValue(io_luaState))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Failed to load top value");
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (!LoadBottomValue(io_luaState))
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

	bool LoadLeftValue(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "left";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
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
		return !wereThereErrors;
	}
	bool LoadRightValue(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "right";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
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
		return !wereThereErrors;
	}
	bool LoadTopValue(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "top";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
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
		return !wereThereErrors;
	}
	bool LoadBottomValue(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "bottom";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
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
		return !wereThereErrors;
	}

	bool LoadPixelCoordinates(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "pixel_coordinates";
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
			const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
			int16_t xy[2] = { 0,0 };
			if (arrayLength == 2)
			{
				// Remember that Lua arrays are 1-based and not 0-based!
				for (size_t i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						fprintf_s(stderr, "No value for key:%zu was found in the table", i);
						lua_pop(&io_luaState, 1);
						return !wereThereErrors;
					}
					if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
					{
						xy[i - 1] = static_cast<int16_t>(lua_tonumber(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
					}
					else
					{
						wereThereErrors = true;
						fprintf_s(stderr, "The value isn't a number!");
						lua_pop(&io_luaState, 1);
						return !wereThereErrors;
					}
				}
				rectTranform.pixelCoordinates.x = xy[0];
				rectTranform.pixelCoordinates.y = xy[1];
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "There are %zu pixel coordinates instead of 2", arrayLength);
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
		return !wereThereErrors;
	}
	bool LoadWidth(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "width";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			lua_pop(&io_luaState, 1);
			return !wereThereErrors;
		}
		if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
		{
			rectTranform.width = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
			lua_pop(&io_luaState, 1);
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			lua_pop(&io_luaState, 1);
			return !wereThereErrors;
		}
		return !wereThereErrors;
	}
	bool LoadHeight(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "height";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			lua_pop(&io_luaState, 1);
			return !wereThereErrors;
		}
		if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
		{
			rectTranform.height = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
			lua_pop(&io_luaState, 1);
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a number (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			lua_pop(&io_luaState, 1);
			return !wereThereErrors;
		}
		return !wereThereErrors;
	}
}