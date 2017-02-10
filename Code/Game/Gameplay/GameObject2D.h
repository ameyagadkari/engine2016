#ifndef EAE6320_GAMEOBJECT2D_H
#define EAE6320_GAMEOBJECT2D_H

#include <cstdint>
#include "../../Engine/Graphics/SpriteData.h"

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

// Helper Structs
//================
namespace eae6320
{
	namespace Gameplay
	{
		enum struct Anchor { TOP_LEFT, BOTTOM_LEFT, TOP_RIGHT, BOTTOM_RIGHT, CENTER, LEFT_CENTER, BOTTOM_CENTER, TOP_CENTER, RIGHT_CENTER };
		struct RectTransform
		{
			struct
			{
				int16_t x, y;
			}pixelCoordinates;
			uint16_t width, height;
		};

		Graphics::Sprite::ScreenPosition ConvertPixelCoordinatesUsingAnchorToScreenCoordinates(const RectTransform i_rectTransform, const Anchor i_anchor);
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
			static GameObject2D* LoadGameObject2D(const char * const relativePath, const bool doNotUsePixelCoordinatesFromFile = false, const int16_t x = 0, const int16_t y = 0);

#pragma region Gets
			Graphics::Material* GetMaterial()const;
			Graphics::cSprite* GetSprite()const;
			RectTransform GetRectTransform()const;
			Anchor GetAnchor()const;
#pragma endregion

#pragma region Sets
			void SetMaterial(Graphics::Material* const effect);
			void SetSprite(Graphics::cSprite* const sprite);
			void SetRectTransform(const RectTransform rectTransform);
			void SetAnchor(const Anchor anchor);
#pragma endregion

		private:
			GameObject2D();
			RectTransform rectTransform;
			Graphics::cSprite* sprite;
			Graphics::Material* material;
			Anchor anchor;
		};
	}
}

#endif // !EAE6320_GAMEOBJECT2D_H