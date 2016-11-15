#ifndef EAE6320_GAMEOBJECT_H
#define EAE6320_GAMEOBJECT_H

#include "IGameObjectController.h"

#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Graphics/Effect.h"
#include "../../Engine/Math/cVector.h"
#include "../../Engine/Math/cQuaternion.h"
#include "../../External/Lua/Includes.h"

namespace eae6320
{
	namespace Gameplay
	{		
		class GameObject
		{
		public:
			
			inline virtual ~GameObject();
			static GameObject* LoadGameObject(const char * const relativePath);

			void UpdateGameObjectPosition();
			void UpdateGameObjectOrientation();

#pragma region Gets
			int GetIsStatic()const;
			int GetIsRotating()const;
			RotationAxis GetRotationAxis()const;
			Graphics::Effect* GetEffect()const;
			Graphics::Mesh* GetMesh()const;
			Math::cVector GetPosition()const;
			Math::cVector GetOrientationEular()const;
			Math::cQuaternion GetOrientation() const;
#pragma endregion

#pragma region Sets
			void SetIsStatic(const int isStatic);
			void SetIsRotating(const int isRotating);
			void SetRotationAxis(const RotationAxis rotationAxis);
			void SetEffect(Graphics::Effect* const effect);
			void SetMesh(Graphics::Mesh* const mesh);
			void SetPosition(const Math::cVector position);
			void SetOrientationEular(const Math::cVector eularAngles);
#pragma endregion
			
		private:
			inline GameObject();

			Graphics::Mesh*mesh = NULL;
			Graphics::Effect*effect = NULL;
			IGameObjectController* controller = NULL;
			Math::cVector position;
			Math::cQuaternion orientation;
			Math::cVector eularAngles;

			Math::cVector initialPositionOffset;
			Math::cVector initialEularOffset;

			uint8_t isStatic;
			uint8_t isRotating;
			RotationAxis rotationAxis;
		};
	}
}

#endif // !EAE6320_GAMEOBJECT_H
