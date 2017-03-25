#ifndef EAE6320_FLYCAMERACONTROLLER_H
#define EAE6320_FLYCAMERACONTROLLER_H

#include "ICameraController.h"
#include "../Math/cVector.h"
#include <cstdint>

namespace eae6320
{
	namespace Camera
	{
		class FlyCameraController final : public ICameraController
		{
		public:
			static const uint32_t classUUID;
			static FlyCameraController* Initialize();
		private:
			FlyCameraController() = default;
			void UpdateCameraPosition(const LocalAxes i_localAxes, Math::cVector& o_position)const override;
			void UpdateCameraOrientation(Math::cVector& o_eularAngles)const override;
		};
	}
}

#endif // !EAE6320_FLYCAMERACONTROLLER_H