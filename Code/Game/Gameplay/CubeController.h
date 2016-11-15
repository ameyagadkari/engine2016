#ifndef EAE6320_CUBECONTROLLER_H
#define EAE6320_CUBECONTROLLER_H

#include "IGameObjectController.h"
#include <cstdint>
namespace eae6320
{
	namespace Gameplay
	{
		class CubeController final : private IGameObjectController
		{	
		public:
			static const uint32_t classTypeHash;
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