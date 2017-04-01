#include "TPSCameraController.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include "../StringHandler/HashedString.h"
#include "../../Game/Gameplay/LocalAxes.h"
#include "../../Game/Gameplay/Transform.h"

uint32_t const eae6320::Camera::TPSCameraController::classUUID = StringHandler::HashedString("TPSCameraController").GetHash();



void eae6320::Camera::TPSCameraController::UpdatePosition(Gameplay::Transform& io_transform)
{
	if(!m_playerTransform)return;
	Math::cVector desiredPosition =
		m_playerTransform->m_position -
		m_playerTransform->m_localAxes.m_forward*500.0f;

	if (desiredPosition.DistanceBetween(io_transform.m_position) > 10.0f)
	{
		Math::cVector steering = (desiredPosition - io_transform.m_position).CreateNormalized();
		Math::cVector acceleration = steering * m_acceleration;

		m_velocity += acceleration * Time::GetElapsedSecondCount_duringPreviousFrame();
		float distance = io_transform.m_position.DistanceBetween(desiredPosition);
		if (distance < 25.0f)
		{
			m_velocity = m_velocity.ClampMagnitude(250.0f * (distance / 25.0f));
		}
		else
		{
			m_velocity = m_velocity.ClampMagnitude(250.0f);
		}
		io_transform.m_position += m_velocity * Time::GetElapsedSecondCount_duringPreviousFrame();
	}
	//if (UserInput::IsKeyPressed('W'))
	//	localOffset += i_localAxes.m_forward;
	//if (UserInput::IsKeyPressed('S'))
	//	localOffset -= i_localAxes.m_forward;
	//if (UserInput::IsKeyPressed('D'))
	//	localOffset += i_localAxes.m_right;
	//if (UserInput::IsKeyPressed('A'))
	//	localOffset -= i_localAxes.m_right;

	//const float speed_unitsPerSecond = 250.0f;
	//const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	//localOffset *= offsetModifier;
	//localOffset.y = 0.0f;
	//Physics::HitData forwardHitData;
	////CheckCollision(o_position + localOffset, i_localAxes, &forwardHitData);
	//if (!Physics::isPlayerOnGround)localOffset -= Math::cVector::up*10.0f;
	//if (Physics::isPlayerFowardHit)
	//{	
	//	float d = -Dot(forwardHitData.normal, forwardHitData.intersectionPoint);
	//	float dist = Dot(forwardHitData.normal, o_position + localOffset) + d;
	//	Math::cVector currentProjection = (o_position + localOffset) - (forwardHitData.normal*dist);
	//	Math::cVector newForward = (forwardHitData.intersectionPoint - currentProjection);
	//	localOffset -= i_localAxes.m_forward*offsetModifier;
	//	localOffset += newForward*10.0f;
	//}
	//Physics::isPlayerOnGround = true;
	//Physics::isPlayerFowardHit = false;
	//o_position += localOffset;
}

void eae6320::Camera::TPSCameraController::UpdateOrientation(Gameplay::Transform& io_transform) const
{
	if (!m_playerTransform)return;
	io_transform.m_orientationEular = m_playerTransform->m_orientationEular;
	//Math::cVector localOffset = Math::cVector::zero;
	//if (UserInput::IsKeyPressed('H'))
	//	localOffset.y += 1.0f;
	//if (UserInput::IsKeyPressed('F'))
	//	localOffset.y -= 1.0f;
	//if (UserInput::IsKeyPressed('G'))
	//	localOffset.x += 1.0f;
	//if (UserInput::IsKeyPressed('T'))
	//	localOffset.x -= 1.0f;

	//const float speed_unitsPerSecond = 200.0f;
	//const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	//localOffset *= offsetModifier;

	//o_eularAngles += localOffset;
	//if (o_eularAngles.x > 89.0f)o_eularAngles.x = 89.0f;		
	//if (o_eularAngles.x < -89.0f)o_eularAngles.x = -89.0f;		
}
