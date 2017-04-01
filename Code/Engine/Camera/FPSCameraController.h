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
			void UpdatePosition(const Gameplay::LocalAxes i_localAxes, Math::cVector& o_position) override;
			void UpdateOrientation(Math::cVector& o_eularAngles)const override;
		};
	}
}
#endif
