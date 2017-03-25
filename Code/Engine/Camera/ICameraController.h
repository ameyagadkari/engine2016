#ifndef EAE6320_ICAMERACONTROLLER_H
#define EAE6320_ICAMERACONTROLLER_H

#include "LocalAxes.h"
namespace eae6320
{
	namespace Camera
	{
		class ICameraController
		{
		public:
			virtual void UpdateCameraPosition(const LocalAxes i_localAxes, Math::cVector& o_position)const = 0;
			virtual void UpdateCameraOrientation(Math::cVector& o_eularAngles)const = 0;
			virtual ~ICameraController() = default;
		};
	}
}

#endif // !EAE6320_ICAMERACONTROLLER_H