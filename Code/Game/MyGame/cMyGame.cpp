// Header Files
//=============

#include <map>
#include <vector>
#include <regex>
#include "../Gameplay/GameObject.h"
#include "../Gameplay/GameObject2D.h"
#include "../Debug/DebugObject.h"
#include "../Debug/Text.h"
#include "../Debug/Checkbox.h"
#include "../Debug/Slider.h"
#include "../Debug/Button.h"
#include "cMyGame.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Camera/FPSCameraController.h"
#include "../../Engine/Camera/FlyCameraController.h"
#include "../../Engine/Time/Time.h"



// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cMyGame::~cMyGame()
{

}
// Helper Function Declarations
//=============================

namespace
{
	void GenerateRelativePaths(std::string prefix);
	std::map<const std::string, eae6320::Gameplay::GameObject*> gameObjects;
	std::vector<eae6320::Debug::Shapes::DebugObject*> debugObjects;
	std::map<const std::string, eae6320::Gameplay::GameObject2D*> gameObjects2D;
	std::vector<std::string> relativePaths;
	std::vector<std::string> fileNames;
	const std::regex pattern_match("(\\.)([[:alpha:]]+)");
	const std::string pattern_replace("");
	float resetRadius;
}
// Inherited Implementation
//=========================

// Initialization / Clean Up
//--------------------------

bool eae6320::cMyGame::Initialize()
{
	bool wereThereErrors = false;
	{
		const std::string prefix = "data/gameobjects/";
		GenerateRelativePaths(prefix);
		const size_t numberOfGameObjects = relativePaths.size();
		for (size_t i = 0; i < numberOfGameObjects; i++)
		{
			gameObjects[fileNames[i]] = (Gameplay::GameObject::LoadGameObject(relativePaths[i].c_str()));
		}
		if (!numberOfGameObjects)
		{
			wereThereErrors = true;
			EAE6320_ASSERT(false);
			Logging::OutputError("No Gameobjects to draw build the assets.");
		}
	}
	{
		const std::string prefix = "data/gameobjects2d/";
		GenerateRelativePaths(prefix);
		const size_t numberOfGameObjects2D = relativePaths.size();
		for (size_t i = 0; i < numberOfGameObjects2D; i++)
		{
			gameObjects2D[fileNames[i]] = (Gameplay::GameObject2D::LoadGameObject2D(relativePaths[i].c_str()));
		}
	}
	{
#if defined(EAE6320_DEBUG_UI_AREENABLED)
		Debug::UI::debugUIs.push_back(new Debug::UI::Text({ -500,350 }, "FPS: ", { 1.0f,0.0f,0.0f }));
		Debug::UI::debugUIs.push_back(new Debug::UI::Checkbox({ -500,300 }, "Switch Debug Shapes Off", "Switch Debug Shapes On", { 0.0f, 1.0f, 0.0f }));
		Debug::UI::debugUIs.push_back(new Debug::UI::Slider({ -500,250 }, "Radius: ", { 1.0f,0.0f,0.5f }, 10.0f, 100.0f));
		Debug::UI::debugUIs.push_back(new Debug::UI::Button({ -500,200 }, "Reset Radius", { 0.8f,1.0f,0.0f }));

		//After adding all debug UIs, doing this is must
		Debug::UI::IUIController::UpdateUIElements();
#endif
	}

	//Make different cameras and pushback in cameras vector
	Camera::Camera *fpsCam = Camera::Camera::Initialize(Camera::FPSCameraController::Initialize(),Math::cVector(0.0f, 100.0f, 400.0f));
	Camera::Camera::PushBackToVector(fpsCam);
	Camera::Camera *flyCam = Camera::Camera::Initialize(Camera::FlyCameraController::Initialize(), Math::cVector(-5.0f, 5.0f, 50.0f));
	Camera::Camera::PushBackToVector(flyCam);

	//After adding all cameras, doing this is must
	Camera::Camera::UpdateMaxCameras();

	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		for (size_t i = 0; i < 3; i++)
		{
			debugObjects.push_back(fpsCam->m_sphere[i]);
		}
		for (size_t i = 0; i < 3; i++)
		{
			debugObjects.push_back(flyCam->m_sphere[i]);
		}
#endif
	}

	return !wereThereErrors;
}

void eae6320::cMyGame::ChangeCamera()
{
#if defined(EAE6320_DEBUG_UI_AREENABLED)
	if (!Debug::UI::isDebugMenuEnabled)
#endif
	{
		Camera::Camera::ChangeCurrentCamera();
	}
}

void eae6320::cMyGame::UpdateCameraPostion()
{
#if defined(EAE6320_DEBUG_UI_AREENABLED)
	if (!Debug::UI::isDebugMenuEnabled)
#endif
	{
		Camera::Camera::GetCurrentCamera()->UpdateCameraPosition();
	}
}

void eae6320::cMyGame::UpdateDebugUI()
{
#if defined(EAE6320_DEBUG_UI_AREENABLED)
	Debug::UI::IUIController::ProcessInput();
	if (Debug::UI::isDebugMenuEnabled)
	{
		if (Debug::UI::debugUIs[0])
			Debug::UI::debugUIs[0]->Update("FPS: " + std::to_string(Time::fps));
		if (Debug::UI::debugUIs[1])
			Debug::UI::debugUIs[1]->Update();
		if (Debug::UI::debugUIs[2])
		{
			const float valueBeforeUpdate = reinterpret_cast<Debug::UI::Slider*>(Debug::UI::debugUIs[2])->GetValue();
			Debug::UI::debugUIs[2]->Update();
			const float valueAfterUpdate = reinterpret_cast<Debug::UI::Slider*>(Debug::UI::debugUIs[2])->GetValue();
			if (valueBeforeUpdate != valueAfterUpdate)
			{
				//debugObjects[0]->UpdateSphere(valueAfterUpdate);
			}
		}
		if (Debug::UI::debugUIs[3])
		{
			Debug::UI::debugUIs[3]->Update();
			if (reinterpret_cast<Debug::UI::Button*>(Debug::UI::debugUIs[3])->GetState())
			{
				//debugObjects[0]->UpdateSphere(resetRadius);
				reinterpret_cast<Debug::UI::Slider*>(Debug::UI::debugUIs[2])->Reset(resetRadius);
				reinterpret_cast<Debug::UI::Button*>(Debug::UI::debugUIs[3])->SetState(false);
			}
		}
	}
#endif
}

