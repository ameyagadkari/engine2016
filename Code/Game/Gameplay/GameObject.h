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
#pragma endregion

#pragma region Sets
			/*void SetMaterial(Graphics::Material* const effect);
			void SetMesh(Graphics::Mesh* const mesh);*/
#pragma endregion
			
		private:
			GameObject();
			Transform m_transform;

			Graphics::Mesh* m_mesh;
			Graphics::Material* m_material;
			cbController* m_controller;

			/*Math::cVector initialPositionOffset;
			Math::cVector initialEularOffset;*/
		};
	}
}

#endif // !EAE6320_GAMEOBJECT_H
