#ifndef EAE6320_TEXT_H
#define EAE6320_TEXT_H

#include <string>
#include "Color.h"
#include "PixelCoordinates.h"
#include "IUIController.h"

namespace eae6320
{
	namespace Graphics
	{
		namespace Sprite
		{
			struct ScreenPosition;
		}
		struct MeshData;
	}
}

namespace eae6320
{
	namespace Debug
	{
		namespace UI
		{
			class Text :public IUIController
			{
			public:
				explicit Text(const PixelCoordinates i_pixelCoordinates = { 0,0 }, const std::string i_text = "", const Color i_color = { 1.0f,1.0f,1.0f });
				~Text();			
			private:
				void Update(std::string i_string) override;
				void Draw() override;
				void Initialize() override;
				void CleanUp()override;
				void GetColor(float& i_r, float& i_g, float& i_b)const override;
				std::string m_text;
				Color m_color;
				size_t m_numberOfCharacters;
				Graphics::Sprite::ScreenPosition* m_screenPositionForEachCharacter;
				PixelCoordinates m_pixelCoordinates;
				Graphics::MeshData* m_meshData;
			};
		}
	}
}

#endif // !EAE6320_TEXT_H