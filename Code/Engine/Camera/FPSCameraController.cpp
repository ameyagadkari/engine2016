#include "FPSCameraController.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include "../StringHandler/HashedString.h"
#include "../../Game/Gameplay/Transform.h"
#include "../Physics/Physics.h"
#include "../Physics/HitData.h"

uint32_t const eae6320::Camera::FPSCameraController::classUUID = StringHandler::HashedString("FPSCameraController").GetHash();

void eae6320::Camera::FPSCameraController::UpdatePosition(Gameplay::Transform& io_transform)
{
	Math::cVector localOffset = Math::cVector::zero;

	if (UserInput::GetKey('W'))
		localOffset += io_transform.m_localAxes.m_forward;
	if (UserInput::GetKey('S'))
		localOffset -= io_transform.m_localAxes.m_forward;
	if (UserInput::GetKey('D'))
		localOffset += io_transform.m_localAxes.m_right;
	if (UserInput::GetKey('A'))
		localOffset -= io_transform.m_localAxes.m_right;

	const float speed_unitsPerSecond = 250.0f;
	const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	localOffset *= offsetModifier;
	localOffset.y = 0.0f;
	Physics::HitData forwardHitData;
	//CheckCollision(o_position + localOffset, i_localAxes, &forwardHitData);
	if (!Physics::isPlayerOnGround)localOffset -= Math::cVector::up*10.0f;
	if (Physics::isPlayerFowardHit)
	{
		float d = -Dot(forwardHitData.normal, forwardHitData.intersectionPoint);
		float dist = Dot(forwardHitData.normal, io_transform.m_position + localOffset) + d;
		Math::cVector currentProjection = (io_transform.m_position + localOffset) - (forwardHitData.normal*dist);
		Math::cVector newForward = (forwardHitData.intersectionPoint - currentProjection);
		localOffset -= io_transform.m_localAxes.m_forward*offsetModifier;
		localOffset += newForward*10.0f;
	}
	Physics::isPlayerOnGround = true;
	Physics::isPlayerFowardHit = false;
	io_transform.m_position += localOffset;
}

void eae6320::Camera::FPSCameraController::UpdateOrientation(Gameplay::Transform& io_transform) const
{
	Math::cVector localOffset = Math::cVector::zero;
	if (UserInput::GetKey('H'))
		localOffset.y += 1.0f;
	if (UserInput::GetKey('F'))
		localOffset.y -= 1.0f;
	if (UserInput::GetKey('G'))
		localOffset.x += 1.0f;
	if (UserInput::GetKey('T'))
		localOffset.x -= 1.0f;

	const float speed_unitsPerSecond = 200.0f;
	const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	localOffset *= offsetModifier;

	io_transform.SetOrientationEular(io_transform.GetOrientationEular() + localOffset);
	Math::cVector currentEularOrientation(io_transform.GetOrientationEular());
	if (currentEularOrientation.x > 89.0f)io_transform.SetOrientationEular(89.0f, currentEularOrientation.y, currentEularOrientation.z);
	if (currentEularOrientation.x < -89.0f)io_transform.SetOrientationEular(-89.0f, currentEularOrientation.y, currentEularOrientation.z);
}
