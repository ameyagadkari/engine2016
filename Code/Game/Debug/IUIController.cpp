#include "IUIController.h"
#include "../../Engine/UserInput/UserInput.h"
#include "Text.h"

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