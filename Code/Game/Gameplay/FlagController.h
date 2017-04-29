#ifndef EAE6320_FLAG_CONTROLLER_H
#define EAE6320_FLAG_CONTROLLER_H

#include "cbController.h"
#include <cstdint>

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
		private:
			explicit FlagController()
				:
				m_playerTransform(nullptr),
				m_flagNotCarrying(true)
			{}
			void UpdatePosition(Transform& io_transform) override;
			void UpdateOrientation(Transform& io_transform) override;
			Transform const * m_playerTransform;
			bool m_flagNotCarrying;
		};
	}
}

#endif // !EAE6320_FLAG_CONTROLLER_H
