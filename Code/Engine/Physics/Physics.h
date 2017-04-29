#ifndef EAE6320_PHYSICS_H
#define EAE6320_PHYSICS_H
#include "../Math/cVector.h"

namespace eae6320
{
	namespace Graphics
	{
		struct MeshData;
	}
}

namespace eae6320
{
	namespace Physics
	{
		struct HitData;
		struct Triangle;
	}
}

namespace eae6320
{
	namespace Physics
	{
		bool Initialize();
		bool CleanUp();
		void CheckCollision(const Math::cVector i_start, const Math::cVector i_end, HitData& o_hitData);

		extern bool hasIntersected;
	}
}

#endif // !EAE6320_PHYSICS_H
