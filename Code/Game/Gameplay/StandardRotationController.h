#ifndef EAE6320_STANDARD_ROTATION_CONTROLLER_H
#define EAE6320_STANDARD_ROTATION_CONTROLLER_H

#include "cbController.h"
#include <cstdint>
namespace eae6320
{
	namespace Gameplay
	{
		class StandardRotationController final : cbController
		{
		public:
			static const uint32_t classUUID;
			static StandardRotationController* Initialize(const float i_speed = 0.0f) { return new StandardRotationController(i_speed); }
		private:
			explicit StandardRotationController(const float i_speed) : m_speed(i_speed) {}
			void UpdatePosition(Transform& io_transform) override;
			void UpdateOrientation(Transform& io_transform)const override;
			const float m_speed;
		};
	}
}

#endif // !EAE6320_STANDARD_ROTATION_CONTROLLER_H
