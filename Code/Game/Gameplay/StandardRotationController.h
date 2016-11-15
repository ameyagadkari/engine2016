#ifndef EAE6320_STANDARD_ROTATION_CONTROLLER_H
#define EAE6320_STANDARD_ROTATION_CONTROLLER_H

#include "IGameObjectController.h"
#include <cstdint>
namespace eae6320
{
	namespace Gameplay
	{
		class StandardRotationController final : public IGameObjectController
		{
		public:
			static const uint32_t classUUID;
			static StandardRotationController* Initialize();
		private:
			StandardRotationController();
			virtual Math::cVector UpdatePosition()override;
			virtual Math::cVector UpdateOrientation(RotationAxis axis)override;
			float rotationSpeed;
		};
	}
}

#endif // !EAE6320_STANDARD_ROTATION_CONTROLLER_H
