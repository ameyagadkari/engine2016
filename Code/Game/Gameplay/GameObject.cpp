#include "GameObject.h"
#include "PlayerController.h"
#include "StandardRotationController.h"
#include "DefaultController.h"

#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Platform/Platform.h"
#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/Material.h"

namespace eae6320
{
	namespace Gameplay
	{
		GameObject * GameObject::LoadGameObject(const char * const relativePath)
		{
			bool wereThereErrors = false;
			GameObject *gameObject = new GameObject();

			// Load the binary GameObject file
			Platform::sDataFromFile binaryGameObject;
			{
				std::string errorMessage;
				if (!LoadBinaryFile(relativePath, binaryGameObject, &errorMessage))
				{
					wereThereErrors = true;
					EAE6320_ASSERTF(false, errorMessage.c_str());
					Logging::OutputError("Failed to load the binary gameobject file \"%s\": %s", relativePath, errorMessage.c_str());
					goto OnExit;
				}
			}

			// Casting data to uint8_t* for pointer arithematic
			uint8_t* data = reinterpret_cast<uint8_t*>(binaryGameObject.data);

			uint8_t offsetToAdd = 0;

			// Extracting Binary Data
			{
				// Extracting and setting the initial position
				gameObject->m_transform.m_position = *reinterpret_cast<Math::cVector*>(data);

				// Extracting and setting the initial orientation
				data += sizeof(Math::cVector);
				gameObject->m_transform.SetOrientationEular(*reinterpret_cast<Math::cVector*>(data));
				gameObject->m_transform.UpdateLocalAxes();

				// Extracting Controller Name Hash and initializing the right controller
				data += sizeof(Math::cVector);
				{
					gameObject->m_controllerUUID = *reinterpret_cast<uint32_t *>(data);
					if (gameObject->m_controllerUUID == PlayerController::classUUID)
					{
						gameObject->m_controller = reinterpret_cast<cbController*>(PlayerController::Initialize(1000.0f, 105.0f));
					}
					else if (gameObject->m_controllerUUID == DefaultController::classUUID)
					{
						gameObject->m_controller = reinterpret_cast<cbController*>(DefaultController::Initialize());
					}
					else if (gameObject->m_controllerUUID == StandardRotationController::classUUID)
					{
						gameObject->m_controller = reinterpret_cast<cbController*>(StandardRotationController::Initialize(250.0f));
					}
					else
					{
						gameObject->m_controller = nullptr;
					}
				}

				// Extracting Offset To Add
				data += sizeof(uint32_t);
				offsetToAdd = *reinterpret_cast<uint8_t*>(data);

				// Extracting Material File Path and loading material
				data += sizeof(offsetToAdd);
				{
					const char * const materialPath = reinterpret_cast<const char *>(data);
					if (!Graphics::Material::LoadMaterial(materialPath, *gameObject->m_material))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						Logging::OutputError("Failed to load the binary effect file: %s", materialPath);
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
					if (!Graphics::Mesh::LoadMesh(meshPath, *gameObject->m_mesh))
					{
						wereThereErrors = true;
						EAE6320_ASSERT(false);
						Logging::OutputError("Failed to load the binary mesh file: %s", meshPath);
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
				return nullptr;
			}
			return gameObject;
		}

		GameObject::GameObject() : m_transform(),
			m_mesh(new Graphics::Mesh()),
			m_material(new Graphics::Material()),
			m_controller(nullptr) {}

		GameObject::~GameObject()
		{
			if (m_mesh)delete m_mesh;
			if (m_material)delete m_material;
			if (m_controller)delete m_controller;
		}

#pragma region Gets
		Graphics::Material * GameObject::GetMaterial() const
		{
			return m_material;
		}
		Graphics::Mesh * GameObject::GetMesh() const
		{
			return m_mesh;
		}
		Transform GameObject::GetTransform() const
		{
			return m_transform;
		}
		Transform& GameObject::GetTransformAddress()
		{
			return m_transform;
		}
#pragma endregion

#pragma region Sets
#pragma endregion


		void GameObject::UpdateGameObjectPosition()
		{
			if (m_controller)m_controller->UpdatePosition(m_transform);
		}

		void GameObject::UpdateGameObjectOrientation()
		{
			if (m_controller)m_controller->UpdateOrientation(m_transform);
		}
	}
}
