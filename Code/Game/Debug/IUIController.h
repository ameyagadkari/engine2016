#ifndef EAE6320_IUICONTROLLER_H
#define EAE6320_IUICONTROLLER_H

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
				virtual void Draw(const Graphics::Material* const i_material = nullptr, const float alpha = 1.0f, const bool invertColor = false)const;
				virtual void Update() = 0;
				virtual void GetColor(float& o_r, float& o_g, float& o_b)const;
			protected:
				explicit IUIController(const Color i_color, const bool i_isSelected);
				Color m_selectedColor;
				Color m_deselectedColor;
			private:
				virtual void CleanUp() = 0;
				virtual void Initialize() = 0;
				static float const colorManipulator;
			public:
				bool isSelected;
			};
		}
	}
}

#endif // !EAE6320_IUICONTROLLER_H
