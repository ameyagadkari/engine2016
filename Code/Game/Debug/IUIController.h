#ifndef EAE6320_IUICONTROLLER_H
#define EAE6320_IUICONTROLLER_H

#include "ConfigurationUI.h"

#if defined(EAE6320_DEBUG_UI_AREENABLED)
#include <vector>
#include "Color.h"

namespace eae6320
{
	namespace Graphics
	{
		class Material;
	}
}

namespace eae6320
{
	namespace Debug
	{
		namespace UI
		{
			class IUIController
			{
			public:
				virtual ~IUIController() = default;
				virtual void Draw(const Graphics::Material* const i_material = nullptr)const;
				virtual void Update(std::string i_string = "") = 0;
				virtual void GetColor(float& o_r, float& o_g, float& o_b)const;
				static void UpdateUIElements();
				static void ProcessInput();
			protected:
				explicit IUIController(Color i_color);
				Color m_selectedColor;
				Color m_deselectedColor;
			private:
				virtual void CleanUp() = 0;
				virtual void Initialize() = 0;
				static IUIController* currentSelectedUI;
				static size_t maxUIElements;
				static size_t currentSelectedUINumber;
			public:
				bool isSelected;
			};


			extern const float colorManipulator;
			extern bool isDebugMenuEnabled;
			extern std::vector<IUIController*>debugUIs;
		}
	}
}

#endif // !EAE6320_IUICONTROLLER_H

#endif