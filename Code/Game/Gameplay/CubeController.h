#ifndef EAE6320_CUBECONTROLLER_H
#define EAE6320_CUBECONTROLLER_H

#include "IGameObjectController.h"
namespace eae6320
{
	namespace Gameplay
	{
		class CubeController final : private IGameObjectController
		{	
		public:
			static const char * const classType;
			static CubeController* Initialize();
		private:
			CubeController();
			virtual Math::cVector UpdatePosition()override;
			virtual Math::cVector UpdateOrientation(RotationAxis axis)override;

			float rotationSpeedOfCube;
			RotationAxis rotationAxis;
			char sign;
			Direction direction;
		};
	}
}



#endif // !EAE6320_CUBECONTROLLER_H