#include "ConsoleMenu.h"
#include "ConfigurationConsoleMenu.h"
#include "IUIController.h"
#include "Text.h"
#include "Checkbox.h"
#include "Slider.h"
#include "../../Engine/Time/Time.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/UserInput/UserInput.h"
#include "../../Engine/Network/NetworkScene.h"
#include "../../Engine/UserSettings/UserSettings.h"
#include "../../Engine/Audio/AudioManager.h"

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
	const int16_t x = -380;
	int16_t y = 350;
	menuItems.push_back(new UI::Text({ x,y }, "FPS: ", { 1.0f,0.55f,0.0f }, &Time::SetFPS));
	y -= 50;
	menuItems.push_back(new UI::Checkbox({ x,y }, "Switch Debug Shapes Off", "Switch Debug Shapes On", { 1.0f,0.55f,0.0f }, true, &Shapes::DebugObject::Draw));
	y -= 50;
	menuItems.push_back(new UI::Text({ x,y }, "Network Type: ", { 1.0f,0.55f,0.0f }, &Network::NetworkScene::SetNetworkType));
	y -= 50;
	if (UserSettings::GetMusicState())
	{
		menuItems.push_back(new UI::Slider({ x,y }, "Music: ", { 1.0f,0.55f,0.0f }, 0.25f, 0.0f, 2.0f, 200.0f, nullptr, &Audio::ChangeMusicVolume));
		y -= 50;
	}
	if (UserSettings::GetSoundEffectsState())
	{
		menuItems.push_back(new UI::Slider({ x,y }, "SFX:   ", { 1.0f,0.55f,0.0f }, 1.0f, 0.0f, 2.0f, 200.0f, nullptr, &Audio::ChangeSFXVolume));
		y -= 50;
	}
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