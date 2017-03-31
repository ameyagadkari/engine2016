#ifndef EAE6320_HITDATA_H
#define EAE6320_HITDATA_H

#include "../Math/cVector.h"
namespace eae6320
{
	namespace Physics
	{
		struct HitData
		{
			Math::cVector normal;
			Math::cVector intersectionPoint;
		};
	}
}
#endif // !EAE6320_HITDATA_H
