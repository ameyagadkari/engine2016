#include "StandardRotationController.h"

#include"../../Engine/Time/Time.h"
#include "../../Engine/StringHandler/HashedString.h"
#define SPEED 10.0f
namespace eae6320
{
	namespace Gameplay
	{
		uint32_t const StandardRotationController::classUUID = StringHandler::HashedString("StandardRotationController").GetHash();
		StandardRotationController* StandardRotationController::Initialize()
		{
			return new StandardRotationController();
		}
		StandardRotationController::StandardRotationController():
			rotationSpeed(SPEED)
		{
			postionOffset = Math::cVector::zero;
			eularOffset = Math::cVector::zero;
		}
		Math::cVector StandardRotationController::UpdatePosition()
		{
			return Math::cVector::zero;
		}

		Math::cVector StandardRotationController::UpdateOrientation(RotationAxis axis)
		{
			const float offsetModifier = rotationSpeed * Time::GetElapsedSecondCount_duringPreviousFrame();
			switch (axis)
			{
			case RotationAxis::X_AXIS:
				eularOffset.x += offsetModifier;
				break;
			case RotationAxis::Y_AXIS:
				eularOffset.y += offsetModifier;
				break;
			case RotationAxis::Z_AXIS:
				eularOffset.z += offsetModifier;
				break;
			default:
				break;
			}
			return eularOffset;
		}
	}
}