#ifndef EAE6320_GAMEOBJECT_H
#define EAE6320_GAMEOBJECT_H

#include "../Graphics/Mesh.h"

namespace eae6320
{
	namespace Gameplay
	{
		class GameObject
		{
		public:
			Graphics::Mesh*mesh;
			Graphics::ConstantBufferData::sDrawCall drawCallBufferData;
			inline virtual ~GameObject();
			static GameObject* Initilaize(const char* relativePath);
			void SetNewOffset(const float x, const float y);
			void SetNewInitialOffset(const float x, const float y);
		private:
			inline GameObject();
			float initialOffset[2];
			float offset[2];
		};
	}
}

#endif // !EAE6320_GAMEOBJECT_H
