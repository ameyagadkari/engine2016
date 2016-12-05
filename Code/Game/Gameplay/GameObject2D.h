#ifndef EAE6320_GAMEOBJECT2D_H
#define EAE6320_GAMEOBJECT2D_H

// Forward Declarations
//=====================
namespace eae6320
{
	namespace Graphics
	{
		class Material;
		class cSprite;
	}
}

namespace eae6320
{
	namespace Gameplay
	{
		class GameObject2D
		{
		public:
			
			~GameObject2D();
			static GameObject2D* LoadGameObject2D(const char * const relativePath);

#pragma region Gets
			Graphics::Material* GetMaterial()const;
			Graphics::cSprite* GetSprite()const;
#pragma endregion

#pragma region Sets
			void SetMaterial(Graphics::Material* const effect);
			void SetSprite(Graphics::cSprite* const sprite);
#pragma endregion

		private:
			GameObject2D();
			Graphics::cSprite* sprite;
			Graphics::Material* material;
		};
	}
}

#endif // !EAE6320_GAMEOBJECT2D_H