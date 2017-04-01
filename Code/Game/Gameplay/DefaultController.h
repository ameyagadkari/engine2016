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
			void UpdatePosition(const LocalAxes i_localAxes, Math::cVector& o_position) override;
			void UpdateOrientation(Math::cVector& o_eularAngles)const override;
		};
	}
}

#endif // !EAE6320_DEFAULT_CONTROLLER_H
