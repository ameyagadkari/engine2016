#include "StandardRotationController.h"

#include"../../Engine/Time/Time.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "Transform.h"

uint32_t const eae6320::Gameplay::StandardRotationController::classUUID(StringHandler::HashedString("StandardRotationController").GetHash());

void eae6320::Gameplay::StandardRotationController::UpdatePosition(Transform& io_transform) {}

void eae6320::Gameplay::StandardRotationController::UpdateOrientation(Transform& io_transform)
{
	const float offsetModifier = m_speed * Time::GetElapsedSecondCount_duringPreviousFrame();
	io_transform.SetOrientationEular(io_transform.GetOrientationEular() + Math::cVector(0.0f, offsetModifier, 0.0f));
	io_transform.UpdateLocalAxes();
}

