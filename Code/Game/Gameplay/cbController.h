#ifndef EAE6320_CBCONTROLLER_H
#define EAE6320_CBCONTROLLER_H

namespace eae6320
{
	namespace Math
	{
		class cVector;
	}
}

namespace eae6320
{
	namespace Gameplay
	{
		struct LocalAxes;
	}
}

namespace eae6320
{
	namespace Gameplay
	{
		class cbController
		{
		public:
			virtual void UpdatePosition(const LocalAxes i_localAxes, Math::cVector& o_position) = 0;
			virtual void UpdateOrientation(Math::cVector& o_eularAngles)const = 0;
			virtual ~cbController() = default;
		};
	}
}

#endif // !EAE6320_CBCONTROLLER_H