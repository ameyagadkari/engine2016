#ifndef EAE6320_FLYCAMERACONTROLLER_H
#define EAE6320_FLYCAMERACONTROLLER_H

#include "../../Game/Gameplay/cbController.h"
#include <cstdint>

namespace eae6320
{
	namespace Camera
	{
		class FlyCameraController final : Gameplay::cbController
		{
		public:
			static const uint32_t classUUID;
			static FlyCameraController* Initialize(){ return new FlyCameraController(); }
		private:
			FlyCameraController() = default;
			void UpdatePosition(const Gameplay::LocalAxes i_localAxes, Math::cVector& o_position) override;
			void UpdateOrientation(Math::cVector& o_eularAngles)const override;
		};
	}
}

#endif // !EAE6320_FLYCAMERACONTROLLER_H