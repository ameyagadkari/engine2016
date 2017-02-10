#ifndef EAE6320_IGAMEOBJECTCONTROLLER_H
#define EAE6320_IGAMEOBJECTCONTROLLER_H

#include "../../Engine/Math/cVector.h"

namespace eae6320
{
	namespace Gameplay
	{
		enum struct RotationAxis { X_AXIS, Y_AXIS, Z_AXIS };
		enum struct Direction { forward, backward, leftward, rightward };
		class IGameObjectController
		{
		public:
			virtual Math::cVector UpdatePosition() = 0;
			virtual Math::cVector UpdateOrientation(RotationAxis axis) = 0;
			virtual ~IGameObjectController() = default;
		protected:
			Math::cVector eularOffset;
			Math::cVector postionOffset;
		};
	}
}

#endif // !EAE6320_IGAMEOBJECTCONTROLLER_H