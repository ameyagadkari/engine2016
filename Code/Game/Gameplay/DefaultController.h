#ifndef EAE6320_DEFAULT_CONTROLLER_H
#define EAE6320_DEFAULT_CONTROLLER_H

#include "cbController.h"
#include <cstdint>
namespace eae6320
{
	namespace Gameplay
	{
		class DefaultController final : cbController
		{
		public:
			static const uint32_t classUUID;
			static DefaultController* Initialize() { return new DefaultController(); }
		private:
			explicit DefaultController() {}

			void UpdatePosition(Transform& io_transform) override;
			void UpdateOrientation(Transform& io_transform)const override;
		};
	}
}

#endif // !EAE6320_DEFAULT_CONTROLLER_H
