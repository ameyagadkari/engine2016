#include "CubeController.h"

#include"../../Engine/Time/Time.h"
#include "../../Engine/UserInput/UserInput.h"

#define START_SPEED 100.0f
namespace eae6320
{
	namespace Gameplay
	{
		const char * const CubeController::classType = "newcube";
		CubeController* CubeController::Initialize()
		{
			return new CubeController();
		}
		CubeController::CubeController()
		{
			postionOffset = Math::cVector::zero;
			eularOffset = Math::cVector::zero;
			rotationSpeedOfCube = START_SPEED;	
			rotationAxis = RotationAxis::X_AXIS;
			sign = 1;
			direction = Direction::forward;
		}
		Math::cVector CubeController::UpdatePosition()
		{
			return Math::cVector::zero;
		}

		Math::cVector CubeController::UpdateOrientation(RotationAxis axis)
		{			
			if (UserInput::IsKeyPressedOnce('W'))// W key rotating forward on x
			{
				if (direction != Direction::backward)
				{
					rotationAxis = RotationAxis::X_AXIS;
					sign = 1;
					direction = Direction::forward;
				}
			}
			if (UserInput::IsKeyPressedOnce('S'))// S key rotating backward on x
			{
				if (direction != Direction::forward)
				{
					rotationAxis = RotationAxis::X_AXIS;
					sign = -1;
					direction = Direction::backward;
				}
			}
			if (UserInput::IsKeyPressedOnce('A'))// A key rotating rightward on y
			{
				if (direction != Direction::leftward)
				{
					rotationAxis = RotationAxis::Y_AXIS;
					sign = -1;
					direction = Direction::rightward;
				}
			}
			if (UserInput::IsKeyPressedOnce('D'))// D key rotating leftward on y
			{
				if (direction != Direction::rightward)
				{
					rotationAxis = RotationAxis::Y_AXIS;
					sign = 1;
					direction = Direction::leftward;
				}
			}
			const float offsetModifier = rotationSpeedOfCube * Time::GetElapsedSecondCount_duringPreviousFrame();
			switch (rotationAxis)
			{
			case RotationAxis::X_AXIS:
				eularOffset.x += sign*offsetModifier;
				break;
			case RotationAxis::Y_AXIS:
				eularOffset.y += sign*offsetModifier;
				break;
			default:
				break;
			}
			return eularOffset;
		}
	}
}