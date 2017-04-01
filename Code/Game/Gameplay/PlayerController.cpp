#include "PlayerController.h"

#include"../../Engine/Time/Time.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "LocalAxes.h"
#include "../../Engine/UserInput/UserInput.h"
//#include "../../Engine/Physics/HitData.h"
//#include "../../Engine/Physics/Physics.h"

namespace
{
	const float s_epsilon = 1.0e-10f;
}

uint32_t const eae6320::Gameplay::PlayerController::classUUID(StringHandler::HashedString("PlayerController").GetHash());

void eae6320::Gameplay::PlayerController::UpdatePosition(const LocalAxes i_localAxes, Math::cVector & o_position)
{
	Math::cVector localOffset = Math::cVector::zero;

	if (UserInput::IsKeyPressed('I'))
		localOffset += i_localAxes.m_forward;
	if (UserInput::IsKeyPressed('K'))
		localOffset -= i_localAxes.m_forward;
	if (UserInput::IsKeyPressed('L'))
		localOffset += i_localAxes.m_right;
	if (UserInput::IsKeyPressed('J'))
		localOffset -= i_localAxes.m_right;

	//Math::cVector steering = localOffset.ClampMagnitude(1.0f);
	Math::cVector acceleration = localOffset * m_acceleration;

	if (acceleration.GetLength() > s_epsilon)
	{
		m_velocity += acceleration * Time::GetElapsedSecondCount_duringPreviousFrame();
		m_velocity = m_velocity.ClampMagnitude(250.0f);
		o_position += m_velocity * Time::GetElapsedSecondCount_duringPreviousFrame();
	}
	else
	{
		m_velocity = Math::cVector::zero;
	}
	/*const float speed_unitsPerSecond = 250.0f;
	const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	localOffset *= offsetModifier;
	localOffset.y = 0.0f;
	Physics::HitData forwardHitData;
	CheckCollision(o_position + localOffset, i_localAxes, &forwardHitData);
	if (!Physics::isPlayerOnGround)localOffset -= Math::cVector::up*10.0f;
	if (Physics::isPlayerFowardHit)
	{
		float d = -Dot(forwardHitData.normal, forwardHitData.intersectionPoint);
		float dist = Dot(forwardHitData.normal, o_position + localOffset) + d;
		Math::cVector currentProjection = (o_position + localOffset) - (forwardHitData.normal*dist);
		Math::cVector newForward = (forwardHitData.intersectionPoint - currentProjection);
		localOffset -= i_localAxes.m_forward*offsetModifier;
		localOffset += newForward*10.0f;
	}
	Physics::isPlayerOnGround = true;
	Physics::isPlayerFowardHit = false;
	o_position += localOffset;*/
}

void eae6320::Gameplay::PlayerController::UpdateOrientation(Math::cVector & o_eularAngles) const
{
	/*const float offsetModifier = m_speed * Time::GetElapsedSecondCount_duringPreviousFrame();
	o_eularAngles.y += offsetModifier;*/
}