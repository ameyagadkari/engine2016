#ifndef EAE6320_GAMEOBJECT_H
#define EAE6320_GAMEOBJECT_H

#include "../../Engine/Graphics/Mesh.h"
#include "../../Engine/Math/cVector.h"
#include "../../Engine/Math/cQuaternion.h"

namespace eae6320
{
	namespace Gameplay
	{
		enum struct RotationAxis { X_AXIS, Y_AXIS, Z_AXIS };
		class GameObject
		{
		public:
			
			inline virtual ~GameObject();
			
			static GameObject* Initilaize(const char* relativePath);
			
			void SetNewInitialPositionOffset(const Math::cVector startPosition);
			void SetNewInitialEularOffset(const Math::cVector startOrientaion);
			void UpdateGameObjectPosition();
			void UpdateGameObjectOrientation();

#pragma region Gets
			bool GetIsStatic()const;
			bool GetIsRotating()const;
			RotationAxis GetRotationAxis()const;
			Graphics::Mesh* GetMesh()const;
			Math::cVector GetPosition()const;
			Math::cVector GetOrientationEular()const;
			Math::cQuaternion GetOrientation() const;
#pragma endregion

#pragma region Sets
			void SetIsStatic(const bool isStatic);
			void SetIsRotating(const bool isRotating);
			void SetRotationAxis(const RotationAxis rotationAxis);
			void SetMesh(Graphics::Mesh* const mesh);
			void SetPosition(const Math::cVector position);
			void SetOrientationEular(const Math::cVector eularAngles);
#pragma endregion
			
		private:
			inline GameObject();

			Graphics::Mesh*mesh;

			Math::cVector position;
			Math::cVector initialPositionOffset;
			Math::cVector postionOffset;

			Math::cQuaternion orientation;
			Math::cVector eularAngles;
			Math::cVector initialEularOffset;
			Math::cVector eularOffset;

			bool isStatic;
			bool isRotating;
			RotationAxis rotationAxis;
		};
	}
}

#endif // !EAE6320_GAMEOBJECT_H
