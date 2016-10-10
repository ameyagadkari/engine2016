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
			initialOffset[0] = 0.0f;
			initialOffset[1] = 0.0f;
			initialOffset[2] = 0.0f;
			offset[0] = 0.0f;
			offset[1] = 0.0f;
			offset[2] = 0.0f;
			eularOrientationOffsetsDegrees[0] = 0.0f;
			eularOrientationOffsetsDegrees[1] = 0.0f;
			eularOrientationOffsetsDegrees[2] = 0.0f;
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
		Graphics::ConstantBufferData::sDrawCall GameObject::GetDrawCallBufferData()const
		{
			return drawCallBufferData;
		}
		Math::cVector GameObject::GetPosition() const
		{
			return position;
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
		void GameObject::SetDrawCallBufferData(const Graphics::ConstantBufferData::sDrawCall drawCallBufferData)
		{
			this->drawCallBufferData = drawCallBufferData;
		}
		void GameObject::SetPosition(const Math::cVector position)
		{
			this->position = position;
		}
		void GameObject::SetOrientation(const Math::cQuaternion orientation)
		{
			this->orientation = orientation;
		}
#pragma endregion


		void GameObject::UpdateGameObjectPosition()
		{
			float localOffset[3] = { 0.0f,0.0f,0.0f };
			if (!isStatic)
			{
				//if (UserInput::IsKeyPressed(0x45))//E
				//	localOffset[2] += 1.0f;
				//if (UserInput::IsKeyPressed(0x51))//Q
				//	localOffset[2] -= 1.0f;
				if (UserInput::IsKeyPressed(0x57))//W
					localOffset[1] += 1.0f;
				if (UserInput::IsKeyPressed(0x53))//S
					localOffset[1] -= 1.0f;
				if (UserInput::IsKeyPressed(0x41))//A
					localOffset[0] -= 1.0f;
				if (UserInput::IsKeyPressed(0x44))//D
					localOffset[0] += 1.0f;

				const float speed_unitsPerSecond = 0.5f;
				const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
				localOffset[0] *= offsetModifier;
				localOffset[1] *= offsetModifier;
				localOffset[2] *= offsetModifier;
			}	

			offset[0] += localOffset[0];
			offset[1] += localOffset[1];
			offset[2] += localOffset[2];

			position.x = initialOffset[0] + offset[0];
			position.y = initialOffset[1] + offset[1];
			position.z = initialOffset[2] + offset[2];

			drawCallBufferData.g_transform_localToWorld = Math::cMatrix_transformation(orientation, position);
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
					eularOrientationOffsetsDegrees[0] += offsetModifier;
					orientation = Math::cQuaternion(Math::ConvertDegreesToRadians(eularOrientationOffsetsDegrees[0]), Math::cVector::right);
					break;
				case RotationAxis::Y_AXIS:
					eularOrientationOffsetsDegrees[1] += offsetModifier;
					orientation = Math::cQuaternion(Math::ConvertDegreesToRadians(eularOrientationOffsetsDegrees[1]), Math::cVector::up);
					break;
				case RotationAxis::Z_AXIS:
					eularOrientationOffsetsDegrees[2] += offsetModifier;
					orientation = Math::cQuaternion(Math::ConvertDegreesToRadians(eularOrientationOffsetsDegrees[2]), Math::cVector::forward);
					break;
				default:
					break;
				}
			}
			drawCallBufferData.g_transform_localToWorld = Math::cMatrix_transformation(orientation, position);
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

		void GameObject::SetNewInitialOffset(const float x, const float y, const float z)
		{
			initialOffset[0] = x;
			initialOffset[1] = y;
			initialOffset[2] = z;
		}
	}
}