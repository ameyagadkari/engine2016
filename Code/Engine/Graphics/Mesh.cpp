#include<iostream>
#include<cmath>

#include "Mesh.h"
#include "../Asserts/Asserts.h"
#include "../Logging/Logging.h"
#include "../Platform/Platform.h"
#include "../../Tools/AssetBuildLibrary/UtilityFunctions.h"
#include "../../External/Lua/Includes.h"

// Helper Function Declarations
//=============================

namespace
{
	using namespace eae6320::Graphics;

	bool LoadInitialTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadVerticesTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadPositionsTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadXYTable(lua_State& io_luaState, MeshData&meshData, int index);
	bool LoadColorsTable(lua_State& io_luaState, MeshData&meshData);
	bool LoadRGBATable(lua_State& io_luaState, MeshData&meshData, int index);
	bool LoadIndicesTable(lua_State& io_luaState, MeshData&meshData);

	bool CheckIfColorIsInCorrectFormat(float *rgba);
}

eae6320::Graphics::Mesh* eae6320::Graphics::Mesh::LoadMesh(const char * const relativePath)
{
	{
		bool wereThereErrors = false;
		Mesh *mesh = new Mesh();
		// Create a new Lua state
		lua_State* luaState = NULL;
		{
			luaState = luaL_newstate();
			if (!luaState)
			{
				wereThereErrors = true;
				eae6320::Logging::OutputError("Failed to create a new Lua state");
				goto OnExit;
			}
		}

		// Open the standard libraries
		luaL_openlibs(luaState);

		// Load and execute it
		{
			std::string errorMessage;
			if (Platform::DoesFileExist(relativePath, &errorMessage))
			{

				// Load the asset file as a "chunk",
				// meaning there will be a callable function at the top of the stack
				const int stackTopBeforeLoad = lua_gettop(luaState);
				{
					const int luaResult = luaL_loadfile(luaState, relativePath);
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
				AssetBuild::OutputErrorMessage(errorMessage.c_str(), relativePath);
				goto OnExit;
			}
		}

		//If this code is reached the asset file was loaded successfully,
		//and its table is now at index -1
		MeshData *meshData = reinterpret_cast<MeshData*>(malloc(sizeof(MeshData)));
		if (!LoadInitialTable(*luaState, *meshData))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			Logging::OutputError("Failed to load initial table");
			goto OnExit;
		}
		lua_pop(luaState, 1);


		if (!mesh->Initialize(*meshData))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			Logging::OutputError("Failed to initialize mesh");
			goto OnExit;
		}
		if (meshData->indexData)
		{
			free(meshData->indexData);
		}
		if (meshData->vertexData)
		{
			free(meshData->vertexData);
		}
		if (meshData)
		{
			free(meshData);
		}
	OnExit:

		if (luaState)
		{
			// If I haven't made any mistakes
			// there shouldn't be anything on the stack
			// regardless of any errors
			EAE6320_ASSERT(lua_gettop(luaState) == 0);

			lua_close(luaState);
			luaState = NULL;
		}
		

		if (wereThereErrors)
		{
			return NULL;
		}
		else
		{
			return mesh;
		}
	}
}

// Helper Function Definitions
//=============================

