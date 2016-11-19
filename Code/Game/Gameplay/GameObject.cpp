#include "GameObject.h"
#include "CubeController.h"
#include "SnakeController.h"
#include "StandardRotationController.h"

#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Platform/Platform.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/Math/Functions.h"
#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/Material.h"

#include <iostream>

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
				// Extracting and setting the initial position
				gameObject->initialPositionOffset = *reinterpret_cast<Math::cVector*>(data);

				// Extracting and setting the initial orientation
				data += sizeof(Math::cVector);
				gameObject->initialEularOffset = *reinterpret_cast<Math::cVector*>(data);

				// Extracting Controller Name Hash and initializing the right controller
				data += sizeof(Math::cVector);
				{
					const uint32_t classUUID = *reinterpret_cast<uint32_t *>(data);
					if (classUUID == CubeController::classUUID)
					{
						gameObject->controller = reinterpret_cast<IGameObjectController*>(CubeController::Initialize());
					}
					else if (classUUID == SnakeController::classUUID)
					{
						gameObject->controller = reinterpret_cast<IGameObjectController*>(SnakeController::Initialize());
					}
					else if (classUUID == StandardRotationController::classUUID)
					{
						gameObject->controller = reinterpret_cast<IGameObjectController*>(StandardRotationController::Initialize());
					}
					else
					{
						gameObject->controller = NULL;
					}
				}

				// Extracting the rotation axis name and setting the right axis
				data += sizeof(uint32_t);
				{
					gameObject->rotationAxis = static_cast<RotationAxis>(*reinterpret_cast<uint8_t *>(data));
				}

				// Extracting and setting the isStatic
				data += sizeof(uint8_t);
				gameObject->isStatic = *reinterpret_cast<uint8_t*>(data);

				// Extracting and setting the isRotating
				data += sizeof(uint8_t);
				gameObject->isRotating = *reinterpret_cast<uint8_t*>(data);

				// Extracting Offset To Add
				data += sizeof(uint8_t);
				offsetToAdd = *reinterpret_cast<uint8_t*>(data);

				// Extracting Material File Path and loading material
				data += sizeof(offsetToAdd);
				{
					const char * const materialPath = reinterpret_cast<const char *>(data);
					if (!Graphics::Material::LoadMaterial(materialPath, *gameObject->material))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						eae6320::Logging::OutputError("Failed to load the binary effect file: %s", materialPath);
						goto OnExit;
					}
				}

				// Extracting Offset To Add
				data += offsetToAdd;
				offsetToAdd = *reinterpret_cast<uint8_t*>(data);

				// Extracting Mesh File Path and loading mesh
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
			material = new Graphics::Material();
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
			if (material && !material->CleanUpMaterial())
			{
				EAE6320_ASSERT(false);
				Logging::OutputError("Material cleanup failed");
			}
			if (material)
			{
				delete material;
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
		Graphics::Material * GameObject::GetMaterial() const
		{
			return material;
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
		void GameObject::SetMaterial(Graphics::Material * const material)
		{
			this->material = material;
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
	}
}
