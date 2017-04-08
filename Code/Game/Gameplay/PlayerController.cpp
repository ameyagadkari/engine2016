#include "PlayerController.h"

#include"../../Engine/Time/Time.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "LocalAxes.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../Debug/DebugObject.h"
#include "../../Engine/Physics/HitData.h"
#include "../../Engine/Physics/Physics.h"
#include "Transform.h"

namespace
{
	const float s_epsilon = 1.0e-10f;
	const float s_epsilon2 = 1.0e-4f;
	const float s_maxVelocity = 250.0f;
}

uint32_t const eae6320::Gameplay::PlayerController::classUUID(StringHandler::HashedString("PlayerController").GetHash());

void eae6320::Gameplay::PlayerController::UpdatePosition(Transform& io_transform)
{
	Math::cVector localOffset = Math::cVector::zero;

	if (UserInput::GetKey('W'))
		localOffset += io_transform.m_localAxes.m_forward;
	//if (UserInput::IsKeyPressed('S'))
	//	localOffset -= io_transform.m_localAxes.m_forward;
	if (UserInput::GetKey('D'))
		localOffset += io_transform.m_localAxes.m_right;
	if (UserInput::GetKey('A'))
		localOffset -= io_transform.m_localAxes.m_right;

	Math::cVector acceleration = localOffset * m_acceleration;
	Math::cVector tempPosition;
	if (acceleration.GetLength() > s_epsilon)
	{
		m_velocity += acceleration * Time::GetElapsedSecondCount_duringPreviousFrame();
		m_velocity = m_velocity.ClampMagnitude(s_maxVelocity);
	}
	else
	{
		m_velocity = Math::cVector::zero;
	}
	tempPosition = io_transform.m_position + m_velocity * Time::GetElapsedSecondCount_duringPreviousFrame();

	Physics::HitData forwardHitData;
	Physics::HitData downHitData;
	CheckCollision(tempPosition, io_transform.m_localAxes, m_height, &forwardHitData, &downHitData);
	Math::cVector feetPosition = tempPosition - Math::cVector::up*m_height;
	float distance = feetPosition.DistanceBetween(downHitData.intersectionPoint);
	if (!Physics::isPlayerOnGround)
	{
		m_velocity2 -= Math::cVector::up*10.0f * Time::GetElapsedSecondCount_duringPreviousFrame();
		m_velocity2 = m_velocity2.ClampMagnitude(s_maxVelocity);
	}
	else
	{
		m_velocity2 = Math::cVector::zero;
		if (Math::AlmostEqualUlpsAndAbs(distance, s_epsilon2))
		{
			io_transform.m_position.y += distance - s_epsilon2*2.0f;
		}
	}
	io_transform.m_position += (m_velocity + m_velocity2) * Time::GetElapsedSecondCount_duringPreviousFrame();

	if (!m_forward)
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_forward = new Debug::Shapes::DebugObject(true, io_transform.m_position, { 1.0f,0.0f,0.0f });
		m_forward->CreateLine(io_transform.m_position + io_transform.m_localAxes.m_forward*100.0f);
		Debug::Shapes::DebugObject::ms_debugObjects.push_back(m_forward);
#endif
	}
	else
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_forward->SetPosition(io_transform.m_position);
		m_forward->UpdateLine(io_transform.m_position + io_transform.m_localAxes.m_forward*100.0f);
#endif
	}


	if (!m_down)
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_down = new Debug::Shapes::DebugObject(true, io_transform.m_position, { 1.0f,0.0f,0.0f });
		m_down->CreateLine(io_transform.m_position - Math::cVector::up*m_height);
		Debug::Shapes::DebugObject::ms_debugObjects.push_back(m_down);
#endif
	}
	else
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_down->SetPosition(tempPosition);
		m_down->UpdateLine(tempPosition - Math::cVector::up*m_height);
#endif
	}
	/*if (Physics::isPlayerFowardHit)
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

void eae6320::Gameplay::PlayerController::UpdateOrientation(Transform& io_transform) const
{
	Math::cVector localOffset = Math::cVector::zero;
	if (UserInput::GetKey('D'))
		localOffset.y += 1.0f;
	if (UserInput::GetKey('A'))
		localOffset.y -= 1.0f;
	if (UserInput::GetKeyDown('S'))
	{
		//io_transform.m_localAxes.m_forward = -io_transform.m_localAxes.m_forward;
		io_transform.SetOrientationEular(io_transform.GetOrientationEular() + Math::cVector(0.0f, 180.0f, 0.0f));
		io_transform.UpdateLocalAxes(/*false, io_transform.m_localAxes.m_forward*/);
		return;
	}

	const float speed_unitsPerSecond = 200.0f;
	const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	localOffset *= offsetModifier;

	io_transform.SetOrientationEular(io_transform.GetOrientationEular() + localOffset);
	io_transform.UpdateLocalAxes();
}