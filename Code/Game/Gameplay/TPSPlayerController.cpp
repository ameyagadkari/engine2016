#include "TPSPlayerController.h"

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

uint32_t const eae6320::Gameplay::TPSPlayerController::classUUID(StringHandler::HashedString("TPSPlayerController").GetHash());

void eae6320::Gameplay::TPSPlayerController::UpdatePosition(Transform& io_transform)
{
	if (!m_cameraTransform)return;
	Math::cVector localOffset = Math::cVector::zero;

	if (UserInput::GetKey('W'))
		localOffset += m_cameraTransform->m_localAxes.m_forward;
	if (UserInput::GetKeyDown('S'))
	{
		io_transform.SetOrientationEular(io_transform.GetOrientationEular() + Math::cVector(0.0f, 180.0f, 0.0f));
		io_transform.UpdateLocalAxes();
	}
	if (UserInput::GetKey('D'))
		localOffset += m_cameraTransform->m_localAxes.m_right;
	if (UserInput::GetKey('A'))
		localOffset -= m_cameraTransform->m_localAxes.m_right;

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
	Math::cVector tempPosition = io_transform.m_position + m_velocity * Time::GetElapsedSecondCount_duringPreviousFrame();

	Physics::HitData hitData;
	CheckCollision(tempPosition, m_velocity.CreateNormalized(), m_height, &hitData, true);
	Math::cVector feetPosition = tempPosition - Math::cVector::up*(m_height);
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
			tempPosition.y = io_transform.m_position.y += distance - s_epsilon2*2.0f;
		}
	}

	CheckCollision(tempPosition, m_velocity.CreateNormalized(), m_height, &hitData, false);

	if (Physics::hasIntersected)
	{
		float d = Dot(m_velocity, -hitData.normal);
		m_velocity -= d*-hitData.normal;
	}
	Physics::hasIntersected = false;

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
	io_transform.m_position += (m_velocity + m_velocityDown) * Time::GetElapsedSecondCount_duringPreviousFrame();

	if (!m_forward)
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_forward = new Debug::Shapes::DebugObject(true, io_transform.m_position, { 1.0f,0.0f,0.0f });
		m_forward->CreateLine(io_transform.m_position + m_velocity.CreateNormalized()*125.0f);
		Debug::Shapes::DebugObject::ms_debugObjects.push_back(m_forward);
#endif
	}
	else
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		m_forward->SetPosition(io_transform.m_position);
		m_forward->UpdateLine(io_transform.m_position + m_velocity.CreateNormalized()*125.0f);
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
}

void eae6320::Gameplay::TPSPlayerController::UpdateOrientation(Transform& io_transform)
{}