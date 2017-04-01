#ifndef EAE6320_CBCONTROLLER_H
#define EAE6320_CBCONTROLLER_H

namespace eae6320
{
	namespace Gameplay
	{
		struct Transform;
	}
}

namespace eae6320
{
	namespace Gameplay
	{
		class cbController
		{
		public:
			virtual void UpdatePosition(Transform& io_transform) = 0;
			virtual void UpdateOrientation(Transform& io_transform)const = 0;
			virtual ~cbController() = default;
		};
	}
}

#endif // !EAE6320_CBCONTROLLER_H