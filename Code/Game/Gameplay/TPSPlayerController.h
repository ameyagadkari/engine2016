#ifndef EAE6320_TPS_PLAYER_CONTROLLER_H
#define EAE6320_TPS_PLAYER_CONTROLLER_H

#include "cbController.h"
#include <cstdint>
#include "../../Engine/Math/cVector.h"

namespace eae6320
{
	namespace Debug
	{
		namespace Shapes
		{
			class DebugObject;
		}
	}
}

namespace eae6320
{
	namespace Gameplay
	{
		class TPSPlayerController final : cbController
		{
		public:
			static const uint32_t classUUID;
			static TPSPlayerController* Initialize(const float i_acceleration = 0.0f, const float i_height = 0.0f) { return new TPSPlayerController(i_acceleration, i_height); }
		private:
			explicit TPSPlayerController(const float i_acceleration, const float i_height)
				:
				m_velocity(Math::cVector::zero),
				m_velocity2(Math::cVector::zero),
				m_forward(nullptr),
				m_down(nullptr),
				m_acceleration(i_acceleration),
				m_height(i_height)
			{
			}

			void UpdatePosition(Transform& io_transform) override;
			void UpdateOrientation(Transform& io_transform) override;
			Math::cVector m_velocity;
			Math::cVector m_velocity2;
			Debug::Shapes::DebugObject* m_forward;
			Debug::Shapes::DebugObject* m_down;
			const float m_acceleration;
			const float m_height;
		};
	}
}

#endif // !EAE6320_TPS_PLAYER_CONTROLLER_H