#ifndef EAE6320_SNAKECONTROLLER_H
#define EAE6320_SNAKECONTROLLER_H

#include "IGameObjectController.h"
namespace eae6320
{
	namespace Gameplay
	{
		class SnakeController final : public IGameObjectController
		{
		public:
			static const char * const classType;
			static SnakeController* Initialize();
		private:
			SnakeController();
			virtual Math::cVector UpdatePosition()override;
			virtual Math::cVector UpdateOrientation(RotationAxis axis)override;

			float speedOfSnake;
			char sign;
			Direction direction;
		};
	}
}

#endif // !EAE6320_SNAKECONTROLLER_H
