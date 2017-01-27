// Header Files
//=============

#include "cDebugShapeBuilder.h"

#include <sstream>
#include <iostream>
#include <regex>
#include "../../Engine/Asserts/Asserts.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Math/cVector.h"
#include "../../Engine/Graphics/MeshData.h"
#include "../../Engine/Math/PrimitiveShapeGenerator.h"

// Inherited Implementation
//=========================

// Build
//------
namespace
{
	bool LoadPrimitiveobjectTable(lua_State& io_luaState);
	bool LoadBoxTable(lua_State& io_luaState);
	bool LoadMaterialTable(lua_State& io_luaState);
	bool LoadBoxDimensions(lua_State& io_luaState);
	bool LoadColorsTable(lua_State& io_luaState);
	bool LoadPositionTable(lua_State& io_luaState);
	bool LoadEularTable(lua_State& io_luaState);

	std::cmatch match;
	const std::regex pattern_match("(.*\\\\)(.*)");

	bool CreateBinaryFile(const char*const targetPath);
	void InitilizeMeshData(const uint32_t i_numberOfVertices, const uint32_t i_numberOfIndices);

	FILE * outputFile = nullptr;
	const char* materialPath = nullptr;
	eae6320::Math::cVector position = eae6320::Math::cVector::zero;
	eae6320::Math::cVector orientation = eae6320::Math::cVector::zero;
	eae6320::Graphics::MeshData *meshData = nullptr;
	float rgba[4] = { 0.0f,0.0f,0.0f,0.0f };
	std::string targetPath;
}

bool eae6320::AssetBuild::cDebugShapeBuilder::Build(const std::vector<std::string>& i_arguments)
{
	bool wereThereErrors = false;
	regex_match(m_path_target, match, pattern_match);

	std::string errorMessage;

	// Create a new Lua state
	lua_State* luaState = nullptr;
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			wereThereErrors = true;
			OutputErrorMessage("Failed to create a new Lua state", __FILE__);
			goto OnExit;
		}
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

	if (!LoadPrimitiveobjectTable(*luaState))
	{
		wereThereErrors = true;
		OutputErrorMessage("Failed to load Primitiveobject table", __FILE__);
		goto OnExit;
	}
	lua_pop(luaState, 1);


OnExit:

	if (luaState)
	{
		// If I haven't made any mistakes
		// there shouldn't be anything on the stack
		// regardless of any errors
		EAE6320_ASSERT(lua_gettop(luaState) == 0);

		lua_close(luaState);
		luaState = nullptr;
	}
	if (materialPath)
	{
		delete materialPath;
	}
	if (outputFile)
	{
		if (fclose(outputFile))
		{
			wereThereErrors = true;
			eae6320::AssetBuild::OutputErrorMessage("Failed to close target file", __FILE__);
		}
		else
		{
			outputFile = nullptr;
		}
	}
	if (meshData)
	{
		if (meshData->indexData)
		{
			free(meshData->indexData);
			meshData->indexData = nullptr;
		}
		if (meshData->vertexData)
		{
			free(meshData->vertexData);
			meshData->vertexData = nullptr;
		}
		free(meshData);
		meshData = nullptr;
	}
	return !wereThereErrors;
}

// Helper Function Definitions
//=============================

