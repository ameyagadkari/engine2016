#include "Camera.h"

#include "../UserSettings/UserSettings.h"
#include "../UserInput/UserInput.h"
#include "../../Game/Gameplay/cbController.h"
#include "../../Game/Debug/DebugObject.h"

std::vector<eae6320::Camera::Camera*> eae6320::Camera::Camera::s_cameras;
eae6320::Camera::Camera* eae6320::Camera::Camera::s_currentCamera = nullptr;
size_t eae6320::Camera::Camera::s_currentCameraNumber = 0;
size_t eae6320::Camera::Camera::s_maxCameraNumber = s_cameras.size();

#pragma region Gets
eae6320::Gameplay::Transform eae6320::Camera::Camera::GetTransform() const
{
	return m_transform;
}
float eae6320::Camera::Camera::GetFieldOfView()const
{
	return m_fieldOfView;
}
float eae6320::Camera::Camera::GetNearPlaneDistance()const
{
	return m_nearPlaneDistance;
}
float eae6320::Camera::Camera::GetFarPlaneDistance()const
{
	return m_farPlaneDistance;
}
float eae6320::Camera::Camera::GetAspectRatio() const
{
	return m_aspectRatio;
}

eae6320::Gameplay::cbController& eae6320::Camera::Camera::GetController() const
{
	return *m_controller;
}

eae6320::Gameplay::Transform& eae6320::Camera::Camera::GetTransformAddress()
{
	return m_transform;
}
#pragma endregion

#pragma region Sets
void eae6320::Camera::Camera::SetFieldOfView(const float i_fieldOfView)
{
	this->m_fieldOfView = i_fieldOfView;
}
void eae6320::Camera::Camera::SetNearPlaneDistance(const float i_nearPlaneDistance)
{
	this->m_nearPlaneDistance = i_nearPlaneDistance;
}
void eae6320::Camera::Camera::SetFarPlaneDistance(const float i_farPlaneDistance)
{
	this->m_farPlaneDistance = i_farPlaneDistance;
}
#pragma endregion

eae6320::Camera::Camera::Camera
(
	Gameplay::cbController const * const i_controller,
	const Math::cVector i_position,
	const Math::cVector i_orientationEular,
	const float i_fieldOfView,
	const float i_nearPlaneDistance,
	const float i_farPlaneDistance)
	:
	m_transform(i_position, i_orientationEular),
	m_controller(const_cast<Gameplay::cbController*>(i_controller)),
	m_fieldOfView(i_fieldOfView),
	m_nearPlaneDistance(i_nearPlaneDistance),
	m_farPlaneDistance(i_farPlaneDistance),
	m_aspectRatio(static_cast<float>(UserSettings::GetResolutionWidth() / UserSettings::GetResolutionHeight()))
{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
	m_sphere = new Debug::Shapes::DebugObject(true, m_transform.m_position, { 0.0f,1.0f,1.0f });
	m_sphere->CreateSphere(20.0f, 20, 20);
	m_forward = new Debug::Shapes::DebugObject(true, m_transform.m_position, { 1.0f,0.0f,1.0f });
	m_forward->CreateLine(m_transform.m_position + m_transform.m_localAxes.m_forward*100.0f);
#endif
}

eae6320::Camera::Camera::~Camera()
{
	if (m_controller)delete m_controller;
}


bool eae6320::Camera::Camera::CleanUp()
{
	bool wereThereErrors = false;
	for (size_t i = 0; i < s_maxCameraNumber; i++)
	{
		if (s_cameras[i])
		{
			delete s_cameras[i];
			s_cameras[i] = nullptr;
		}
		else wereThereErrors = true;

	}
	return !wereThereErrors;
}

void eae6320::Camera::Camera::UpdateCameraPosition()
{
	if (m_controller)m_controller->UpdatePosition(m_transform);

#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
	m_forward->SetPosition(m_transform.m_position);
	m_forward->UpdateLine(m_transform.m_position + m_transform.m_localAxes.m_forward*100.0f);
#endif
}

void eae6320::Camera::Camera::UpdateCameraOrientation()
{
	if (m_controller)m_controller->UpdateOrientation(m_transform);
}

void eae6320::Camera::Camera::UpdateMaxCameras()
{
	s_maxCameraNumber = s_cameras.size();
	if (s_maxCameraNumber > 0 && !s_currentCamera)
	{
		s_currentCamera = s_cameras[0];
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		s_currentCamera->m_sphere->SetIsDisplayed(false);
#endif
	}
	for (size_t i = 0; i < s_maxCameraNumber; i++)
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		Debug::Shapes::DebugObject::ms_debugObjects.push_back(s_cameras[i]->m_sphere);
		Debug::Shapes::DebugObject::ms_debugObjects.push_back(s_cameras[i]->m_forward);
#endif
	}
}

void eae6320::Camera::Camera::ChangeCurrentCamera()
{
	if (UserInput::GetKeyDown('C'))
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		s_currentCamera->m_sphere->SetIsDisplayed(true);
		s_currentCamera->m_sphere->SetPosition(s_currentCamera->m_transform.m_position);
#endif
		if (s_currentCameraNumber == s_maxCameraNumber - 1)
		{
			s_currentCameraNumber = 0;
		}
		else
		{
			s_currentCameraNumber++;
		}
		s_currentCamera = s_cameras[s_currentCameraNumber];
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		s_currentCamera->m_sphere->SetIsDisplayed(false);
#endif
	}
	if (UserInput::GetKeyDown('V'))
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		s_currentCamera->m_sphere->SetIsDisplayed(true);
		s_currentCamera->m_sphere->SetPosition(s_currentCamera->m_transform.m_position);
#endif
		if (s_currentCameraNumber == 0)
		{
			s_currentCameraNumber = s_maxCameraNumber - 1;
		}
		else
		{
			s_currentCameraNumber--;
		}
		s_currentCamera = s_cameras[s_currentCameraNumber];
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		s_currentCamera->m_sphere->SetIsDisplayed(false);
#endif
	}
}

eae6320::Camera::Camera* eae6320::Camera::Camera::GetCurrentCamera()
{
	if (s_currentCamera)return s_currentCamera;
	return nullptr;
}

void eae6320::Camera::Camera::PushBackToVector(Camera* camera)
{
	s_cameras.push_back(camera);
}
