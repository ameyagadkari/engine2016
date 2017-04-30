#ifndef EAE6320_FLAG_CONTROLLER_H
#define EAE6320_FLAG_CONTROLLER_H

#include "cbController.h"
#include <cstdint>
#include "../../Engine/Math/cVector.h"
namespace eae6320
{
	namespace Gameplay
	{

		class TPSPlayerController;
	}
}

namespace eae6320
{
	namespace Gameplay
	{

		class FlagController final : cbController
		{
		public:
			static const uint32_t classUUID;
			static FlagController* Initialize()
			{
				return new FlagController();
			}
			void SetPlayerTransform(Transform const * const  i_playerTransform)
			{
				m_playerTransform = i_playerTransform;
			}
			void SetTPSPlayerController(TPSPlayerController * const  i_tpsPlayerController)
			{
				m_tpsPlayerController = i_tpsPlayerController;
			}
			void ResetFlagPostion() { m_flagNotCarrying = true; m_resetFlagPosition = true; }
		private:
			explicit FlagController()
				:
				m_playerTransform(nullptr), 
				m_tpsPlayerController(nullptr),
				m_flagNotCarrying(true),
				m_resetFlagPosition(false)
			{
			}

			void UpdatePosition(Transform& io_transform) override;
			void UpdateOrientation(Transform& io_transform) override;
			Math::cVector m_resetPosition;
			Transform const * m_playerTransform;
			TPSPlayerController * m_tpsPlayerController;
			bool m_flagNotCarrying;
			bool m_resetFlagPosition;
		};
	}
}

#endif // !EAE6320_FLAG_CONTROLLER_H
