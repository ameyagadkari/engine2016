#include "FPSCameraController.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include "../StringHandler/HashedString.h"
#include "../Physics/Physics.h"
#include "../Physics/HitData.h"

uint32_t const eae6320::Camera::FPSCameraController::classUUID = StringHandler::HashedString("FPSCameraController").GetHash();

eae6320::Camera::FPSCameraController* eae6320::Camera::FPSCameraController::Initialize()
{
	return new FPSCameraController();
}

void eae6320::Camera::FPSCameraController::UpdateCameraPosition(const LocalAxes i_localAxes, Math::cVector& o_position)const
{
	Math::cVector localOffset = Math::cVector::zero;

	if (UserInput::IsKeyPressed('W'))
		localOffset += i_localAxes.m_forward;
	if (UserInput::IsKeyPressed('S'))
		localOffset -= i_localAxes.m_forward;
	if (UserInput::IsKeyPressed('D'))
		localOffset += i_localAxes.m_right;
	if (UserInput::IsKeyPressed('A'))
		localOffset -= i_localAxes.m_right;

	const float speed_unitsPerSecond = 250.0f;
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
	o_position += localOffset;
}

void eae6320::Camera::FPSCameraController::UpdateCameraOrientation(Math::cVector& o_eularAngles)const
{
	Math::cVector localOffset = Math::cVector::zero;
	if (UserInput::IsKeyPressed('H'))
		localOffset.y += 1.0f;
	if (UserInput::IsKeyPressed('F'))
		localOffset.y -= 1.0f;
	if (UserInput::IsKeyPressed('G'))
		localOffset.x += 1.0f;
	if (UserInput::IsKeyPressed('T'))
		localOffset.x -= 1.0f;

	const float speed_unitsPerSecond = 200.0f;
	const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	localOffset *= offsetModifier;

	o_eularAngles += localOffset;
	if (o_eularAngles.x > 89.0f)o_eularAngles.x = 89.0f;		
	if (o_eularAngles.x < -89.0f)o_eularAngles.x = -89.0f;		
}
