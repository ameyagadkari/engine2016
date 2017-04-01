#ifndef EAE6320_PHYSICS_H
#define EAE6320_PHYSICS_H
#include "../Math/cVector.h"
#include "../../Game/Gameplay/LocalAxes.h"


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
	}
}

namespace eae6320
{
	namespace Physics
	{
		bool Initialize();
		bool CleanUp();
		void CheckCollision(const Math::cVector i_newPosition, const Gameplay::LocalAxes i_localAxes, const float i_playerHeight, HitData* o_forwardHitData, HitData* o_downHitData);

		extern bool isPlayerOnGround;
		extern bool isPlayerFowardHit;
		extern Graphics::MeshData* collisionData;
	}
}

#endif // !EAE6320_PHYSICS_H
