#ifndef EAE6320_FPSCAMERACONTROLLER_H
#define EAE6320_FPSCAMERACONTROLLER_H

#include "ICameraController.h"
#include "../Math/cVector.h"
#include <cstdint>

namespace eae6320
{
	namespace Camera
	{
		class FPSCameraController final : public ICameraController
		{
		public:
			static const uint32_t classUUID;
			static FPSCameraController* Initialize();
		private:
			FPSCameraController() = default;
			void UpdateCameraPosition(const LocalAxes i_localAxes, Math::cVector& o_position)const override;
			void UpdateCameraOrientation(Math::cVector& o_eularAngles)const override;
		};
	}
}
#endif
