#ifndef EAE6320_LOCALAXES_H
#define EAE6320_LOCALAXES_H
#include "../Math/cVector.h"

namespace eae6320
{
	namespace Camera
	{
		struct LocalAxes
		{
			Math::cVector front;
			Math::cVector up;
			Math::cVector right;
		};
	}
}

#endif // !EAE6320_LOCALAXES_H
