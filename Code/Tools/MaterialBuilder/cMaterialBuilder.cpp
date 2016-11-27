// Header Files
//=============

#include "cMaterialBuilder.h"

#include <sstream>
#include <iostream>

#include "../../Engine/Asserts/Asserts.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Graphics/ConstantBufferData.h"

// Inherited Implementation
//=========================

// Build
//------

// Helper Function Declarations
//=============================

namespace
{
	eae6320::Graphics::ConstantBufferData::sMaterial material;
	const char * effectPath = NULL;
	const char * texturePath = _strdup("data/textures/default.dds");

	FILE * outputFile = NULL;

	bool LoadInitialTable(lua_State& io_luaState);
	bool LoadConstantBufferDataTable(lua_State& io_luaState);
	bool LoadColorsTable(lua_State& io_luaState);
	bool LoadEffectPath(lua_State& io_luaState);
	bool LoadTexturePath(lua_State& io_luaState);
}

bool eae6320::AssetBuild::cMaterialBuilder::Build(const std::vector<std::string>& i_arguments)
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

	if (!LoadInitialTable(*luaState))
	{
		wereThereErrors = true;
		OutputErrorMessage("Failed to load initial table", __FILE__);
		goto OnExit;
	}
	lua_pop(luaState, 1);

	// Writing data to file 
	{
		// Writing the constant buffer data
		fwrite(&material, sizeof(Graphics::ConstantBufferData::sMaterial), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing constant buffer data to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing the Effect Path
		if (!WriteCStringToFile(effectPath, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write effect path to %s file", m_path_target);
			goto OnExit;
		}
		// Writing the Texture Path
		if (!WriteCStringToFile(texturePath, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write texture path to %s file", m_path_target);
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

	if (effectPath)
	{
		delete effectPath;
	}
	return !wereThereErrors;
}

// Helper Function Definitions
//=============================

namespace
{
	bool LoadInitialTable(lua_State& io_luaState)
	{
		if (!LoadConstantBufferDataTable(io_luaState))
		{
			return false;
		}
		if (!LoadEffectPath(io_luaState))
		{
			return false;
		}
		if (!LoadTexturePath(io_luaState))
		{
			return false;
		}

		return true;
	}
	bool LoadConstantBufferDataTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "constant_buffer_data";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			if (!LoadColorsTable(io_luaState))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:

		// Pop the constant buffer data table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;

	}

	bool LoadColorsTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "g_color";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			goto OnExit;
		}
		else
		{
			if (lua_type(&io_luaState, -1) == LUA_TTABLE)
			{
				const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
				float rgba[4] = { 0.0f,0.0f,0.0f,0.0f };
				if (arrayLength == 4)
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
							goto OnExit;
						}
						if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
						{
							rgba[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
							lua_pop(&io_luaState, 1);
						}
						else
						{
							wereThereErrors = true;
							fprintf_s(stderr, "The value isn't a number!");
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
					}
					if (eae6320::AssetBuild::CheckIfNumberIsNormalized(rgba, 4))
					{
						material.g_color.r = rgba[0];
						material.g_color.g = rgba[1];
						material.g_color.b = rgba[2];
						material.g_color.a = rgba[3];
					}
					else
					{
						wereThereErrors = true;
						fprintf_s(stderr, "The color values were not normalized in range [0,1]");
						goto OnExit;
					}
				}
				else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "There are %zu channels instead of 4", arrayLength);
					goto OnExit;
				}
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
				goto OnExit;
			}
		}
	OnExit:
		// Pop the Colors table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadEffectPath(lua_State & io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "effect_filepath";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TSTRING)
		{
			std::string effectPathString;
			std::string errorMessage;
			const char * const sourceRelativePath = lua_tostring(&io_luaState, -1);
			const char * const assetType = "effects";
			if (!eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(sourceRelativePath, assetType, effectPathString, &errorMessage))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "Cannot convert Convert Source Relative Path %s To Built Relative Path for Asset Type %s....Error: %s", sourceRelativePath, assetType, errorMessage.c_str());
				goto OnExit;
			}
			effectPath = _strdup(effectPathString.c_str());
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a string (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadTexturePath(lua_State & io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "texture_filepath";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{			
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TSTRING)
		{
			std::string texturePathString;
			std::string errorMessage;
			const char * const sourceRelativePath = lua_tostring(&io_luaState, -1);
			const char * const assetType = "textures";
			if (!eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(sourceRelativePath, assetType, texturePathString, &errorMessage))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "Cannot convert Convert Source Relative Path %s To Built Relative Path for Asset Type %s....Error: %s", sourceRelativePath, assetType, errorMessage.c_str());
				goto OnExit;
			}
			texturePath = _strdup(texturePathString.c_str());
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a string (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
}