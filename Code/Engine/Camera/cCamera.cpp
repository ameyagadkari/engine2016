#include "cCamera.h"

#include "../UserSettings/UserSettings.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include "../Physics/Physics.h"

namespace eae6320
{
	namespace Camera
	{
		std::vector<cCamera*> cCamera::sCameras;
		cCamera* cCamera::sCurrentCamera = nullptr;
		size_t cCamera::sCurrentCameraNumber = 0;
		size_t cCamera::sMaxCameraNumber = sCameras.size();
#pragma region Gets
		Math::cVector cCamera::GetPosition()const
		{
			return position;
		}
		Math::cVector cCamera::GetOrientationEular()const
		{
			return eularAngles;
		}
		Math::cQuaternion cCamera::GetOrientation()const
		{
			return orientation;
		}
		float cCamera::GetFieldOfView()const
		{
			return fieldOfView;
		}
		float cCamera::GetNearPlaneDistance()const
		{
			return nearPlaneDistance;
		}
		float cCamera::GetFarPlaneDistance()const
		{
			return farPlaneDistance;
		}
		float cCamera::GetAspectRatio() const
		{
			return aspectRatio;
		}
#pragma endregion

#pragma region Sets
		void cCamera::SetPosition(const Math::cVector position)
		{
			this->position = position;
		}
		void cCamera::SetOrientationEular(const Math::cVector eularAngles)
		{
			this->eularAngles = eularAngles;
		}
		void cCamera::SetFieldOfView(const float fieldOfView)
		{
			this->fieldOfView = fieldOfView;
		}
		void cCamera::SetNearPlaneDistance(const float nearPlaneDistance)
		{
			this->nearPlaneDistance = nearPlaneDistance;
		}
		void cCamera::SetFarPlaneDistance(const float farPlaneDistance)
		{
			this->farPlaneDistance = farPlaneDistance;
		}
#pragma endregion
		cCamera::cCamera(
			const bool isFlyCam,
			const bool isStatic,
			const Math::cVector eularAngles,
			const Math::cVector position,
			const Math::cQuaternion orientation,
			const float fieldOfView,
			const float nearPlaneDistance,
			const float farPlaneDistance)
			:
			position(position),
			orientation(orientation),
			eularAngles(eularAngles),
			fieldOfView(fieldOfView),
			nearPlaneDistance(nearPlaneDistance),
			farPlaneDistance(farPlaneDistance),
			aspectRatio(static_cast<float>(UserSettings::GetResolutionWidth() / UserSettings::GetResolutionHeight())),
			isStatic(isStatic),
			isFlyCam(isFlyCam)
		{
			UpdateCameraAxes();
		}

		inline cCamera::~cCamera() {}

		void cCamera::UpdateCameraAxes()
		{
			// Calculate the new Front vector
			Math::cVector front;
			front.z = -(cos(Math::ConvertDegreesToRadians(eularAngles.y)) * cos(Math::ConvertDegreesToRadians(eularAngles.x)));
			front.y = -sin(Math::ConvertDegreesToRadians(eularAngles.x));
			front.x = sin(Math::ConvertDegreesToRadians(eularAngles.y)) * cos(Math::ConvertDegreesToRadians(eularAngles.x));

			this->localAxes.front = front.CreateNormalized();
			// Also re-calculate the Right and Up vector
			localAxes.right = (Cross(this->localAxes.front, Math::cVector::up)).CreateNormalized();  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
			localAxes.up = (Cross(localAxes.right, this->localAxes.front)).CreateNormalized();
		}

