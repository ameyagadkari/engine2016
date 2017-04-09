#include "TPSCameraController.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include "../StringHandler/HashedString.h"
#include "../../Game/Gameplay/LocalAxes.h"
#include "../../Game/Gameplay/Transform.h"

namespace
{
#ifdef _DEBUG
	const float rotateSpeed = 1000.0f;
#else
	const float rotateSpeed = 10000.0f;
#endif
	const float damping = 2.5f;
	float oldEularY;
	eae6320::Math::cVector offset;
	bool notDone = true;
}

uint32_t const eae6320::Camera::TPSCameraController::classUUID = StringHandler::HashedString("TPSCameraController").GetHash();

void eae6320::Camera::TPSCameraController::UpdatePosition(Gameplay::Transform& io_transform)
{
	if (!m_playerTransform)return;
	if (notDone)
	{
		Math::cVector desiredStart = -m_playerTransform->m_localAxes.m_forward*500.0f + m_playerTransform->m_localAxes.m_up*50.0f;
		offset = m_playerTransform->m_position - desiredStart;
		notDone = false;
	}
	float currentAngle = io_transform.GetOrientationEular().y;
	if (UserInput::GetKeyDown('Q') || UserInput::GetKeyDown('E'))
	{
		oldEularY = io_transform.GetOrientationEular().y;
	}
	else if (UserInput::GetKeyUp('Q') || UserInput::GetKeyUp('E'))
	{
		io_transform.SetOrientationEular(0.0f, oldEularY, 0.0f);
	}
	else if (UserInput::GetKey('Q') || UserInput::GetKey('E'))
	{
		float desiredAngle = 0.0f;
		if (UserInput::GetKey('Q'))
		{
			desiredAngle = io_transform.GetOrientationEular().y + rotateSpeed * Time::GetElapsedSecondCount_duringPreviousFrame();
		}
		else if (UserInput::GetKey('E'))
		{
			desiredAngle = io_transform.GetOrientationEular().y - rotateSpeed * Time::GetElapsedSecondCount_duringPreviousFrame();
		}
		float angle = Math::LerpDegrees(-currentAngle, -desiredAngle, Time::GetElapsedSecondCount_duringPreviousFrame() * damping);
		Math::cQuaternion rotation = Math::cQuaternion(0, Math::cVector::right)*
			Math::cQuaternion(Math::ConvertDegreesToRadians(angle), Math::cVector::up)*
			Math::cQuaternion(0, Math::cVector::forward);
		io_transform.m_position = m_playerTransform->m_position - (rotation * offset);
	}
	else
	{
		float desiredAngle = m_playerTransform->GetOrientationEular().y;
		float angle = Math::LerpDegrees(-currentAngle, -desiredAngle, Time::GetElapsedSecondCount_duringPreviousFrame() * damping);
		Math::cQuaternion rotation = Math::cQuaternion(0, Math::cVector::right)*
			Math::cQuaternion(Math::ConvertDegreesToRadians(angle), Math::cVector::up)*
			Math::cQuaternion(0, Math::cVector::forward);
		io_transform.m_position = m_playerTransform->m_position - (rotation * offset);
	}
	Math::cQuaternion neworientationQuaternion = Math::cQuaternion::LookRotation(io_transform.m_position,m_playerTransform->m_position);
	io_transform.SetOrientationEular(neworientationQuaternion.ToEular());
	io_transform.UpdateLocalAxes();
}

void eae6320::Camera::TPSCameraController::UpdateOrientation(Gameplay::Transform& io_transform)
{
}
