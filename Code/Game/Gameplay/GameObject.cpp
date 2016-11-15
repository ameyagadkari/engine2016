#include "GameObject.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Platform/Platform.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/Math/Functions.h"

#include<iostream>

namespace eae6320
{
	namespace Gameplay
	{
		GameObject * GameObject::LoadGameObject(const char * const relativePath)
		{
			bool wereThereErrors = false;
			GameObject *gameObject = new GameObject();

			// Load the binary GameObject file
			eae6320::Platform::sDataFromFile binaryGameObject;
			{
				std::string errorMessage;
				if (!eae6320::Platform::LoadBinaryFile(relativePath, binaryGameObject, &errorMessage))
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, errorMessage.c_str());
					eae6320::Logging::OutputError("Failed to load the binary mesh file \"%s\": %s", relativePath, errorMessage.c_str());
					goto OnExit;
				}
			}

			// Casting data to uint8_t* for pointer arithematic
			uint8_t* data = reinterpret_cast<uint8_t*>(binaryGameObject.data);

			uint8_t offsetToAdd = 0;

			// Extracting Binary Data
			{
				// Extracting Offset To Add
				offsetToAdd = *reinterpret_cast<uint8_t*>(data);

				// Extracting Effect File Path
				data += sizeof(offsetToAdd);
				{
					const char * const effectPath = reinterpret_cast<const char *>(data);
					if (!Graphics::Effect::LoadEffect(effectPath, *gameObject->effect))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Failed to load the binary effect file: %s", effectPath);
						goto OnExit;
					}
				}

				// Extracting Offset To Add
				data += offsetToAdd;
				offsetToAdd = *reinterpret_cast<uint8_t*>(data);

				// Extracting Mesh File Path
				data += sizeof(offsetToAdd);
				{
					const char * const meshPath = reinterpret_cast<const char *>(data);
					if (!Graphics::Mesh::LoadMesh(meshPath, *gameObject->mesh))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Failed to load the binary mesh file: %s", meshPath);
						goto OnExit;
					}
				}
		
				// Extracting Controller Name Hash and initializing the right controller
				data += offsetToAdd;
				{
					const uint32_t classTypeHash = *reinterpret_cast<uint32_t *>(data);
					if (classTypeHash == CubeController::classTypeHash)
					{
						gameObject->controller = reinterpret_cast<IGameObjectController*>(CubeController::Initialize());
					}
					else if (classTypeHash == SnakeController::classTypeHash)
					{
						gameObject->controller = reinterpret_cast<IGameObjectController*>(SnakeController::Initialize());
					}
					else
					{
						gameObject->controller = NULL;
					}
				}

				// Extracting Offset To Add
				data += sizeof(uint32_t);
				offsetToAdd = *reinterpret_cast<uint8_t*>(data);

				// Extracting the rotation axis name and setting the right axis
				data += sizeof(offsetToAdd);
				{
					const char * axis = reinterpret_cast<const char *>(data);
					if (strcmp(axis, "x_axis") == 0)
					{
						gameObject->rotationAxis = eae6320::Gameplay::RotationAxis::X_AXIS;
					}
					else if (strcmp(axis, "y_axis") == 0)
					{
						gameObject->rotationAxis = eae6320::Gameplay::RotationAxis::Y_AXIS;
					}
					else if (strcmp(axis, "z_axis") == 0)
					{
						gameObject->rotationAxis = eae6320::Gameplay::RotationAxis::Z_AXIS;
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Specify a valid axis", axis);
						goto OnExit;
					}
				}

				// Extracting and setting the initial position
				data += offsetToAdd;
				gameObject->initialPositionOffset = *reinterpret_cast<Math::cVector*>(data);

				// Extracting and setting the initial orientation
				data += sizeof(Math::cVector);
				gameObject->initialEularOffset = *reinterpret_cast<Math::cVector*>(data);

				// Extracting and setting the isStatic
				data += sizeof(Math::cVector);
				gameObject->isStatic= *reinterpret_cast<bool*>(data);

				// Extracting and setting the isRotating
				data += sizeof(bool);
				gameObject->isRotating = *reinterpret_cast<bool*>(data);
			}

		OnExit:

			binaryGameObject.Free();

			if (wereThereErrors)
			{
				EAE6320_ASSERT(false);
				Logging::OutputError("Gameobject at %s was not loaded", relativePath);
				return NULL;
			}
			else
			{
				return gameObject;
			}
		}

		inline GameObject::GameObject()
		{
			mesh = new Graphics::Mesh();
			effect = new Graphics::Effect();
		}
		inline GameObject::~GameObject()
		{
			if (mesh && !mesh->CleanUp())
			{
				EAE6320_ASSERT(false);
				Logging::OutputError("Mesh cleanup failed");
			}
			if (mesh)
			{
				delete mesh;
			}
			if (effect && !effect->CleanUpEffect())
			{
				EAE6320_ASSERT(false);
				Logging::OutputError("Effect cleanup failed");
			}
			if (effect)
			{
				delete effect;
			}
			if (controller)
			{
				delete controller;
			}
		}