void eae6320::cMyGame::UpdateCameraOrientation()
{
#if defined(EAE6320_DEBUG_UI_AREENABLED)
	if (!Debug::UI::isDebugMenuEnabled)
#endif
	{
		Camera::Camera::GetCurrentCamera()->UpdateCameraOrientation();
	}
}


void eae6320::cMyGame::SubmitCamera()
{
	Camera::Camera *currentCamera = Camera::Camera::GetCurrentCamera();
	if (currentCamera)
	{
		Graphics::SetCamera(currentCamera);
	}
	else
	{
		EAE6320_ASSERT(false);
		Logging::OutputError("No Current Camera Exists");
	}
}

void eae6320::cMyGame::UpdateGameObjectPosition()
{
#if defined(EAE6320_DEBUG_UI_AREENABLED)
	if (!Debug::UI::isDebugMenuEnabled)
#endif
	{
		for (auto const& gameObject : gameObjects)
		{
			if (gameObject.second)
			{
				gameObject.second->UpdateGameObjectPosition();
			}
		}
	}
}

void eae6320::cMyGame::UpdateGameObjectOrientation()
{
#if defined(EAE6320_DEBUG_UI_AREENABLED)
	if (!Debug::UI::isDebugMenuEnabled)
#endif
	{
		for (auto const& gameObject : gameObjects)
		{
			if (gameObject.second)
			{
				gameObject.second->UpdateGameObjectOrientation();
			}
		}
	}
}

void eae6320::cMyGame::SubmitDrawcallData3D()
{
	for (auto const& gameObject : gameObjects)
	{
		if (gameObject.second)
		{
			Graphics::SetGameObject(gameObject.second);
		}
	}
}

void eae6320::cMyGame::SubmitDebugShapeData3D()
{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
	if (reinterpret_cast<Debug::UI::Checkbox*>(Debug::UI::debugUIs[1])->GetIsEnabled())
	{
		const size_t length = debugObjects.size();
		for (size_t i = 0; i < length; i++)
		{
			Graphics::SetDebugObject(debugObjects[i]);
		}
	}
#endif
}

void eae6320::cMyGame::SubmitDebugUIData2D()
{
#if defined(EAE6320_DEBUG_UI_AREENABLED)
	if (Debug::UI::isDebugMenuEnabled)
	{
		const size_t length = Debug::UI::debugUIs.size();
		for (size_t i = 0; i < length; i++)
		{
			Graphics::SetDebugUI(Debug::UI::debugUIs[i]);
		}
	}
#endif
}

void eae6320::cMyGame::SubmitDrawcallData2D()
{
	for (auto const& gameObject2D : gameObjects2D)
	{
		if (gameObject2D.second)
		{
			Graphics::SetGameObject2D(gameObject2D.second);
		}
	}
}

bool eae6320::cMyGame::CleanUp()
{
	bool wereThereErrors = false;

	// Deleting GameObjects
	{
		for (auto const& gameObject : gameObjects)
		{
			if (gameObject.second)
			{
				delete gameObject.second;
			}
		}
		gameObjects.clear();
	}
	// Deleting DebugObjects
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		const size_t length = debugObjects.size();
		for (size_t i = 0; i < length; i++)
		{
			delete debugObjects[i];
		}

		debugObjects.clear();
#endif
	}
	// Deleting Debug UI
	{
#if defined(EAE6320_DEBUG_UI_AREENABLED)
		const size_t length = Debug::UI::debugUIs.size();
		for (size_t i = 0; i < length; i++)
		{
			delete Debug::UI::debugUIs[i];
		}
		Debug::UI::debugUIs.clear();
#endif
	}
	// Deleting GameObjects2D
	{
		for (auto const& gameObject2D : gameObjects2D)
		{
			if (gameObject2D.second)
			{
				delete gameObject2D.second;
			}
		}
		gameObjects2D.clear();
	}
	if (!Camera::Camera::CleanUp())
	{
		wereThereErrors = true;
		EAE6320_ASSERT(false);
		Logging::OutputError("Camera Cleanup Failed");
	}
	return !wereThereErrors;
}

// Helper Function Definitions
//=============================

namespace
{
	void GenerateRelativePaths(std::string prefix)
	{
		relativePaths.clear();
		fileNames.clear();
		WIN32_FIND_DATA search_data;
		memset(&search_data, 0, sizeof(WIN32_FIND_DATA));
		HANDLE handle = FindFirstFile((prefix + "*").c_str(), &search_data);
		if (handle != INVALID_HANDLE_VALUE)
		{
			do
			{
				if (!strcmp(search_data.cFileName, "."))continue;
				if (!strcmp(search_data.cFileName, ".."))continue;
				if (!strcmp(search_data.cFileName, "ui"))continue;
				relativePaths.push_back((prefix + search_data.cFileName).c_str());
				fileNames.push_back(std::regex_replace(static_cast<std::string>(search_data.cFileName), pattern_match, pattern_replace));
			} while (FindNextFile(handle, &search_data));
		}
		FindClose(handle);
	}
}