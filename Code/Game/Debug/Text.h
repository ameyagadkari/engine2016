#ifndef EAE6320_TEXT_H
#define EAE6320_TEXT_H

#include "ConfigurationUI.h"

#if defined(EAE6320_DEBUG_UI_AREENABLED)
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
			class Text final :public IUIController
			{
			public:
				explicit Text(const PixelCoordinates i_pixelCoordinates = { 0,0 }, const std::string i_text = "", const Color i_color = { 1.0f,1.0f,1.0f });
				~Text();
				int16_t GetRightTextPixelCoordinate()const;
			private:
				void Update(std::string i_string = "") override;
				void Draw(const Graphics::Material* const i_material = nullptr)const override;
				void Initialize() override;
				void CleanUp()override;
				std::string m_text;
				size_t m_numberOfCharacters;
				Graphics::Sprite::ScreenPosition* m_screenPositionForEachCharacter;
				PixelCoordinates m_pixelCoordinates;
				Graphics::MeshData* m_meshData;
				int16_t m_rightTextPixelCoordinate;
			};
		}
	}
}

#endif // !EAE6320_TEXT_H

#endif