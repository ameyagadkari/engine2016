#include "Camera.h"

#include "../UserSettings/UserSettings.h"
#include "../UserInput/UserInput.h"
#include "../../Game/Gameplay/cbController.h"
#include "../../Game/Debug/DebugObject.h"

#define NUMBER_OF_SPHERES 3

std::vector<eae6320::Camera::Camera*> eae6320::Camera::Camera::s_cameras;
eae6320::Camera::Camera* eae6320::Camera::Camera::s_currentCamera = nullptr;
size_t eae6320::Camera::Camera::s_currentCameraNumber = 0;
size_t eae6320::Camera::Camera::s_maxCameraNumber = s_cameras.size();

namespace
{
	bool sphereState = true;
}

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
	//m_sphere = reinterpret_cast<Debug::Shapes::DebugObject**>(malloc(NUMBER_OF_SPHERES * sizeof(Debug::Shapes::DebugObject)));
	/*for (size_t i = 0; i < NUMBER_OF_SPHERES; i++)
	{
		m_sphere[i] = new Debug::Shapes::DebugObject(true, position, { 1.0f,0.5f,0.0f });
	}
	m_sphere[0]->CreateSphere(20.0f, 20, 20);
	m_sphere[1]->CreateLine(Math::cVector::zero);
	m_sphere[2]->CreateLine(Math::cVector::zero);*/
#endif
}

eae6320::Camera::Camera::~Camera() 
{ 
	if (m_controller)delete m_controller;
	//if (m_sphere)delete[]m_sphere;
}


bool eae6320::Camera::Camera::CleanUp()
{
	bool wereThereErrors = false;
	for (size_t i = 0; i < s_maxCameraNumber; i++)
	{
		if (s_cameras[i])delete s_cameras[i];
		else wereThereErrors = true;
		
	}
	s_cameras._Pop_back_n(s_maxCameraNumber);
	return !wereThereErrors;
}

void eae6320::Camera::Camera::UpdateCameraPosition()
{
	if (m_controller)m_controller->UpdatePosition(m_transform.m_localAxes, m_transform.m_position);
	/*sCurrentCamera->m_sphere[1]->SetPosition(s_currentCamera->position);
	sCurrentCamera->m_sphere[2]->SetPosition(s_currentCamera->position);
	sCurrentCamera->m_sphere[1]->UpdateLine(s_currentCamera->position - Math::cVector::up*100.0f);
	sCurrentCamera->m_sphere[2]->UpdateLine(s_currentCamera->position + localAxes.m_forward*100.0f);*/
}

void eae6320::Camera::Camera::UpdateCameraOrientation()
{
	if (m_controller)m_controller->UpdateOrientation(m_transform.m_orientationEular);
	m_transform.UpdateLocalAxes();
}

void eae6320::Camera::Camera::UpdateMaxCameras()
{
	s_maxCameraNumber = s_cameras.size();
	if (s_maxCameraNumber > 0 && !s_currentCamera)
	{
		s_currentCamera = s_cameras[0];
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		s_currentCamera->ChangeSphereState();
#endif
	}
}

void eae6320::Camera::Camera::ChangeCurrentCamera()
{
	if (UserInput::IsKeyPressedOnce('C'))
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		s_currentCamera->ChangeSphereState();
		//s_currentCamera->m_sphere[0]->SetPosition(s_currentCamera->m_transform.m_position);
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
		s_currentCamera->ChangeSphereState();
#endif
	}
	if (UserInput::IsKeyPressedOnce('V'))
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		s_currentCamera->ChangeSphereState();
		//s_currentCamera->m_sphere[0]->SetPosition(s_currentCamera->m_transform.m_position);
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
		s_currentCamera->ChangeSphereState();
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

void eae6320::Camera::Camera::ChangeSphereState() const
{
	/*m_sphere[0]->SetIsDisplayed(sphereState);
	for (size_t i = 1; i < NUMBER_OF_SPHERES; i++)
	{
		m_sphere[i]->SetIsDisplayed(sphereState);
	}*/
	//sphereState = !sphereState;
}
