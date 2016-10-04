#include "cCamera.h"

#include "../UserSettings/UserSettings.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"

namespace eae6320
{
	namespace Camera
	{
		std::vector<eae6320::Camera::cCamera*> cCamera::sCameras;
		cCamera* cCamera::sCurrentCamera = NULL;
		size_t cCamera::sCurrentCameraNumber = 0;
		size_t cCamera::sMaxCameraNumber = cCamera::sCameras.size();
#pragma region Gets
		Math::cVector cCamera::GetPosition()const
		{
			return position;
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
		float cCamera::GetAspectRatio()
		{
			return aspectRatio;
		}
#pragma endregion

#pragma region Sets
		void cCamera::SetPosition(const Math::cVector position)
		{
			this->position = position;
		}
		void cCamera::SetOrientation(const Math::cQuaternion orientation)
		{
			this->orientation = orientation;
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
		cCamera::cCamera(bool isStatic, Math::cVector position, Math::cQuaternion orientation, float fieldOfView, float nearPlaneDistance, float farPlaneDistance) :
			isStatic(isStatic),
			position(position),
			orientation(orientation),
			fieldOfView(fieldOfView),
			nearPlaneDistance(nearPlaneDistance),
			farPlaneDistance(farPlaneDistance),
			aspectRatio(static_cast<float>(UserSettings::GetResolutionWidth() / UserSettings::GetResolutionHeight()))
		{
			eularOrientationOffsetsDegrees[0] = 0.0f;
			eularOrientationOffsetsDegrees[1] = 0.0f;
			eularOrientationOffsetsDegrees[2] = 0.0f;
		}

		inline cCamera::~cCamera() {}

		cCamera * cCamera::Initialize(bool isStatic, Math::cVector position, Math::cQuaternion orientation, float fieldOfView, float nearPlaneDistance, float farPlaneDistance)
		{
			if (!isStatic)
			{
				orientation = Math::cQuaternion();
			}
			cCamera *camera = new cCamera(isStatic, position, orientation, fieldOfView, nearPlaneDistance, farPlaneDistance);
			return camera;
		}

		bool eae6320::Camera::cCamera::CleanUp()
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
			float localOffset[3] = { 0.0f,0.0f,0.0f };
			if (!isStatic)
			{
				if (UserInput::IsKeyPressed(0x4F))//O
					localOffset[2] += 1.0f;
				if (UserInput::IsKeyPressed(0x55))//U
					localOffset[2] -= 1.0f;
				if (UserInput::IsKeyPressed(0x49))//I
					localOffset[1] += 1.0f;
				if (UserInput::IsKeyPressed(0x4B))//K
					localOffset[1] -= 1.0f;
				if (UserInput::IsKeyPressed(0x4A))//J
					localOffset[0] -= 1.0f;
				if (UserInput::IsKeyPressed(0x4C))//L
					localOffset[0] += 1.0f;

				const float speed_unitsPerSecond = 1.0f;
				const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
				localOffset[0] *= offsetModifier;
				localOffset[1] *= offsetModifier;
				localOffset[2] *= offsetModifier;
			}

			position.x += localOffset[0];
			position.y += localOffset[1];
			position.z += localOffset[2];
		}

		void cCamera::UpdateCurrentCameraOrientation()
		{
			float localOffset[3] = { 0.0f,0.0f,0.0f };
			if (!isStatic)
			{
				if (UserInput::IsKeyPressed(0x59))//Y
					localOffset[2] += 1.0f;
				if (UserInput::IsKeyPressed(0x52))//R
					localOffset[2] -= 1.0f;
				if (UserInput::IsKeyPressed(0x54))//T
					localOffset[1] += 1.0f;
				if (UserInput::IsKeyPressed(0x47))//G
					localOffset[1] -= 1.0f;
				if (UserInput::IsKeyPressed(0x46))//F
					localOffset[0] -= 1.0f;
				if (UserInput::IsKeyPressed(0x48))//H
					localOffset[0] += 1.0f;

				const float speed_unitsPerSecond = 10.0f;
				const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
				localOffset[0] *= offsetModifier;
				localOffset[1] *= offsetModifier;
				localOffset[2] *= offsetModifier;
			}
			eularOrientationOffsetsDegrees[0] += localOffset[0];
			eularOrientationOffsetsDegrees[1] += localOffset[1];
			eularOrientationOffsetsDegrees[2] += localOffset[2];

			Math::cQuaternion orientationAroundX = Math::cQuaternion(Math::ConvertDegreesToRadians(eularOrientationOffsetsDegrees[0]), Math::cVector::right);
			Math::cQuaternion orientationAroundY = Math::cQuaternion(Math::ConvertDegreesToRadians(eularOrientationOffsetsDegrees[1]), Math::cVector::up);
			Math::cQuaternion orientationAroundZ = Math::cQuaternion(Math::ConvertDegreesToRadians(eularOrientationOffsetsDegrees[2]), Math::cVector::forward);

			if (!isStatic)
			{
				orientation = orientationAroundX*orientationAroundY*orientationAroundZ;
			}			
		}

		void cCamera::UpdateMaxCameras()
		{
			sMaxCameraNumber = cCamera::sCameras.size();
			if (sMaxCameraNumber > 0)
			{
				sCurrentCamera = sCameras[0];
			}
		}

		void cCamera::ChangeCurrentCamera()
		{
			if (UserInput::IsKeyPressedOnce(0x43))//C Forward Traversing
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
			if (UserInput::IsKeyPressedOnce(0x56))//V Backward Traversing
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
			return NULL;
		}

		void cCamera::PushBackToVector(cCamera *camera)
		{
			sCameras.push_back(camera);
		}
	}
}