// Header Files
//=============

#include "cEffectBuilder.h"

#include <sstream>
#include <iostream>

#include "../../Engine/Asserts/Asserts.h"
#include "../AssetBuildLibrary/UtilityFunctions.h"
#include "../../Engine/Platform/Platform.h"
#include "../../External/Lua/Includes.h"
#include "../../Engine/Graphics/cRenderState.h"

// Inherited Implementation
//=========================

// Build
//------

// Helper Function Declarations
//=============================

namespace
{
	const char * vertexShaderPath = NULL;
	const char * fragmentShaderPath = NULL;
	uint8_t renderStateBits = 0;

	FILE * outputFile = NULL;

	bool LoadInitialTable(lua_State& io_luaState);
	bool LoadShadersTable(lua_State& io_luaState);
	bool LoadVertexShaderPath(lua_State& io_luaState);
	bool LoadFragmentShaderPath(lua_State& io_luaState);
	bool LoadRenderStatesTable(lua_State& io_luaState);
	bool LoadAlphaTransparencyTable(lua_State& io_luaState);
	bool LoadDepthBufferingTable(lua_State& io_luaState);
	bool LoadDrawBothTriangleSidesTable(lua_State& io_luaState);
	bool LoadWireFrameModeTable(lua_State& io_luaState);
}

bool eae6320::AssetBuild::cEffectBuilder::Build(const std::vector<std::string>& i_arguments)
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
		// Render State Bits
		fwrite(&renderStateBits, sizeof(uint8_t), 1, outputFile);
		if (ferror(outputFile))
		{
			fprintf_s(stderr, "Error writing render state bits bool to %s \n", m_path_target);
			wereThereErrors = true;
			goto OnExit;
		}
		// Writing the Vertex Shader Path
		if (!WriteCStringToFile(vertexShaderPath, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write vertex shader path to %s file", m_path_target);
			goto OnExit;
		}
		// Writing the Fragment Shader Path
		if (!WriteCStringToFile(fragmentShaderPath, outputFile))
		{
			wereThereErrors = true;
			fprintf_s(stderr, "Failed to write fragment shader path to %s file", m_path_target);
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

	if (vertexShaderPath)
	{
		delete vertexShaderPath;
	}
	if (fragmentShaderPath)
	{
		delete fragmentShaderPath;
	}

	return !wereThereErrors;
}

// Helper Function Definitions
//=============================

namespace
{
	bool LoadInitialTable(lua_State& io_luaState)
	{
		if (!LoadShadersTable(io_luaState))
		{
			return false;
		}
		if (!LoadRenderStatesTable(io_luaState))
		{
			return false;
		}
		return true;
	}
	bool LoadShadersTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "shaders";
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
			if (!LoadVertexShaderPath(io_luaState))
			{
				wereThereErrors = true;
				goto OnExit;
			}

			if (!LoadFragmentShaderPath(io_luaState))
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

		// Pop the Shaders table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;

	}

	bool LoadVertexShaderPath(lua_State & io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "vertex_shader";
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
			std::string vertexShaderPathString;
			std::string errorMessage;
			const char * const sourceRelativePath = lua_tostring(&io_luaState, -1);
			const char * const assetType = "shaders";
			if(!eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(sourceRelativePath, assetType, vertexShaderPathString, &errorMessage))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "Cannot convert Convert Source Relative Path %s To Built Relative Path for Asset Type %s....Error: %s", sourceRelativePath, assetType, errorMessage.c_str());
				goto OnExit;
			}
			vertexShaderPath = _strdup(vertexShaderPathString.c_str());
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

	bool LoadFragmentShaderPath(lua_State & io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "fragment_shader";
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
			std::string fragmentShaderPathString;
			std::string errorMessage;
			const char * const sourceRelativePath = lua_tostring(&io_luaState, -1);
			const char * const assetType = "shaders";
			if (!eae6320::AssetBuild::ConvertSourceRelativePathToBuiltRelativePath(sourceRelativePath, assetType, fragmentShaderPathString, &errorMessage))
			{
				wereThereErrors = true;
				fprintf_s(stderr, "Cannot convert Convert Source Relative Path %s To Built Relative Path for Asset Type %s....Error: %s", sourceRelativePath, assetType, errorMessage.c_str());
				goto OnExit;
			}
			fragmentShaderPath = _strdup(fragmentShaderPathString.c_str());
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

	bool LoadRenderStatesTable(lua_State & io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "render_states";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			renderStateBits = 0x02;
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TTABLE)
		{
			if (!LoadAlphaTransparencyTable(io_luaState))
			{
				wereThereErrors = true;
				goto OnExit;
			}

			if (!LoadDepthBufferingTable(io_luaState))
			{
				wereThereErrors = true;
				goto OnExit;
			}

			if (!LoadDrawBothTriangleSidesTable(io_luaState))
			{
				wereThereErrors = true;
				goto OnExit;
			}

			if (!LoadWireFrameModeTable(io_luaState))
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

		// Pop the Render States table
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}

	bool LoadAlphaTransparencyTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "alpha_transparency";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TBOOLEAN)
		{
			if (lua_toboolean(&io_luaState, -1))
			{
				eae6320::Graphics::RenderStates::EnableAlphaTransparency(renderStateBits);
			}
			else
			{
				eae6320::Graphics::RenderStates::DisableAlphaTransparency(renderStateBits);
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a boolean (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadDepthBufferingTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "depth_buffering";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			eae6320::Graphics::RenderStates::EnableDepthBuffering(renderStateBits);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TBOOLEAN)
		{
			if (lua_toboolean(&io_luaState, -1))
			{
				eae6320::Graphics::RenderStates::EnableDepthBuffering(renderStateBits);
			}
			else
			{
				eae6320::Graphics::RenderStates::DisableDepthBuffering(renderStateBits);
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a boolean (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadDrawBothTriangleSidesTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "draw_both_triangle_sides";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TBOOLEAN)
		{
			if (lua_toboolean(&io_luaState, -1))
			{
				eae6320::Graphics::RenderStates::EnableDrawingBothTriangleSides(renderStateBits);
			}
			else
			{
				eae6320::Graphics::RenderStates::DisableDrawingBothTriangleSides(renderStateBits);
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a boolean (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
	bool LoadWireFrameModeTable(lua_State& io_luaState)
	{
		bool wereThereErrors = false;
		const char* const key = "wire_frame_mode";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		if (lua_isnil(&io_luaState, -1))
		{
			eae6320::Graphics::RenderStates::DisableWireFrameMode(renderStateBits);
			goto OnExit;
		}
		if (lua_type(&io_luaState, -1) == LUA_TBOOLEAN)
		{
			if (lua_toboolean(&io_luaState, -1))
			{
				eae6320::Graphics::RenderStates::EnableWireFrameMode(renderStateBits);
			}
			else
			{
				eae6320::Graphics::RenderStates::DisableWireFrameMode(renderStateBits);
			}
		}
		else
		{
			wereThereErrors = true;
			fprintf_s(stderr, "The value at \"%s\" must be a boolean (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
			goto OnExit;
		}

	OnExit:
		lua_pop(&io_luaState, 1);

		return !wereThereErrors;
	}
}