namespace
{
	bool LoadPrimitiveobjectTable(lua_State & io_luaState)
	{
		if (!LoadMaterialTable(io_luaState))
		{
			return false;
		}
		if (!LoadBoxTable(io_luaState))
		{
			return false;
		}
		return true;
	}
	bool LoadBoxTable(lua_State & io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "boxes";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
		}
		else
		{
			if (lua_type(&io_luaState, -1) == LUA_TTABLE)
			{
				const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
				// Remember that Lua arrays are 1-based and not 0-based!
				for (size_t i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						fprintf_s(stderr, "No value for key:%zu was found in the table", i);
						goto OnExit;
					}
					if (lua_type(&io_luaState, -1) == LUA_TTABLE)
					{
						if (!LoadColorsTable(io_luaState))
						{
							wereThereErrors = true;
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						if (!LoadBoxDimensions(io_luaState))
						{
							wereThereErrors = true;
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						if (!LoadPositionTable(io_luaState))
						{
							wereThereErrors = true;
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						if (!LoadEularTable(io_luaState))
						{
							wereThereErrors = true;
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						
						targetPath = match.str(1).append("box" + std::to_string(i)).append(".pbinobj");
						if (!CreateBinaryFile(targetPath.c_str()))
						{
							fprintf_s(stderr, "Failed to create binary file");
							lua_pop(&io_luaState, 1);
							goto OnExit;
						}
						lua_pop(&io_luaState, 1);
					}
					else
					{
						wereThereErrors = true;
						fprintf_s(stderr, "The value isn't a table!");
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			}
		}
	OnExit:
		// Pop the boxes table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadColorsTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "color";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			rgba[0] = 1.0f;
			rgba[1] = 1.0f;
			rgba[2] = 1.0f;
			rgba[3] = 1.0f;
		}
		else
		{
			if (lua_type(&io_luaState, -1) == LUA_TTABLE)
			{
				const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
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
					}
					else
					{
						wereThereErrors = true;
						fprintf_s(stderr, "The color values were not normalized in range [0,1]");
					}
				}
				else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "There are %zu channels instead of 4", arrayLength);
				}
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			}
		}
	OnExit:
		// Pop the Colors table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadBoxDimensions(lua_State & io_luaState)
	{
		bool wereThereErrors = false;
		float dimensions[3] = { 0.0f,0.0f,0.0f };
		const char* const key = "box_dimensions";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "No value for key:\"%s\" was found in the table", key);
		}
		else
		{
			if (lua_type(&io_luaState, -1) == LUA_TTABLE)
			{
				const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
				if (arrayLength == 3)
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
							dimensions[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
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
					InitilizeMeshData(24, 36);
					eae6320::Math::PrimitiveShapeGenerator::CreateBox(*meshData, dimensions[0], dimensions[1], dimensions[2], rgba[0], rgba[1], rgba[2], rgba[3]);
				}
				else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "There are %zu dimensions for box instead of 3", arrayLength);
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			}
		}
	OnExit:

		// Pop the Box Dimensions table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;

	}
	bool LoadMaterialTable(lua_State & io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "material_path";
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
			std::string materialFilePathString;
			std::string errorMessage;
			const char * const sourceRelativePath = lua_tostring(&io_luaState, -1);
			const char * const assetType = "materials";
			if (!eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(sourceRelativePath, assetType, materialFilePathString, &errorMessage))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "Cannot convert Convert Source Relative Path %s To Built Relative Path for Asset Type %s....Error: %s", sourceRelativePath, assetType, errorMessage.c_str());
				goto OnExit;
			}
			materialPath = _strdup(materialFilePathString.c_str());
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a string (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
		}
	OnExit:

		// Pop the material table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadPositionTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "position";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			position.x = 0.0f;
			position.y = 0.0f;
			position.z = 0.0f;
		}
		else
		{
			if (lua_type(&io_luaState, -1) == LUA_TTABLE)
			{
				const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
				float xyz[3] = { 0.0f,0.0f,0.0f };
				if (arrayLength == 3)
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
							xyz[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
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
					position.x = xyz[0];
					position.y = xyz[1];
					position.z = xyz[2];
				}
				else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "There are %zu coordinates instead of 3", arrayLength);
				}
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			}
		}
	OnExit:

		// Pop the position table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadEularTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* key = "eular_angles";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			orientation.x = 0.0f;
			orientation.y = 0.0f;
			orientation.z = 0.0f;
		}
		else
		{
			if (lua_type(&io_luaState, -1) == LUA_TTABLE)
			{
				const size_t arrayLength = static_cast<size_t>(luaL_len(&io_luaState, -1));
				float xyz[3] = { 0.0f,0.0f,0.0f };
				if (arrayLength == 3)
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
							xyz[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
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
					orientation.x = xyz[0];
					orientation.y = xyz[1];
					orientation.z = xyz[2];
				}
				else
				{
					wereThereErrors = true;
					fprintf_s(stderr, "There are %zu coordinates instead of 3", arrayLength);
				}
			}
			else
			{
				wereThereErrors = true;
				fprintf_s(stderr, "The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			}
		}
	OnExit:

		// Pop the position table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	void InitilizeMeshData(const uint32_t i_numberOfVertices, const uint32_t i_numberOfIndices)
	{
		if (meshData)
		{
			if (meshData->indexData)
			{
				free(meshData->indexData);
				meshData->indexData = nullptr;
			}
			if (meshData->vertexData)
			{
				free(meshData->vertexData);
				meshData->vertexData = nullptr;
			}
			free(meshData);
			meshData = nullptr;
		}
		meshData = reinterpret_cast<eae6320::Graphics::MeshData*>(malloc(sizeof(eae6320::Graphics::MeshData)));
		meshData->typeOfIndexData = 16;
		meshData->numberOfVertices = i_numberOfVertices;
		meshData->vertexData = reinterpret_cast<eae6320::Graphics::MeshData::Vertex*>(malloc(i_numberOfVertices * sizeof(eae6320::Graphics::MeshData::Vertex)));
		meshData->numberOfIndices = i_numberOfIndices;
		meshData->indexData = reinterpret_cast<uint16_t*>(malloc(i_numberOfIndices * sizeof(uint16_t)));
	}
	bool CreateBinaryFile(const char*const targetPath)
	{
		bool wereThereErrors = false;
		outputFile = fopen(targetPath, "wb");
		if (!outputFile)
		{
			perror("The target file cannot be opened for writing out data");
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Position Vector
		fwrite(&position, sizeof(eae6320::Math::cVector), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing position vector to %s \n", targetPath);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Orientation Vector
		fwrite(&orientation, sizeof(eae6320::Math::cVector), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing orientation vector to %s \n", targetPath);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Type of index array to file	
		fwrite(&meshData->typeOfIndexData, sizeof(uint32_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing type of index array to %s \n", targetPath);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Number Of Vertices
		fwrite(&meshData->numberOfVertices, sizeof(uint32_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing number of indices to %s \n", targetPath);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Number Of Indices
		fwrite(&meshData->numberOfIndices, sizeof(uint32_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing number of indices to %s \n", targetPath);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Vertex Array
		fwrite(meshData->vertexData, sizeof(eae6320::Graphics::MeshData::Vertex), meshData->numberOfVertices, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing vertex array to %s \n", targetPath);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing Index Array
		fwrite(meshData->indexData, sizeof(uint16_t), meshData->numberOfIndices, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing index array to %s \n", targetPath);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing the Material File Path
		if (!eae6320::AssetBuild::WriteCStringToFile(materialPath, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write material file path to %s file", targetPath);
			goto OnExit;
		}
	OnExit:
		// Closing file only if it got opened
		if (outputFile)
		{
			if (fclose(outputFile))
			{
				wereThereErrors = true;
				eae6320::AssetBuild::OutputErrorMessage("Failed to close target file", __FILE__);
			}
			else
			{
				outputFile = nullptr;
			}
		}
		if (meshData)
		{
			if (meshData->indexData)
			{
				free(meshData->indexData);
				meshData->indexData = nullptr;
			}
			if (meshData->vertexData)
			{
				free(meshData->vertexData);
				meshData->vertexData = nullptr;
			}
			free(meshData);
			meshData = nullptr;
		}
		return !wereThereErrors;
	}
}