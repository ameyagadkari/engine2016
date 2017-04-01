#include "DefaultController.h"

#include "../../Engine/StringHandler/HashedString.h"
#include "LocalAxes.h"

uint32_t const eae6320::Gameplay::DefaultController::classUUID(StringHandler::HashedString("DefaultController").GetHash());

void eae6320::Gameplay::DefaultController::UpdatePosition(const LocalAxes i_localAxes, Math::cVector & o_position) {}

void eae6320::Gameplay::DefaultController::UpdateOrientation(Math::cVector & o_eularAngles) const{}

