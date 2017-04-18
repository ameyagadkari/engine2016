#include "IUIController.h"

#include "../../Engine/Graphics/ConstantBufferData.h"
#include "../../Engine/Graphics/Material.h"
#include "../../Engine/Graphics/ConstantBuffer.h"

float const eae6320::Debug::UI::IUIController::colorManipulator(0.25f);

eae6320::Debug::UI::IUIController::IUIController(const Color i_color, const bool i_isSelected) :
	m_selectedColor(i_color),
	m_deselectedColor(i_color.r - (i_color.r*colorManipulator), i_color.g - (i_color.g*colorManipulator), i_color.b - (i_color.b*colorManipulator)),
	isSelected(i_isSelected)
{}

void eae6320::Debug::UI::IUIController::GetColor(float & o_r, float & o_g, float & o_b) const
{
	if (isSelected)
	{
		o_r = m_selectedColor.r;
		o_g = m_selectedColor.g;
		o_b = m_selectedColor.b;
	}
	else
	{
		o_r = m_deselectedColor.r;
		o_g = m_deselectedColor.g;
		o_b = m_deselectedColor.b;
	}
}

void eae6320::Debug::UI::IUIController::Draw(const Graphics::Material * const i_material, const float alpha, const bool invertColor)const
{
	if (i_material)
	{
		i_material->BindMaterial();
		Graphics::ConstantBufferData::sMaterial materialBuffer;
		GetColor(materialBuffer.g_color.r, materialBuffer.g_color.g, materialBuffer.g_color.b);
		if (invertColor)
		{
			materialBuffer.g_color.r = 1.0f - materialBuffer.g_color.r;
			materialBuffer.g_color.g = 1.0f - materialBuffer.g_color.g;
			materialBuffer.g_color.b = 1.0f - materialBuffer.g_color.b;
		}
		materialBuffer.g_color.a = alpha;
		i_material->GetMaterialBuffer()->UpdateConstantBuffer(&materialBuffer, sizeof(materialBuffer));
	}
}
