#ifndef EAE6320_GAMEOBJECT_H
#define EAE6320_GAMEOBJECT_H

#include "cbController.h"
#include "Transform.h"

// Forward Declarations
//=====================
namespace eae6320
{
	namespace Graphics
	{
		class Material;
		class Mesh;
	}
}

namespace eae6320
{
	namespace Gameplay
	{
		class cbController;
	}
}

namespace eae6320
{
	namespace Gameplay
	{		
		class GameObject
		{
		public:			
			
			~GameObject();
			static GameObject* LoadGameObject(const char * const relativePath);

			void UpdateGameObjectPosition();
			void UpdateGameObjectOrientation();

#pragma region Gets
			Graphics::Material* GetMaterial()const;
			Graphics::Mesh* GetMesh()const;
			Transform GetTransform()const;		
			Transform& GetTransformAddress();
			cbController* GameObject::GetController()const;
#pragma endregion

#pragma region Sets
			void SetTransformSpecial(const Math::cVector i_position, const Math::cVector i_orientationEular);
#pragma endregion
			
		private:
			GameObject();
			Transform m_transform;

			Graphics::Mesh* m_mesh;
			Graphics::Material* m_material;
		public:
			cbController* m_controller;
			uint32_t m_controllerUUID;
		};
	}
}

#endif // !EAE6320_GAMEOBJECT_H
