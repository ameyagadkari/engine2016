#ifndef EAE6320_FPSCAMERACONTROLLER_H
#define EAE6320_FPSCAMERACONTROLLER_H

#include "../../Game/Gameplay/cbController.h"
#include <cstdint>

namespace eae6320
{
	namespace Camera
	{
		class FPSCameraController final : Gameplay::cbController
		{
		public:
			static const uint32_t classUUID;
			static FPSCameraController* Initialize() { return new FPSCameraController(); }
		private:
			FPSCameraController() = default;
			void UpdatePosition(Gameplay::Transform& io_transform) override;
			void UpdateOrientation(Gameplay::Transform& io_transform)const override;
		};
	}
}
#endif // !EAE6320_FPSCAMERACONTROLLER_H
