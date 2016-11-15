#include "SnakeController.h"

#include"../../Engine/Time/Time.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/StringHandler/HashedString.h"
#define START_SPEED 10.0f
namespace eae6320
{
	namespace Gameplay
	{
		uint32_t const SnakeController::classTypeHash = StringHandler::HashedString("SnakeController").GetHash();
		SnakeController* SnakeController::Initialize()
		{
			return new SnakeController();
		}
		SnakeController::SnakeController()
		{
			postionOffset = Math::cVector::zero;
			eularOffset = Math::cVector::zero;
			speedOfSnake = START_SPEED;
			sign = 1;
			direction = Direction::forward;
		}
		Math::cVector SnakeController::UpdatePosition()
		{
			if (UserInput::IsKeyPressedOnce(VK_UP))
			{
				{
					sign = -1;
					direction = Direction::forward;
				}
			}
			if (UserInput::IsKeyPressedOnce(VK_DOWN))
			{
				{
					sign = 1;
					direction = Direction::backward;
				}
			}
			if (UserInput::IsKeyPressedOnce(VK_LEFT))
			{
				{
					sign = -1;
					direction = Direction::leftward;
				}
			}
			if (UserInput::IsKeyPressedOnce(VK_RIGHT))
			{

				{
					sign = 1;
					direction = Direction::rightward;
				}
			}
			const float offsetModifier = speedOfSnake * Time::GetElapsedSecondCount_duringPreviousFrame();
			switch (direction)
			{
			case Direction::forward:
				postionOffset.z += sign*offsetModifier;
				break;
			case Direction::backward:
				postionOffset.z += sign*offsetModifier;
				break;
			case Direction::leftward:
				postionOffset.x += sign*offsetModifier;
				break;
			case Direction::rightward:
				postionOffset.x += sign*offsetModifier;
				break;
			default:
				break;
			}
			return postionOffset;
		}

		Math::cVector SnakeController::UpdateOrientation(RotationAxis axis)
		{
			return Math::cVector::zero;
		}
	}
}