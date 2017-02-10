#ifndef EAE6320_CHECKBOX_H
#define EAE6320_CHECKBOX_H

#include "ConfigurationUI.h"

#if defined(EAE6320_DEBUG_UI_AREENABLED)
#include "PixelCoordinates.h"
#include "IUIController.h"

namespace eae6320
{
	namespace Gameplay
	{
		class GameObject2D;
	}
}

namespace eae6320
{
	namespace Debug
	{
		namespace UI
		{
			class Text;
		}
	}
}

namespace eae6320
{
	namespace Debug
	{
		namespace UI
		{
			class Checkbox final :public IUIController
			{
			public:
				explicit Checkbox(const PixelCoordinates i_pixelCoordinates = { 0,0 }, const std::string i_onText = "", const std::string i_offText = "", const Color i_color = { 1.0f,1.0f,1.0f }, const bool i_isOn = true);
				~Checkbox();
				bool GetIsEnabled()const;
			private:
				void Update(std::string i_string = "") override;
				void Draw(const Graphics::Material* const i_material = nullptr)const override;
				void Initialize() override;
				void CleanUp()override;
				PixelCoordinates m_pixelCoordinates;
				Text* m_onText;
				Text* m_offText;
				Gameplay::GameObject2D* m_unchecked;
				Gameplay::GameObject2D* m_checked;
				bool m_isOn;
			};
		}
	}
}

#endif // !EAE6320_CHECKBOX_H

#endif
