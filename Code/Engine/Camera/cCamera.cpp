#include "cCamera.h"

#include "../UserSettings/UserSettings.h"
#include "../Math/Functions.h"

namespace eae6320
{
	namespace Camera
	{
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
		cCamera::cCamera() :
			position(0.0f, 0.0f, 10.0f),
			orientation(),
			fieldOfView(Math::ConvertDegreesToRadians(60.0f)),
			nearPlaneDistance(0.1f),
			farPlaneDistance(100.0f),
			aspectRatio(static_cast<float>(UserSettings::GetResolutionWidth() / UserSettings::GetResolutionHeight())){}
		inline cCamera::~cCamera(){}
		cCamera * cCamera::Initialize()
		{
			cCamera *camera = new cCamera();
			return camera;
		}
	}
}