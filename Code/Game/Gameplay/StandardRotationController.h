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
			Math::cVector UpdatePosition()override;
			Math::cVector UpdateOrientation(RotationAxis axis)override;
			float rotationSpeed;
		};
	}
}

#endif // !EAE6320_STANDARD_ROTATION_CONTROLLER_H
