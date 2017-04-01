#ifndef EAE6320_TPSCAMERACONTROLLER_H
#define EAE6320_TPSCAMERACONTROLLER_H

#include "../../Game/Gameplay/cbController.h"
#include <cstdint>
#include "../Math/cVector.h"

namespace eae6320
{
	namespace Gameplay
	{
		struct Transform;
	}
}

namespace eae6320
{
	namespace Camera
	{
		class TPSCameraController final : Gameplay::cbController
		{
		public:
			static const uint32_t classUUID;
			static TPSCameraController* Initialize(const float i_acceleration = 0.0f)
			{
				return new TPSCameraController(i_acceleration);
			}
			void SetPlayerTransform(Gameplay::Transform* i_playerTransform)
			{
				m_playerTransform = i_playerTransform;
			}
		private:
			explicit TPSCameraController(const float i_acceleration) 
			: 
			m_playerTransform(nullptr), 
			m_acceleration(i_acceleration)
			{}

			void UpdatePosition(Gameplay::Transform& io_transform) override;
			void UpdateOrientation(Gameplay::Transform& io_transform)const override;
			Gameplay::Transform* m_playerTransform;
			Math::cVector m_velocity;
			const float m_acceleration;
		};
	}
}
#endif // !EAE6320_TPSCAMERACONTROLLER_H
