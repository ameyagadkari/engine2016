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
		cCamera::cCamera(bool isStatic, Math::cVector eularAngles, Math::cVector position, Math::cQuaternion orientation, float fieldOfView, float nearPlaneDistance, float farPlaneDistance) :
			isStatic(isStatic),
			eularAngles(eularAngles),
			position(position),
			orientation(orientation),
			fieldOfView(fieldOfView),
			nearPlaneDistance(nearPlaneDistance),
			farPlaneDistance(farPlaneDistance),
			aspectRatio(static_cast<float>(UserSettings::GetResolutionWidth() / UserSettings::GetResolutionHeight()))
		{
		}

		inline cCamera::~cCamera() {}

		cCamera * cCamera::Initialize(bool isStatic, Math::cVector eularAngles, Math::cVector position, float fieldOfView, float nearPlaneDistance, float farPlaneDistance)
		{
			Math::cQuaternion orientationX = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.x), Math::cVector::right);
			Math::cQuaternion orientationY = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.y), Math::cVector::up);
			Math::cQuaternion orientationZ = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.z), Math::cVector::forward);
			Math::cQuaternion orientation = orientationX*orientationY*orientationZ;
			cCamera *camera = new cCamera(isStatic, eularAngles, position, orientation, fieldOfView, nearPlaneDistance, farPlaneDistance);
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
			if (!isStatic)
			{
				Math::cVector localOffset = Math::cVector::zero;

				if (UserInput::IsKeyPressed(0x4F))//O
					localOffset.z += 1.0f;
				if (UserInput::IsKeyPressed(0x55))//U
					localOffset.z -= 1.0f;
				if (UserInput::IsKeyPressed(0x49))//I
					localOffset.y += 1.0f;
				if (UserInput::IsKeyPressed(0x4B))//K
					localOffset.y -= 1.0f;
				if (UserInput::IsKeyPressed(0x4A))//J
					localOffset.x -= 1.0f;
				if (UserInput::IsKeyPressed(0x4C))//L
					localOffset.x += 1.0f;

				const float speed_unitsPerSecond = 100.0f;
				const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
				localOffset *= offsetModifier;
				position += localOffset;
			}
		}

		void cCamera::UpdateCurrentCameraOrientation()
		{
			
			if (!isStatic)
			{
				Math::cVector localOffset = Math::cVector::zero;

				if (UserInput::IsKeyPressed(0x59))//Y
					localOffset.z += 1.0f;
				if (UserInput::IsKeyPressed(0x52))//R
					localOffset.z -= 1.0f;
				if (UserInput::IsKeyPressed(0x54))//T
					localOffset.y += 1.0f;
				if (UserInput::IsKeyPressed(0x47))//G
					localOffset.y -= 1.0f;
				if (UserInput::IsKeyPressed(0x46))//F
					localOffset.x -= 1.0f;
				if (UserInput::IsKeyPressed(0x48))//H
					localOffset.x += 1.0f;

				const float speed_unitsPerSecond = 100.0f;
				const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
				localOffset *= offsetModifier;
				eularAngles += localOffset;

				Math::cQuaternion orientationAroundX = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.x), Math::cVector::right);
				Math::cQuaternion orientationAroundY = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.y), Math::cVector::up);
				Math::cQuaternion orientationAroundZ = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.z), Math::cVector::forward);

				orientation = orientationAroundX*orientationAroundY*orientationAroundZ;
			}
		}

		void cCamera::UpdateMaxCameras()
		{
			sMaxCameraNumber = cCamera::sCameras.size();
			if (sMaxCameraNumber > 0 && !sCurrentCamera)
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