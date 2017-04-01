#include "FlyCameraController.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include "../StringHandler/HashedString.h"
#include "../../Game/Gameplay/LocalAxes.h"

uint32_t const eae6320::Camera::FlyCameraController::classUUID = StringHandler::HashedString("FlyCameraController").GetHash();

void eae6320::Camera::FlyCameraController::UpdatePosition(const Gameplay::LocalAxes i_localAxes, Math::cVector& o_position)
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

	const float speed_unitsPerSecond = 100.0f;
	const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	localOffset *= offsetModifier;
	o_position += localOffset;
}

void eae6320::Camera::FlyCameraController::UpdateOrientation(Math::cVector& o_eularAngles)const
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

	const float speed_unitsPerSecond = 100.0f;
	const float offsetModifier = speed_unitsPerSecond * Time::GetElapsedSecondCount_duringPreviousFrame();
	localOffset *= offsetModifier;
	o_eularAngles += localOffset;
}
