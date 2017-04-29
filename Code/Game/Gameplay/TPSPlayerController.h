#ifndef EAE6320_TPS_PLAYER_CONTROLLER_H
#define EAE6320_TPS_PLAYER_CONTROLLER_H

#include "cbController.h"
#include <cstdint>
#include "../../Engine/Math/cVector.h"
#include "../Debug/Slider.h"

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
			static TPSPlayerController* Initialize(const float i_acceleration = 0.0f, const float i_height = 0.0f)
			{
				return new TPSPlayerController(i_acceleration, i_height);
			}
			void SetCameraTransform(Transform const * const i_cameraTransform)
			{
				m_cameraTransform = i_cameraTransform;
			}
		private:
			explicit TPSPlayerController(const float i_acceleration, const float i_height)
				:
				m_velocity(Math::cVector::zero),
				m_velocityDown(Math::cVector::zero),
				m_cameraTransform(nullptr),
				m_forward(nullptr),
				m_down(nullptr),
				m_sprint(new Debug::UI::Slider({ -400,200 }, "Speed Boost: ", { 0.0f,1.0f,1.0f }, 0.0f, 100.0f, 200.0f, this, &CalculateRemainingSpeedBoostProxy, true)),
				m_acceleration(i_acceleration),
				m_height(i_height),
				m_isRunning(false)
			{
				if (m_sprint)Debug::UI::HUD.push_back(m_sprint);
			}
			static float CalculateRemainingSpeedBoostProxy(void const * i_thisPointer, const float i_currentValue, const float i_minValue, const float i_maxValue);
			float CalculateRemainingSpeedBoost(const float i_currentValue, const float i_minValue, const float i_maxValue)const;
			void UpdatePosition(Transform& io_transform) override;
			void UpdateOrientation(Transform& io_transform) override;
			Math::cVector m_velocity;
			Math::cVector m_velocityDown;
			Transform const * m_cameraTransform;
			Debug::Shapes::DebugObject* m_forward;
			Debug::Shapes::DebugObject* m_down;
			Debug::UI::Slider* m_sprint;
			const float m_acceleration;
			const float m_height;
			bool m_isRunning;
		};
	}
}

#endif // !EAE6320_TPS_PLAYER_CONTROLLER_H