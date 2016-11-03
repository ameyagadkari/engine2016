#include "GameObject.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/Math/Functions.h"

namespace eae6320
{
	namespace Gameplay
	{
		inline GameObject::GameObject() :
			isStatic(true), 
			isRotating(false), 
			rotationAxis(RotationAxis::X_AXIS),
			initialEularOffset(Math::cVector::zero),
			initialPositionOffset(Math::cVector::zero)
		{
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
			if (controller)
			{
				delete controller;
			}
		}

#pragma region Gets
		bool GameObject::GetIsStatic()const
		{
			return isStatic;
		}
		bool GameObject::GetIsRotating() const
		{
			return isRotating;
		}
		RotationAxis GameObject::GetRotationAxis() const
		{
			return rotationAxis;
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
		void GameObject::SetIsStatic(const bool isStatic)
		{
			this->isStatic = isStatic;
		}
		void GameObject::SetIsRotating(const bool isRotating)
		{
			this->isRotating = isRotating;
		}
		void GameObject::SetRotationAxis(const RotationAxis rotationAxis)
		{
			this->rotationAxis = rotationAxis;
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

		GameObject* GameObject::Initilaize(const char * const relativePath, const char * const classType)
		{
			GameObject *gameObject = new GameObject();
			if (strcmp(classType, CubeController::classType) == 0)
			{
				gameObject->controller = reinterpret_cast<IGameObjectController*>(CubeController::Initialize());
			}
			else if (strcmp(classType, SnakeController::classType) == 0)
			{
				gameObject->controller = reinterpret_cast<IGameObjectController*>(SnakeController::Initialize());
			}
			else
			{
				gameObject->controller = NULL;
			}
			gameObject->mesh = Graphics::Mesh::LoadMesh(relativePath);			
			if (gameObject->mesh)
			{
				return gameObject;
			}
			EAE6320_ASSERT(false);
			Logging::OutputError("Mesh at %s was not loaded", relativePath);
			return NULL;
		}

		void GameObject::SetNewInitialPositionOffset(const Math::cVector startPosition)
		{
			initialPositionOffset = startPosition;
		}

		void GameObject::SetNewInitialEularOffset(const Math::cVector startOrientaion)
		{
			initialEularOffset = startOrientaion;
		}
	}
}