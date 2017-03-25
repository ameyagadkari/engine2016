#include "FlyCameraController.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include "../StringHandler/HashedString.h"

uint32_t const eae6320::Camera::FlyCameraController::classUUID = StringHandler::HashedString("FlyCameraController").GetHash();

eae6320::Camera::FlyCameraController* eae6320::Camera::FlyCameraController::Initialize()
{
	return new FlyCameraController();
}

void eae6320::Camera::FlyCameraController::UpdateCameraPosition(const LocalAxes i_localAxes, Math::cVector& o_position)const
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
	o_position += localOffset;
}

void eae6320::Camera::FlyCameraController::UpdateCameraOrientation(Math::cVector& o_eularAngles)const
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
}
