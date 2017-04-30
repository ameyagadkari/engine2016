#ifndef EAE6320_SCOREZONE_CONTROLLER_H
#define EAE6320_SCOREZONE_CONTROLLER_H

#include "cbController.h"
#include <cstdint>
#include "FlagController.h"

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
		class ScoreZoneController final : cbController
		{
		public:
			static const uint32_t classUUID;
			static ScoreZoneController* Initialize() { return new ScoreZoneController(); }
			void SetPlayerTransform(Transform const * const i_playerTransform)
			{
				m_playerTransform = i_playerTransform;
			}
			void SetTPSPlayerController(TPSPlayerController * const  i_tpsPlayerController)
			{
				m_tpsPlayerController = i_tpsPlayerController;
			}
			void SetFlagController(FlagController * const  i_flagController)
			{
				m_flagController = i_flagController;
			}
		private:
			explicit ScoreZoneController():
				m_score(0),
				m_playerTransform(nullptr),
				m_tpsPlayerController(nullptr), 
				m_flagController(nullptr)
			{
			}

			void UpdatePosition(Transform& io_transform) override;
			void UpdateOrientation(Transform& io_transform) override;
			size_t m_score;
			Transform const * m_playerTransform;
			TPSPlayerController * m_tpsPlayerController;
			FlagController* m_flagController;
		};
	}
}

#endif //  // !EAE6320_SCOREZONE_CONTROLLER_H