namespace
{
	bool LoadInitialTable(lua_State& io_luaState, MeshData&meshData)
	{
		if (!LoadVerticesTable(io_luaState, meshData))
		{
			return false;
		}
		if (!LoadIndicesTable(io_luaState, meshData))
		{
			return false;
		}

		return true;
	}
	bool LoadVerticesTable(lua_State& io_luaState, MeshData&meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "vertices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			if (!LoadPositionsTable(io_luaState, meshData))
			{
				wereThereErrors = true;
				goto OnExit;
			}
			if (!LoadColorsTable(io_luaState, meshData))
			{
				wereThereErrors = true;
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:

		// Pop the Vertices table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;

	}
	bool LoadPositionsTable(lua_State& io_luaState, MeshData&meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "positions";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const int positions = luaL_len(&io_luaState, -1);
			meshData.numberOfVertices = positions;
			if (meshData.numberOfVertices >= MINIMUM_NUMBER_OF_VERTICES)
			{
				meshData.vertexData = reinterpret_cast<MeshData::Vertex*>(malloc(meshData.numberOfVertices * sizeof(MeshData::Vertex)));
				if (!meshData.vertexData)
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("Could not initialize the Vertex Data");
					goto OnExit;
				}

			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("Not enough vertices to draw a triangle");
				goto OnExit;
			}
			eae6320::Logging::OutputMessage("Iterating through every x,y coordinate tables in position table");
			for (int i = 1; i <= meshData.numberOfVertices; ++i)
			{
				if (!LoadXYTable(io_luaState, meshData, (i - 1)))
				{
					wereThereErrors = true;
					goto OnExit;
				}
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:

		// Pop the Positions table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadXYTable(lua_State& io_luaState, MeshData&meshData, int index)
	{
		bool wereThereErrors = false;
		lua_pushinteger(&io_luaState, index + 1);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("No value for key:%d was found in the table", (index + 1));
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const int arrayLength = luaL_len(&io_luaState, -1);
			float xy[2] = { 0.0f,0.0f };
			if (arrayLength == 2)
			{
				eae6320::Logging::OutputMessage("Iterating through XY for index:\"%d\"", (index + 1));
				// Remember that Lua arrays are 1-based and not 0-based!
				for (int i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("No value for key:%d was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
					{
						xy[i - 1] = static_cast<float>(lua_tonumber(&io_luaState, -1));
						lua_pop(&io_luaState, 1);
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
				meshData.vertexData[index].x = xy[0];
				meshData.vertexData[index].y = xy[1];
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("There are %d coordinates instead of 2", arrayLength);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("The value at \"%d\" must be a table (instead of a %s) ", (index + 1), luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the XY table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadColorsTable(lua_State& io_luaState, MeshData&meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "colors";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const int colors = luaL_len(&io_luaState, -1);
			if (colors != meshData.numberOfVertices)
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("The number of color tables and position tables do not match");
				goto OnExit;
			}
			else
			{
				eae6320::Logging::OutputMessage("Iterating through every rgba tables in colors table");
				for (int i = 1; i <= meshData.numberOfVertices; ++i)
				{
					if (!LoadRGBATable(io_luaState, meshData, (i - 1)))
					{
						wereThereErrors = true;
						goto OnExit;
					}
				}
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the Colors table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadRGBATable(lua_State& io_luaState, MeshData&meshData, int index)
	{
		bool wereThereErrors = false;
		lua_pushinteger(&io_luaState, index + 1);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("No value for key:%d was found in the table", (index + 1));
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const int arrayLength = luaL_len(&io_luaState, -1);
			float rgba[4] = { 0.0f,0.0f,0.0f,1.0f };
			if ((arrayLength == 3) || (arrayLength == 4))
			{
				eae6320::Logging::OutputMessage("Iterating through RGBA for index:\"%d\"", (index + 1));
				// Remember that Lua arrays are 1-based and not 0-based!
				for (int i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("No value for key:%d was found in the table", i);
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
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}

				if (CheckIfColorIsInCorrectFormat(rgba))
				{
					meshData.vertexData[index].r = static_cast<uint8_t>(roundf(rgba[0] * 255.0f));
					meshData.vertexData[index].g = static_cast<uint8_t>(roundf(rgba[1] * 255.0f));
					meshData.vertexData[index].b = static_cast<uint8_t>(roundf(rgba[2] * 255.0f));
					meshData.vertexData[index].a = static_cast<uint8_t>(roundf(rgba[3] * 255.0f));
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("The color values were not normalized in range [0,1]", arrayLength);
					goto OnExit;
				}

			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("There are %d channels instead of 3 or 4", arrayLength);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("The value at \"%d\" must be a table (instead of a %s)", (index + 1), luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the RGBA table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadIndicesTable(lua_State& io_luaState, MeshData&meshData)
	{
		bool wereThereErrors = false;
		const char* const key = "indices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			const int arrayLength = luaL_len(&io_luaState, -1);
			if (arrayLength % 3 == 0)
			{
				meshData.numberOfIndices = arrayLength;
				if (meshData.numberOfIndices >= MINIMUM_NUMBER_OF_INDICES)
				{
					meshData.indexData = reinterpret_cast<uint16_t*>(malloc(meshData.numberOfIndices * sizeof(uint16_t)));
				}
				eae6320::Logging::OutputMessage("Iterating through indices array");
				// Remember that Lua arrays are 1-based and not 0-based!
				for (int i = 1; i <= arrayLength; ++i)
				{
					lua_pushinteger(&io_luaState, i);
					lua_gettable(&io_luaState, -2);
					if (lua_isnil(&io_luaState, -1))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("No value for key:%d was found in the table", i);
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
					if (lua_type(&io_luaState, -1) == LUA_TNUMBER)
					{
#if defined( EAE6320_PLATFORM_D3D )
						meshData.indexData[meshData.numberOfIndices - i] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
#elif defined( EAE6320_PLATFORM_GL )
						meshData.indexData[i - 1] = static_cast<uint16_t>(lua_tonumber(&io_luaState, -1));
#endif
						lua_pop(&io_luaState, 1);
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("The value isn't a number!");
						lua_pop(&io_luaState, 1);
						goto OnExit;
					}
				}
			}
			else
			{
				wereThereErrors = true;
				EAE6320_ASSERT(false);
				eae6320::Logging::OutputError("There are %d indices which is incorrect as we are drawing triangles", arrayLength);
				goto OnExit;
			}
		}
		else
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("The value at \"%s\" must be a table (instead of a %s)", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}
	OnExit:
		// Pop the RGBA table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool CheckIfColorIsInCorrectFormat(float *rgba)
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
}