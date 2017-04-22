#include "ConsoleMenu.h"
#include "ConfigurationConsoleMenu.h"
#include "IUIController.h"
#include "Text.h"
#include "Checkbox.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Network/NetworkScene.h"

#include <vector>


namespace
{
	std::vector<eae6320::Debug::UI::IUIController*> menuItems;
	bool isConsoleMenuEnabled = false;
	eae6320::Debug::UI::IUIController* currentSelectedMenuItem = nullptr;
	size_t totalMenuItems = 0;
	size_t currentSelectedMenuItemNumber = 0;

	void ProcessInput();
	void UpdateConsoleMenuItems();
}

namespace
{
	void ProcessInput()
	{
		if (eae6320::UserInput::GetKeyDown(VK_OEM_3))
			isConsoleMenuEnabled = !isConsoleMenuEnabled;
		if (!isConsoleMenuEnabled)return;
		if (eae6320::UserInput::GetKeyDown(VK_DOWN))
		{
			currentSelectedMenuItem->isSelected = false;

			if (currentSelectedMenuItemNumber == totalMenuItems - 1)currentSelectedMenuItemNumber = 0;
			else currentSelectedMenuItemNumber++;

			currentSelectedMenuItem = menuItems[currentSelectedMenuItemNumber];
			currentSelectedMenuItem->isSelected = true;
		}
		if (eae6320::UserInput::GetKeyDown(VK_UP))
		{
			currentSelectedMenuItem->isSelected = false;

			if (currentSelectedMenuItemNumber == 0)currentSelectedMenuItemNumber = totalMenuItems - 1;
			else currentSelectedMenuItemNumber--;

			currentSelectedMenuItem = menuItems[currentSelectedMenuItemNumber];
			currentSelectedMenuItem->isSelected = true;
		}
	}

	void UpdateConsoleMenuItems()
	{
		totalMenuItems = menuItems.size();
		if (totalMenuItems > 0 && !currentSelectedMenuItem)
		{
			currentSelectedMenuItem = menuItems[0];
			currentSelectedMenuItem->isSelected = true;
			currentSelectedMenuItemNumber = 0;
		}
	}
}

void eae6320::Debug::ConsoleMenu::Initialize()
{
#if defined(EAE6320_CONSOLE_MENU_ISENABLED)
	menuItems.push_back(new UI::Text({ -380,350 }, "FPS: ", { 1.0f,0.55f,0.0f }, &Time::SetFPS));
	menuItems.push_back(new UI::Checkbox({ -380,300 }, "Switch Debug Shapes Off", "Switch Debug Shapes On", { 1.0f,0.55f,0.0f }, true, &Shapes::DebugObject::Draw));
	menuItems.push_back(new UI::Text({ -380,250 }, "Network Type: ", { 1.0f,0.55f,0.0f }, &Network::NetworkScene::SetNetworkType));
	UpdateConsoleMenuItems();
#endif
}

void eae6320::Debug::ConsoleMenu::Update()
{
#if defined(EAE6320_CONSOLE_MENU_ISENABLED)	
	ProcessInput();
	for (size_t i = 0; i < totalMenuItems; i++)menuItems[i]->Update();
#endif
}

void eae6320::Debug::ConsoleMenu::Draw()
{
#if defined(EAE6320_CONSOLE_MENU_ISENABLED)	
	if (isConsoleMenuEnabled)for (size_t i = 0; i < totalMenuItems; i++)Graphics::SetUIObjects(menuItems[i]);
#endif
}

void eae6320::Debug::ConsoleMenu::CleanUp()
{
#if defined(EAE6320_CONSOLE_MENU_ISENABLED)
	for (size_t i = 0; i < totalMenuItems; i++)
	{
		delete menuItems[i];
		menuItems[i] = nullptr;
	}
	menuItems.clear();
#endif
}