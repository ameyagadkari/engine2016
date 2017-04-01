#include "StandardRotationController.h"

#include"../../Engine/Time/Time.h"
#include "../../Engine/StringHandler/HashedString.h"
#include "LocalAxes.h"

uint32_t const eae6320::Gameplay::StandardRotationController::classUUID(StringHandler::HashedString("StandardRotationController").GetHash());

void eae6320::Gameplay::StandardRotationController::UpdatePosition(const LocalAxes i_localAxes, Math::cVector & o_position) {}

void eae6320::Gameplay::StandardRotationController::UpdateOrientation(Math::cVector & o_eularAngles) const
{
	const float offsetModifier = m_speed * Time::GetElapsedSecondCount_duringPreviousFrame();
	o_eularAngles.y += offsetModifier;
}

