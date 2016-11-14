// Header Files
//=============

#include "cGameobjectBuilder.h"

#include <sstream>
#include <iostream>
#include "../../Engine/Asserts/Asserts.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Math/cVector.h"

// Inherited Implementation
//=========================

// Build
//------
namespace
{
	const char * effectPath = NULL;
	const char * meshPath = NULL;
	const char * controllerName = NULL;
	const char * rotationAxis = NULL;
	eae6320::Math::cVector position = eae6320::Math::cVector::zero;
	eae6320::Math::cVector orientation = eae6320::Math::cVector::zero;
	uint8_t isStatic = 0;
	uint8_t isRotating = 0;

	FILE * outputFile = NULL;

	bool LoadGameobjectTable(lua_State& io_luaState);

}

bool eae6320::AssetBuild::cGameobjectBuilder::Build(const std::vector<std::string>& i_arguments)
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

	if (!LoadGameobjectTable(*luaState))
	{
		wereThereErrors = true;
		OutputErrorMessage("Failed to load gameobject table", __FILE__);
		goto OnExit;
	}
	lua_pop(luaState, 1);

	// Writing data to file 
	{
		// Writing the Effect File Path
		if (!WriteCStringToFile(effectPath, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write effect file path to %s file", m_path_target);
			goto OnExit;
		}
		// Writing the Mesh File Path
		if (!WriteCStringToFile(meshPath, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write mesh file path to %s file", m_path_target);
			goto OnExit;
		}
		// Writing the Controller Name
		if (!WriteCStringToFile(controllerName, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write controller name to %s file", m_path_target);
			goto OnExit;
		}
		// Writing the RotationAxis
		if (!WriteCStringToFile(rotationAxis, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write rotation axis to %s file", m_path_target);
			goto OnExit;
		}
		// Writing Position Vector
		fwrite(&position, sizeof(Math::cVector), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing position vector to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Orientation Vector
		fwrite(&orientation, sizeof(Math::cVector), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing orientation vector to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing IsStatic bool as uint8_t
		fwrite(&isStatic, sizeof(uint8_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing isStatic bool to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing IsRotating bool as uint8_t
		fwrite(&isRotating, sizeof(uint8_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing isRotating bool to %s \n", m_path_target);
			wereThereErrors = true;
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

	return !wereThereErrors;
}

// Helper Function Definitions
//=============================

namespace
{
	bool LoadGameobjectTable(lua_State & io_luaState)
	{
		bool wereThereErrors = false;

		//Loading effect path
		{
			const char* key = "effect_filepath";
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
				std::string effectFilePathString;
				std::string errorMessage;
				const char * const sourceRelativePath = lua_tostring(&io_luaState, -1);
				const char * const assetType = "effects";
				if (!eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(sourceRelativePath, assetType, effectFilePathString, &errorMessage))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Cannot convert Convert Source Relative Path %s To Built Relative Path for Asset Type %s....Error: %s", sourceRelativePath, assetType, errorMessage.c_str());
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				effectPath = _strdup(effectFilePathString.c_str());
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

		//Loading mesh path
		{
			const char* key = "mesh_filepath";
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
				std::string meshFilePathString;
				std::string errorMessage;
				const char * const sourceRelativePath = lua_tostring(&io_luaState, -1);
				const char * const assetType = "meshes";
				if (!eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(sourceRelativePath, assetType, meshFilePathString, &errorMessage))
				{
					wereThereErrors = true;
					fprintf_s(stderr, "Cannot convert Convert Source Relative Path %s To Built Relative Path for Asset Type %s....Error: %s", sourceRelativePath, assetType, errorMessage.c_str());
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				meshPath = _strdup(meshFilePathString.c_str());
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

		//Loading controller name
		{
			const char* key = "controller_name";
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
				controllerName = _strdup(lua_tostring(&io_luaState, -1));
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

		//Loading rotation axis
		{
			const char* key = "rotation_axis";
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
				rotationAxis = _strdup(lua_tostring(&io_luaState, -1));
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

		//Loading position
		{
			const char* key = "position";
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
				const int arrayLength = luaL_len(&io_luaState, -1);
				float xyz[3] = { 0.0f,0.0f,0.0f };
				if (arrayLength == 3)
				{
					// Remember that Lua arrays are 1-based and not 0-based!
					for (int i = 1; i <= arrayLength; ++i)
					{
						lua_pushinteger(&io_luaState, i);
						lua_gettable(&io_luaState, -2);
						if (lua_isnil(&io_luaState, -1))
						{
							wereThereErrors = true;
							fprintf_s(stderr, "No value for key:%d was found in the table", i);
							lua_pop(&io_luaState, 1);
							return !wereThereErrors;
						}
						if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
						{
							xyz[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
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
					position.x = xyz[0];
					position.y = xyz[1];
					position.z = xyz[2];
				}
				else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "There are %d coordinates instead of 3", arrayLength);
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

		//Loading eular angles
		{
			const char* key = "eular_angles";
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
				const int arrayLength = luaL_len(&io_luaState, -1);
				float xyz[3] = { 0.0f,0.0f,0.0f };
				if (arrayLength == 3)
				{
					// Remember that Lua arrays are 1-based and not 0-based!
					for (int i = 1; i <= arrayLength; ++i)
					{
						lua_pushinteger(&io_luaState, i);
						lua_gettable(&io_luaState, -2);
						if (lua_isnil(&io_luaState, -1))
						{
							wereThereErrors = true;
							fprintf_s(stderr, "No value for key:%d was found in the table", i);
							lua_pop(&io_luaState, 1);
							return !wereThereErrors;
						}
						if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
						{
							xyz[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
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
					orientation.x = xyz[0];
					orientation.y = xyz[1];
					orientation.z = xyz[2];
				}
				else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "There are %d eular angle values instead of 3", arrayLength);
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

		//Loading isstatic value
		{
			const char* key = "is_static";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_isnil(&io_luaState, -1))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TBOOLEAN)
			{
				isStatic = lua_toboolean(&io_luaState, -1);
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a boolean (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}

		//Loading isrotating value
		{
			const char* key = "is_rotating";
			lua_pushstring(&io_luaState, key);
			lua_gettable(&io_luaState, -2);
			if (lua_isnil(&io_luaState, -1))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
			if (lua_type(&io_luaState, -1) == LUA_TBOOLEAN)
			{
				isRotating = lua_toboolean(&io_luaState, -1);
				lua_pop(&io_luaState, 1);
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a boolean (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
				lua_pop(&io_luaState, 1);
				return !wereThereErrors;
			}
		}
		return !wereThereErrors;
	}
}