#pragma region Gets
		int GameObject::GetIsStatic()const
		{
			return isStatic;
		}
		int GameObject::GetIsRotating() const
		{
			return isRotating;
		}
		RotationAxis GameObject::GetRotationAxis() const
		{
			return rotationAxis;
		}
		Graphics::Effect * GameObject::GetEffect() const
		{
			return effect;
		}
		Graphics::Mesh * GameObject::GetMesh() const
		{
			return mesh;
		}
		Math::cVector GameObject::GetPosition() const
		{
			return position;
		}
		Math::cVector GameObject::GetOrientationEular() const
		{
			return eularAngles;
		}
		Math::cQuaternion GameObject::GetOrientation() const
		{
			return orientation;
		}
#pragma endregion

#pragma region Sets
		void GameObject::SetIsStatic(const int isStatic)
		{
			this->isStatic = isStatic;
		}
		void GameObject::SetIsRotating(const int isRotating)
		{
			this->isRotating = isRotating;
		}
		void GameObject::SetRotationAxis(const RotationAxis rotationAxis)
		{
			this->rotationAxis = rotationAxis;
		}
		void GameObject::SetEffect(Graphics::Effect * const effect)
		{
			this->effect = effect;
		}
		void GameObject::SetMesh(Graphics::Mesh * const mesh)
		{
			this->mesh = mesh;
		}
		void GameObject::SetPosition(const Math::cVector position)
		{
			this->position = position;
		}
		void GameObject::SetOrientationEular(const Math::cVector orientation)
		{
			this->eularAngles = eularAngles;
		}
