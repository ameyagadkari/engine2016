#ifndef EAE6320_PLAYER_CONTROLLER_H
#define EAE6320_PLAYER_CONTROLLER_H

#include "cbController.h"
#include <cstdint>
#include "../../Engine/Math/cVector.h"

namespace eae6320
{
	namespace Gameplay
	{
		class PlayerController final : cbController
		{
		public:
			static const uint32_t classUUID;
			static PlayerController* Initialize(const float i_acceleration = 0.0f) { return new PlayerController(i_acceleration); }
		private:
			explicit PlayerController(const float i_acceleration)
				:
				m_velocity(),
				m_acceleration(i_acceleration) {}
			void UpdatePosition(const LocalAxes i_localAxes, Math::cVector& o_position) override;
			void UpdateOrientation(Math::cVector& o_eularAngles)const override;
			Math::cVector m_velocity;
			const float m_acceleration;
		};
	}
}

#endif // !EAE6320_PLAYER_CONTROLLER_H