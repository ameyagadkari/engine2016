#include "FPSCameraController.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include "../StringHandler/HashedString.h"
#include "../../Game/Gameplay/Transform.h"

namespace
{
	const float s_epsilon = 1.0e-10f;
	const float s_maxVelocity = 250.0f;
}

uint32_t const eae6320::Camera::FPSCameraController::classUUID = StringHandler::HashedString("FPSCameraController").GetHash();

void eae6320::Camera::FPSCameraController::UpdatePosition(Gameplay::Transform& io_transform)
{
}

void eae6320::Camera::FPSCameraController::UpdateOrientation(Gameplay::Transform& io_transform)
{
	if (!m_playerTransform)return;
	Math::cVector localOffset = Math::cVector::zero;
	if (UserInput::GetKey('T'))
		localOffset.x += 1.0f;
	if (UserInput::GetKey('G'))
		localOffset.x -= 1.0f;
	if (UserInput::GetKey('F'))
		localOffset.y -= 1.0f;
	if (UserInput::GetKey('H'))
		localOffset.y += 1.0f;

	Math::cVector acceleration = localOffset * m_acceleration;
	if (acceleration.GetLength() > s_epsilon)
	{
		m_velocity += acceleration * Time::GetElapsedSecondCount_duringPreviousFrame();
		m_velocity = Math::cVector::ClampMagnitude(m_velocity, s_maxVelocity);
	}
	else
	{
		m_velocity = Math::cVector::zero;
	}
	Math::cVector tempRotation = io_transform.GetOrientationEular() + m_velocity * Time::GetElapsedSecondCount_duringPreviousFrame();
	if (tempRotation.x > 89.0f)
		tempRotation.x = 89.0f;
	if (tempRotation.x < -89.0f)
		tempRotation.x = -89.0f;
	io_transform.SetOrientationEular(tempRotation);
	m_playerTransform->SetOrientationEular(0.0f, io_transform.GetOrientationEular().y, 0.0f);
	io_transform.UpdateLocalAxes();
	m_playerTransform->UpdateLocalAxes();
	/*const float speed_unitsPerSecond = 200.0f;
	const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	localOffset *= offsetModifier;

	io_transform.SetOrientationEular(io_transform.GetOrientationEular() + localOffset);
	Math::cVector currentEularOrientation(io_transform.GetOrientationEular());
	if (currentEularOrientation.x > 89.0f)io_transform.SetOrientationEular(89.0f, currentEularOrientation.y, currentEularOrientation.z);
	if (currentEularOrientation.x < -89.0f)io_transform.SetOrientationEular(-89.0f, currentEularOrientation.y, currentEularOrientation.z);*/
}