#pragma endregion


		void GameObject::UpdateGameObjectPosition()
		{
			if (!isStatic)
			{
				if (controller)
				{
					position = initialPositionOffset + controller->UpdatePosition();
				}
				else
				{
					position = initialPositionOffset;
				}
			}
			else
			{
				position = initialPositionOffset;
			}
		}

		void GameObject::UpdateGameObjectOrientation()
		{
			if (isRotating)
			{
				if (controller)
				{
					eularAngles = initialEularOffset + controller->UpdateOrientation(rotationAxis);
				}
				else
				{
					eularAngles = initialEularOffset;
				}
			}
			else
			{
				eularAngles = initialEularOffset;
			}
			Math::cQuaternion orientationAroundX = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.x), Math::cVector::right);
			Math::cQuaternion orientationAroundY = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.y), Math::cVector::up);
			Math::cQuaternion orientationAroundZ = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.z), Math::cVector::forward);
			orientation = orientationAroundX*orientationAroundY*orientationAroundZ;
		}

		/*bool GameObject::LoadGameObjectDataTable(lua_State & io_luaState)
		{
			bool wereThereErrors = false;

			//Loading effect
			{
				const char* key = "effect_filepath";
				lua_pushstring(&io_luaState, key);
				lua_gettable(&io_luaState, -2);
				if (lua_isnil(&io_luaState, -1))
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (lua_type(&io_luaState, -1) == LUA_TSTRING)
				{
					const char * const relativePath = lua_tostring(&io_luaState, -1);
					if (!Graphics::Effect::LoadEffect(relativePath, *effect))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Failed to load the binary mesh file: %s", relativePath);
						lua_pop(&io_luaState, 1);
						return !wereThereErrors;
					}
					lua_pop(&io_luaState, 1);
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("The value at \"%s\" must be a string (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
			}


			//Loading mesh
			{
				const char* key = "mesh_filepath";
				lua_pushstring(&io_luaState, key);
				lua_gettable(&io_luaState, -2);
				if (lua_isnil(&io_luaState, -1))
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (lua_type(&io_luaState, -1) == LUA_TSTRING)
				{
					const char * const relativePath = lua_tostring(&io_luaState, -1);
					if (!Graphics::Mesh::LoadMesh(relativePath, *mesh))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Failed to load the binary mesh file: %s", relativePath);
						lua_pop(&io_luaState, 1);
						return !wereThereErrors;
					}
					lua_pop(&io_luaState, 1);
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("The value at \"%s\" must be a string (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
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
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
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
								EAE6320_ASSERT(false);
								eae6320::Logging::OutputError("No value for key:%d was found in the table", i);
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
								EAE6320_ASSERT(false);
								eae6320::Logging::OutputError("The value isn't a number!");
								lua_pop(&io_luaState, 1);
								return !wereThereErrors;
							}
						}
						initialPositionOffset.x = xyz[0];
						initialPositionOffset.y = xyz[1];
						initialPositionOffset.z = xyz[2];
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("There are %d coordinates instead of 3", arrayLength);
						lua_pop(&io_luaState, 1);
						return !wereThereErrors;
					}
					lua_pop(&io_luaState, 1);
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
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
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
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
								EAE6320_ASSERT(false);
								eae6320::Logging::OutputError("No value for key:%d was found in the table", i);
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
								EAE6320_ASSERT(false);
								eae6320::Logging::OutputError("The value isn't a number!");
								lua_pop(&io_luaState, 1);
								return !wereThereErrors;
							}
						}
						initialEularOffset.x = xyz[0];
						initialEularOffset.y = xyz[1];
						initialEularOffset.z = xyz[2];
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("There are %d eular angle values instead of 3", arrayLength);
						lua_pop(&io_luaState, 1);
						return !wereThereErrors;
					}
					lua_pop(&io_luaState, 1);
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("The value at \"%s\" must be a table (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
			}

			//Loading the right controller
			{
				const char* key = "controller";
				lua_pushstring(&io_luaState, key);
				lua_gettable(&io_luaState, -2);
				if (lua_isnil(&io_luaState, -1))
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (lua_type(&io_luaState, -1) == LUA_TSTRING)
				{
					const char* classType = lua_tostring(&io_luaState, -1);
					if (strcmp(classType, CubeController::classType) == 0)
					{
						controller = reinterpret_cast<IGameObjectController*>(CubeController::Initialize());
					}
					else if (strcmp(classType, SnakeController::classType) == 0)
					{
						controller = reinterpret_cast<IGameObjectController*>(SnakeController::Initialize());
					}
					else
					{
						controller = NULL;
					}
					lua_pop(&io_luaState, 1);
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("The value at \"%s\" must be a string (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
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
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
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
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("The value at \"%s\" must be a boolean (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
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
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
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
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("The value at \"%s\" must be a boolean (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
			}

			//Loading correct rotation axis value
			{
				const char* key = "rotation_axis";
				lua_pushstring(&io_luaState, key);
				lua_gettable(&io_luaState, -2);
				if (lua_isnil(&io_luaState, -1))
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("No value for key:\"%s\" was found in the table", key);
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
				if (lua_type(&io_luaState, -1) == LUA_TSTRING)
				{
					const char * axis = lua_tostring(&io_luaState, -1);
					if (strcmp(axis, "x_axis") == 0)
					{
						rotationAxis = eae6320::Gameplay::RotationAxis::X_AXIS;
					}
					else if (strcmp(axis, "y_axis") == 0)
					{
						rotationAxis = eae6320::Gameplay::RotationAxis::Y_AXIS;
					}
					else if (strcmp(axis, "z_axis") == 0)
					{
						rotationAxis = eae6320::Gameplay::RotationAxis::Z_AXIS;
					}
					else
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Specify a valid axis", key);
						lua_pop(&io_luaState, 1);
						return !wereThereErrors;
					}
					lua_pop(&io_luaState, 1);
				}
				else
				{
					wereThereErrors = true;
					EAE6320_ASSERT(false);
					eae6320::Logging::OutputError("The value at \"%s\" must be a string (instead of a %s) ", key, luaL_typename(&io_luaState, -1));
					lua_pop(&io_luaState, 1);
					return !wereThereErrors;
				}
			}
			return !wereThereErrors;
		}*/
	}
}
