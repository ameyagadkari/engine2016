#ifndef EAE6320_GAMEOBJECT_H
#define EAE6320_GAMEOBJECT_H

#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Math/cVector.h"
#include "../../Engine/Math/cQuaternion.h"

namespace eae6320
{
	namespace Gameplay
	{
		class GameObject
		{
		public:
			
			inline virtual ~GameObject();
			
			static GameObject* Initilaize(const char* relativePath);
			
			void SetNewInitialOffset(const float x, const float y);
			void UpdateGameObjectPosition();

#pragma region Gets
			bool GetIsStatic()const;
			Graphics::Mesh* GetMesh()const;
			Graphics::ConstantBufferData::sDrawCall GetDrawCallBufferData()const;
			Math::cVector GetPosition()const;
			Math::cQuaternion GetOrientation()const;
#pragma endregion

#pragma region Sets
			void SetIsStatic(const bool isStatic);
			void SetMesh(Graphics::Mesh* const mesh);
			void SetDrawCallBufferData(const Graphics::ConstantBufferData::sDrawCall drawCallBufferData);
			void SetPosition(const Math::cVector position);
			void SetOrientation(const Math::cQuaternion orientation);
#pragma endregion
			
		private:
			inline GameObject();

			Graphics::Mesh*mesh;
			Graphics::ConstantBufferData::sDrawCall drawCallBufferData;

			Math::cVector position;
			Math::cQuaternion orientation;

			float initialOffset[3];
			float offset[3];
			bool isStatic;
		};
	}
}

#endif // !EAE6320_GAMEOBJECT_H
