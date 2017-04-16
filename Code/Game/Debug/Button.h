#ifndef EAE6320_BUTTON_H
#define EAE6320_BUTTON_H

#include "Color.h"
#include "PixelCoordinates.h"
#include "IUIController.h"
#include <string>

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
			class Button final :public IUIController
			{
			public:
				explicit Button(const PixelCoordinates i_pixelCoordinates = { 0,0 }, const std::string i_buttonName = "", const Color i_color = { 1.0f,1.0f,1.0f }, const float i_backgroundTransperancy = 0.25f);
				~Button();
				void SetState(const bool i_state);
				bool GetState()const;
			private:
				void RecalculateBackgroundWidth() const;
				void Update() override;
				void Draw(const Graphics::Material* const i_material = nullptr, const float alpha = 1.0f, const bool invertColor = false)const override;
				void Initialize() override;
				void CleanUp()override;
				Gameplay::GameObject2D* m_background;
				Text* m_buttonName;
				float m_backgroundTransperancy;
				PixelCoordinates m_pixelCoordinates;
				bool m_pressed;
			};
		}
	}
}

#endif // !EAE6320_BUTTON_H
