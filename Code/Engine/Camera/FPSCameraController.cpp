#include "FPSCameraController.h"
#include "../UserInput/UserInput.h"
#include "../Time/Time.h"
#include "../StringHandler/HashedString.h"
#include "../../Game/Gameplay/Transform.h"

uint32_t const eae6320::Camera::FPSCameraController::classUUID = StringHandler::HashedString("FPSCameraController").GetHash();

void eae6320::Camera::FPSCameraController::UpdatePosition(Gameplay::Transform& io_transform)
{
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
