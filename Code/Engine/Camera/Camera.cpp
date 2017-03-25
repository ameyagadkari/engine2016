#include "Camera.h"

#include "../UserSettings/UserSettings.h"
#include "../UserInput/UserInput.h"
#include "ICameraController.h"


std::vector<eae6320::Camera::Camera*> eae6320::Camera::Camera::sCameras;
eae6320::Camera::Camera* eae6320::Camera::Camera::sCurrentCamera = nullptr;
size_t eae6320::Camera::Camera::sCurrentCameraNumber = 0;
size_t eae6320::Camera::Camera::sMaxCameraNumber = sCameras.size();

#pragma region Gets
eae6320::Math::cVector eae6320::Camera::Camera::GetPosition()const
{
	return position;
}

eae6320::Math::cVector eae6320::Camera::Camera::GetOrientationEular()const
{
	return eularAngles;
}

eae6320::Math::cQuaternion eae6320::Camera::Camera::GetOrientation()const
{
	return orientation;
}
float eae6320::Camera::Camera::GetFieldOfView()const
{
	return fieldOfView;
}
float eae6320::Camera::Camera::GetNearPlaneDistance()const
{
	return nearPlaneDistance;
}
float eae6320::Camera::Camera::GetFarPlaneDistance()const
{
	return farPlaneDistance;
}
float eae6320::Camera::Camera::GetAspectRatio() const
{
	return aspectRatio;
}

/*eae6320::Camera::LocalAxes eae6320::Camera::Camera::GetLocalAxes() const
{
	return localAxes;
}*/
#pragma endregion

#pragma region Sets
void eae6320::Camera::Camera::SetPosition(const Math::cVector position)
{
	this->position = position;
}
void eae6320::Camera::Camera::SetOrientationEular(const Math::cVector eularAngles)
{
	this->eularAngles = eularAngles;
}
void eae6320::Camera::Camera::SetFieldOfView(const float fieldOfView)
{
	this->fieldOfView = fieldOfView;
}
void eae6320::Camera::Camera::SetNearPlaneDistance(const float nearPlaneDistance)
{
	this->nearPlaneDistance = nearPlaneDistance;
}
void eae6320::Camera::Camera::SetFarPlaneDistance(const float farPlaneDistance)
{
	this->farPlaneDistance = farPlaneDistance;
}
#pragma endregion

eae6320::Camera::Camera::Camera
(
	const ICameraController* controller,
	const Math::cVector position,
	const Math::cVector eularAngles,
	const Math::cQuaternion orientation,
	const float fieldOfView,
	const float nearPlaneDistance,
	const float farPlaneDistance)
	:
	orientation(orientation),
	position(position),
	eularAngles(eularAngles),
	controller(controller),
	fieldOfView(fieldOfView),
	nearPlaneDistance(nearPlaneDistance),
	farPlaneDistance(farPlaneDistance),
	aspectRatio(static_cast<float>(UserSettings::GetResolutionWidth() / UserSettings::GetResolutionHeight()))
{
	UpdateCameraAxes();
}

eae6320::Camera::Camera::~Camera() { if (controller)delete controller; }

void eae6320::Camera::Camera::UpdateCameraAxes()
{
	Math::cVector forward;
	forward.z = -(cosf(Math::ConvertDegreesToRadians(eularAngles.y)) * cosf(Math::ConvertDegreesToRadians(eularAngles.x)));
	forward.y = -sinf(Math::ConvertDegreesToRadians(eularAngles.x));
	forward.x = sinf(Math::ConvertDegreesToRadians(eularAngles.y)) * cosf(Math::ConvertDegreesToRadians(eularAngles.x));

	localAxes.m_forward = forward.CreateNormalized();
	localAxes.m_right = Cross(localAxes.m_forward, Math::cVector::up).CreateNormalized();
	localAxes.m_up = Cross(localAxes.m_right, localAxes.m_forward).CreateNormalized();
}

eae6320::Camera::Camera * eae6320::Camera::Camera::Initialize(const ICameraController* controller, const Math::cVector position, const Math::cVector eularAngles, const float fieldOfView, const float nearPlaneDistance, const float farPlaneDistance)
{
	Math::cQuaternion orientationX = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.x), Math::cVector::right);
	Math::cQuaternion orientationY = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.y), Math::cVector::up);
	//Math::cQuaternion orientationZ = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.z), Math::cVector::forward);
	Math::cQuaternion orientation = orientationX*orientationY;// *orientationZ;
	Camera *camera = new Camera(controller, position, eularAngles, orientation, fieldOfView, nearPlaneDistance, farPlaneDistance);
	return camera;
}

bool eae6320::Camera::Camera::CleanUp()
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

void eae6320::Camera::Camera::UpdateCameraPosition()
{
	if (controller)controller->UpdateCameraPosition(localAxes,position);	
}

void eae6320::Camera::Camera::UpdateCameraOrientation()
{
	if (controller)controller->UpdateCameraOrientation(eularAngles);

	UpdateCameraAxes();

	Math::cQuaternion orientationAroundX = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.x), Math::cVector::right);
	Math::cQuaternion orientationAroundY = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.y), Math::cVector::up);
	//Math::cQuaternion orientationAroundZ = Math::cQuaternion(Math::ConvertDegreesToRadians(eularAngles.z), Math::cVector::forward);

	orientation = orientationAroundX*orientationAroundY;// *orientationAroundZ;
}

void eae6320::Camera::Camera::UpdateMaxCameras()
{
	sMaxCameraNumber = sCameras.size();
	if (sMaxCameraNumber > 0 && !sCurrentCamera)
	{
		sCurrentCamera = sCameras[0];
	}
}

void eae6320::Camera::Camera::ChangeCurrentCamera()
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

eae6320::Camera::Camera* eae6320::Camera::Camera::GetCurrentCamera()
{
	if (sCurrentCamera)
	{
		return sCurrentCamera;
	}
	return nullptr;
}

void eae6320::Camera::Camera::PushBackToVector(Camera* camera)
{
	sCameras.push_back(camera);
}

