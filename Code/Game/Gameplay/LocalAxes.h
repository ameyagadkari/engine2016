#ifndef EAE6320_LOCALAXES_H
#define EAE6320_LOCALAXES_H
#include "../../Engine/Math/cVector.h"

namespace eae6320
{
	namespace Gameplay
	{
		struct LocalAxes
		{
			Math::cVector m_forward;
			Math::cVector m_up;
			Math::cVector m_right;
			LocalAxes():m_forward(Math::cVector::forward),m_up(Math::cVector::up),m_right(Math::cVector::right){}
		};
	}
}

#endif // !EAE6320_LOCALAXES_H
