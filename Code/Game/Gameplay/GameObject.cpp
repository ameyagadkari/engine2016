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
		inline GameObject::GameObject() 
		{
			initialPositionOffset = Math::cVector::zero;
			postionOffset = Math::cVector::zero;
			initialEularOffset = Math::cVector::zero;
			eularOffset = Math::cVector::zero;
		}
		inline GameObject::~GameObject() 
		{
			if (mesh && !mesh->CleanUp())
			{
				EAE6320_ASSERT(false);
				Logging::OutputError("Mesh cleanup failed");
			}
			if (mesh)
				delete mesh;
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
			Math::cVector localPositionOffset = Math::cVector::zero;
			if (!isStatic)
			{
				//if (UserInput::IsKeyPressed(0x45))//E
				//	localPositionOffset.z += 1.0f;
				//if (UserInput::IsKeyPressed(0x51))//Q
				//	localPositionOffset.z -= 1.0f;
				if (UserInput::IsKeyPressed(0x57))//W
					localPositionOffset.y += 1.0f;
				if (UserInput::IsKeyPressed(0x53))//S
					localPositionOffset.y -= 1.0f;
				if (UserInput::IsKeyPressed(0x41))//A
					localPositionOffset.x -= 1.0f;
				if (UserInput::IsKeyPressed(0x44))//D
					localPositionOffset.x += 1.0f;

				const float speed_unitsPerSecond = 0.5f;
				const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
				localPositionOffset *= offsetModifier;
				postionOffset += localPositionOffset;				
			}	
			position = initialPositionOffset + postionOffset;
		}

		void GameObject::UpdateGameObjectOrientation()
		{
			if (isRotating)
			{
				const float rotationSpeed = 100.0f;
				const float offsetModifier = rotationSpeed * Time::GetElapsedSecondCount_duringPreviousFrame();
				switch (rotationAxis)
				{
				case RotationAxis::X_AXIS:
					eularOffset.x += offsetModifier;
					break;
				case RotationAxis::Y_AXIS:
					eularOffset.y += offsetModifier;
					break;
				case RotationAxis::Z_AXIS:
					eularOffset.z += offsetModifier;
					break;
				default:
					break;				
				}						
			}
			eularAngles = initialEularOffset + eularOffset;

			Math::cQuaternion orientationAroundX = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.x), Math::cVector::right);
			Math::cQuaternion orientationAroundY = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.y), Math::cVector::up);
			Math::cQuaternion orientationAroundZ = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.z), Math::cVector::forward);

			orientation = orientationAroundX*orientationAroundY*orientationAroundZ;
		}

		GameObject* GameObject::Initilaize(const char* relativePath)
		{
			GameObject *gameObject = new GameObject();
			gameObject->mesh = Graphics::Mesh::LoadMesh(relativePath);
			gameObject->isStatic = true;
			gameObject->isRotating = false;
			gameObject->rotationAxis = RotationAxis::X_AXIS;
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