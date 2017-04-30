#include "TPSPlayerController.h"
#include "FlagController.h"
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
	const float s_maxVelocity2 = s_maxVelocity * 2.0f;
	const float speedBoostDepletionRate = 30.0f;
	const float speedBoostRegenerationRate = speedBoostDepletionRate / 2.0f;
}

uint32_t const eae6320::Gameplay::TPSPlayerController::classUUID(StringHandler::HashedString("TPSPlayerController").GetHash());

float eae6320::Gameplay::TPSPlayerController::CalculateRemainingSpeedBoostProxy(void const * i_thisPointer, const float i_currentValue, const float i_minValue, const float i_maxValue)
{
	return static_cast<TPSPlayerController const*>(i_thisPointer)->CalculateRemainingSpeedBoost(i_currentValue, i_minValue, i_maxValue);
}

float eae6320::Gameplay::TPSPlayerController::CalculateRemainingSpeedBoost(const float i_currentValue, const float i_minValue, const float i_maxValue)const
{
	float offsetModifier;
	if (m_isRunning)
	{
		offsetModifier = speedBoostDepletionRate * Time::GetElapsedSecondCount_duringPreviousFrame();
		return i_currentValue > i_minValue ? i_currentValue - offsetModifier : i_minValue;
	}
	offsetModifier = speedBoostRegenerationRate * Time::GetElapsedSecondCount_duringPreviousFrame();
	return i_currentValue < i_maxValue ? i_currentValue + offsetModifier : i_maxValue;
}

void eae6320::Gameplay::TPSPlayerController::UpdatePosition(Transform& io_transform)
{
	if (!m_cameraTransform)return;
	if(m_remotePlayerTransform)
	{
		float distance = m_remotePlayerTransform->m_position.DistanceBetween(io_transform.m_position);
		if (distance < 50.0f)
		{
			if (m_isCarryingFlag)
			{
				m_isCarryingFlag = false;
				m_flagController->ResetFlagPostion();
			}
		}
	}
	Math::cVector localOffset = Math::cVector::zero;



	if (UserInput::GetKey('W'))
	{
		m_isRunning = UserInput::GetKey(VK_SHIFT) ? true : false;
		localOffset += m_cameraTransform->m_localAxes.m_forward;
	}

	if (UserInput::GetKeyDown('S'))
	{
		io_transform.SetOrientationEular(io_transform.GetOrientationEular() + Math::cVector(0.0f, 180.0f, 0.0f));
		io_transform.UpdateLocalAxes();
	}

	if (UserInput::GetKey('D'))
	{
		m_isRunning = UserInput::GetKey(VK_SHIFT) ? true : false;
		localOffset += m_cameraTransform->m_localAxes.m_right;
	}

	if (UserInput::GetKey('A'))
	{
		m_isRunning = UserInput::GetKey(VK_SHIFT) ? true : false;
		localOffset -= m_cameraTransform->m_localAxes.m_right;
	}

	if (!(UserInput::GetKey('W') || UserInput::GetKey('D') || UserInput::GetKey('A')))
	{
		m_isRunning = false;
	}


	Math::cVector acceleration = localOffset * m_acceleration;
	if (acceleration.GetLength() > s_epsilon)
	{
		m_velocity += acceleration * Time::GetElapsedSecondCount_duringPreviousFrame();
		m_velocity = m_isRunning&&m_sprint->GetValue() > s_epsilon2 ? Math::cVector::ClampMagnitude(m_velocity, s_maxVelocity2) : Math::cVector::ClampMagnitude(m_velocity, s_maxVelocity);
	}
	else
	{
		m_velocity = Math::cVector::zero;
	}

	Math::cVector headPosition(io_transform.m_position + m_velocity * Time::GetElapsedSecondCount_duringPreviousFrame());
	const Math::cVector feetPosition(headPosition - Math::cVector::up*m_height);
	// Ground Check
	{
		Physics::HitData hitData;
		CheckCollision(headPosition, feetPosition, hitData);
		float distance = feetPosition.DistanceBetween(hitData.intersectionPoint);
		if (!Physics::hasIntersected)
		{
			m_velocityDown -= Math::cVector::up*10.0f * Time::GetElapsedSecondCount_duringPreviousFrame();
			m_velocityDown = Math::cVector::ClampMagnitude(m_velocityDown, s_maxVelocity);
		}
		else
		{
			m_velocityDown = Math::cVector::zero;
			if (Math::AlmostEqualUlpsAndAbs(distance, s_epsilon2))
			{
				headPosition.y = io_transform.m_position.y += distance - s_epsilon2*2.0f;
			}
		}
	}

	Math::cVector neckPosition(headPosition - Math::cVector::up*20.0f);
	const Math::cVector bodyPosition(neckPosition + m_velocity.CreateNormalized()*125.0f);
	// World Collision Check
	{
		Physics::HitData hitData;
		CheckCollision(neckPosition, bodyPosition, hitData);
		if (Physics::hasIntersected)
		{
			float d = Dot(m_velocity, -hitData.normal);
			m_velocity -= d*-hitData.normal;
		}
		Physics::hasIntersected = false;
	}

	// Realigning forward to current velocity vector
	{
		Math::cVector tempvelocity(m_velocity.x, 0.0f, m_velocity.z);
		if (tempvelocity.SqrGetLength() > 10.0f)
		{
			float theta = acosf(Dot(tempvelocity.CreateNormalized(), io_transform.m_localAxes.m_forward));
			float sign = Dot(Cross(tempvelocity, io_transform.m_localAxes.m_forward), io_transform.m_localAxes.m_up);
			if (sign < 0.0f)theta = -theta;
			if (theta > 1.0f || theta < -1.0f)
			{
				io_transform.SetOrientationEular(io_transform.GetOrientationEular() + Math::cVector(0.0f, Math::ConvertRadiansToDegrees(theta), 0.0f));
				io_transform.UpdateLocalAxes();
			}
		}
	}
	io_transform.m_position += (m_velocity + m_velocityDown) * Time::GetElapsedSecondCount_duringPreviousFrame();

	if (!m_forward)
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_forward = new Debug::Shapes::DebugObject(true, neckPosition, { 1.0f,0.0f,0.0f });
		m_forward->CreateLine(bodyPosition);
		Debug::Shapes::DebugObject::ms_debugObjects.push_back(m_forward);
#endif
	}
	else
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_forward->SetPosition(neckPosition);
		m_forward->UpdateLine(bodyPosition);
#endif
	}


	if (!m_down)
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_down = new Debug::Shapes::DebugObject(true, headPosition, { 1.0f,0.0f,0.0f });
		m_down->CreateLine(feetPosition);
		Debug::Shapes::DebugObject::ms_debugObjects.push_back(m_down);
#endif
	}
	else
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_down->SetPosition(headPosition);
		m_down->UpdateLine(feetPosition);
#endif
	}
}

void eae6320::Gameplay::TPSPlayerController::UpdateOrientation(Transform& io_transform)
{}