#include "IUIController.h"

#if defined(EAE6320_DEBUG_UI_AREENABLED)
#include "../../Engine/UserInput/UserInput.h"
#include "Text.h"
#include "../../Engine/Graphics/ConstantBufferData.h"
#include "../../Engine/Graphics/Material.h"
#include "../../Engine/Graphics/ConstantBuffer.h"


namespace eae6320
{
	namespace Debug
	{
		namespace UI
		{
			bool isDebugMenuEnabled = false;
			const float colorManipulator = 0.25f;
			std::vector<IUIController*>debugUIs;
		}
	}
}

eae6320::Debug::UI::IUIController* eae6320::Debug::UI::IUIController::currentSelectedUI = nullptr;
size_t  eae6320::Debug::UI::IUIController::maxUIElements = 0;
size_t  eae6320::Debug::UI::IUIController::currentSelectedUINumber = 0;

eae6320::Debug::UI::IUIController::IUIController(Color i_color) :
	isSelected(false),
	m_selectedColor(i_color),
	m_deselectedColor(i_color.r - (i_color.r*colorManipulator), i_color.g - (i_color.g*colorManipulator), i_color.b - (i_color.b*colorManipulator))
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

void eae6320::Debug::UI::IUIController::ProcessInput()
{
	if (UserInput::IsKeyPressedOnce(VK_OEM_3))
	{
		isDebugMenuEnabled = !isDebugMenuEnabled;
	}
	if (isDebugMenuEnabled)
	{
		if (UserInput::IsKeyPressedOnce(VK_DOWN))
		{
			currentSelectedUI->isSelected = false;
			if (currentSelectedUINumber == maxUIElements - 1)
			{
				currentSelectedUINumber = 0;
			}
			else
			{
				currentSelectedUINumber++;
			}
			currentSelectedUI = debugUIs[currentSelectedUINumber];
			currentSelectedUI->isSelected = true;
		}
		if (UserInput::IsKeyPressedOnce(VK_UP))
		{
			currentSelectedUI->isSelected = false;
			if (currentSelectedUINumber == 0)
			{
				currentSelectedUINumber = maxUIElements - 1;
			}
			else
			{
				currentSelectedUINumber--;
			}
			currentSelectedUI = debugUIs[currentSelectedUINumber];
			currentSelectedUI->isSelected = true;
		}
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

void eae6320::Debug::UI::IUIController::UpdateUIElements()
{
	maxUIElements = debugUIs.size();
	if (maxUIElements > 0 && !currentSelectedUI)
	{
		currentSelectedUI = debugUIs[0];
		currentSelectedUI->isSelected = true;
		currentSelectedUINumber = 0;
	}
}

#endif