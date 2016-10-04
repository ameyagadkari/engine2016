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
			
			void SetNewInitialOffset(const float x, const float y, const float z);
			void UpdateGameObjectPosition();
			void UpdateGameObjectOrientation();

#pragma region Gets
			bool GetIsStatic()const;
			bool GetIsRotating()const;
			RotationAxis GetRotationAxis()const;
			Graphics::Mesh* GetMesh()const;
			Graphics::ConstantBufferData::sDrawCall GetDrawCallBufferData()const;
			Math::cVector GetPosition()const;
			Math::cQuaternion GetOrientation()const;
#pragma endregion

#pragma region Sets
			void SetIsStatic(const bool isStatic);
			void SetIsRotating(const bool isRotating);
			void SetRotationAxis(const RotationAxis rotationAxis);
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
			float eularOrientationOffsetsDegrees[3];
			bool isStatic;
			bool isRotating;
			RotationAxis rotationAxis;
		};
	}
}

#endif // !EAE6320_GAMEOBJECT_H