		cCamera * cCamera::Initialize(const bool isFlyCam, const bool isStatic, const Math::cVector eularAngles, const Math::cVector position, const float fieldOfView, const float nearPlaneDistance, const float farPlaneDistance)
		{
			Math::cQuaternion orientationX = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.x), Math::cVector::right);
			Math::cQuaternion orientationY = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.y), Math::cVector::up);
			//Math::cQuaternion orientationZ = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.z), Math::cVector::forward);
			Math::cQuaternion orientation = orientationX*orientationY;// *orientationZ;
			cCamera *camera = new cCamera(isFlyCam, isStatic, eularAngles, position, orientation, fieldOfView, nearPlaneDistance, farPlaneDistance);
			return camera;
		}

		bool cCamera::CleanUp()
		{
			bool wereThereErrors = false;
			for (size_t i = 0; i < sMaxCameraNumber; i++)
			{
				if (sCameras[i])
				{
					delete sCameras[i];
				}
				else
				{
					wereThereErrors = true;
				}
			}
			sCameras._Pop_back_n(sMaxCameraNumber);
			return !wereThereErrors;
		}

		void cCamera::UpdateCurrentCameraPosition()
		{
			if (!isStatic)
			{
				Math::cVector localOffset = Math::cVector::zero;

				if (UserInput::IsKeyPressed('W'))
					localOffset += localAxes.front;
				if (UserInput::IsKeyPressed('S'))
					localOffset -= localAxes.front;
				if (UserInput::IsKeyPressed('D'))
					localOffset += localAxes.right;
				if (UserInput::IsKeyPressed('A'))
					localOffset -= localAxes.right;
				//if (UserInput::IsKeyPressed(0x4A))//J
				//	localOffset.x -= 1.0f;
				//if (UserInput::IsKeyPressed(0x4C))//L
				//	localOffset.x += 1.0f;

				/*if (!isFlyCam)
				{
					localOffset.y = 0.0f;
					if (!Physics::isPlayerOnGround)
					{
						localOffset -= localAxes.up;
					}
				}*/

				const float speed_unitsPerSecond = 250.0f;
				const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
				if (!isFlyCam)
				{
					Physics::CheckCollision(position + localOffset, localAxes, localOffset);
				}
				localOffset *= offsetModifier;
				position += localOffset;
			}
		}

		void cCamera::UpdateCurrentCameraOrientation()
		{

			if (!isStatic)
			{
				Math::cVector localOffset = Math::cVector::zero;

				//if (UserInput::IsKeyPressed(0x59))//Y
				//	localOffset.z += 1.0f;
				//if (UserInput::IsKeyPressed(0x52))//R
				//	localOffset.z -= 1.0f;
				if (UserInput::IsKeyPressed('H'))
					localOffset.y += 1.0f;
				if (UserInput::IsKeyPressed('F'))
					localOffset.y -= 1.0f;
				if (UserInput::IsKeyPressed('G'))
					localOffset.x += 1.0f;
				if (UserInput::IsKeyPressed('T'))
					localOffset.x -= 1.0f;

				const float speed_unitsPerSecond = 100.0f;
				const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
				localOffset *= offsetModifier;
				eularAngles += localOffset;

				if (!isFlyCam)
				{
					if (eularAngles.x > 89.0f)
					{
						eularAngles.x = 89.0f;
					}
					if (eularAngles.x < -89.0f)
					{
						eularAngles.x = -89.0f;
					}
				}
				UpdateCameraAxes();
				Math::cQuaternion orientationAroundX = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.x), Math::cVector::right);
				Math::cQuaternion orientationAroundY = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.y), Math::cVector::up);
				//Math::cQuaternion orientationAroundZ = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.z), Math::cVector::forward);

				orientation = orientationAroundX*orientationAroundY;// *orientationAroundZ;

			}
		}

		void cCamera::UpdateMaxCameras()
		{
			sMaxCameraNumber = sCameras.size();
			if (sMaxCameraNumber > 0 && !sCurrentCamera)
			{
				sCurrentCamera = sCameras[0];
			}
		}

		void cCamera::ChangeCurrentCamera()
		{
			if (UserInput::IsKeyPressedOnce('C'))
			{
				if (sCurrentCameraNumber == sMaxCameraNumber - 1)
				{
					sCurrentCameraNumber = 0;
				}
				else
				{
					sCurrentCameraNumber++;
				}
				sCurrentCamera = sCameras[sCurrentCameraNumber];
			}
			if (UserInput::IsKeyPressedOnce('V'))
			{
				if (sCurrentCameraNumber == 0)
				{
					sCurrentCameraNumber = sMaxCameraNumber - 1;
				}
				else
				{
					sCurrentCameraNumber--;
				}
				sCurrentCamera = sCameras[sCurrentCameraNumber];
			}
		}

		cCamera * cCamera::GetCurrentCamera()
		{
			if (sCurrentCamera)
			{
				return sCurrentCamera;
			}
			return nullptr;
		}

		void cCamera::PushBackToVector(cCamera *camera)
		{
			sCameras.push_back(camera);
		}
	}
}
