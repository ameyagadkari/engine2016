#include "DefaultController.h"

#include "../../Engine/StringHandler/HashedString.h"
#include "LocalAxes.h"

uint32_t const eae6320::Gameplay::DefaultController::classUUID(StringHandler::HashedString("DefaultController").GetHash());

void eae6320::Gameplay::DefaultController::UpdatePosition(Transform& io_transformn) {}

void eae6320::Gameplay::DefaultController::UpdateOrientation(Transform& io_transform){}

