// Header Files
//=============

#include "cMyGame.h"
#include "../Gameplay/GameObject.h"
#include "../Gameplay/GameObject2D.h"
#include "../Debug/DebugObject.h"
#include "../Debug/ConsoleMenu.h"
#include "../../Engine/Asserts/Asserts.h"
#include "../../Engine/Logging/Logging.h"
#include "../../Engine/Graphics/Graphics.h"
#include "../../Engine/Camera/Camera.h"
#include "../../Engine/Camera/FlyCameraController.h"
#include "../../Engine/Camera/FPSCameraController.h"
#include "../../Engine/Camera/TPSCameraController.h"
#include "../Gameplay/FPSPlayerController.h"
#include "../Gameplay/TPSPlayerController.h"
#include "../../Engine/Network/NetworkScene.h"
#include "../../Engine/Network/NetworkManager.h"
#include "../../Engine/Audio/AudioManager.h"

#include <map>
#include <vector>
#include <regex>



#define FIRSTPERSONMODE 0

// Interface
//==========

// Initialization / Clean Up
//--------------------------

eae6320::cMyGame::~cMyGame()
{}
// Helper Function Declarations
//=============================

namespace
{
	void GenerateRelativePaths(std::string prefix);
	std::map<const std::string, eae6320::Gameplay::GameObject*> gameObjects;
	std::map<const std::string, eae6320::Gameplay::GameObject2D*> gameObjects2D;
	std::vector<std::string> relativePaths;
	std::vector<std::string> fileNames;
	const std::regex pattern_match("(\\.)([[:alpha:]]+)");
	const std::string pattern_replace("");

#pragma region Update
	void ChangeCamera();
	void UpdateGameObjectOrientation();
	void UpdateGameObjectPosition();
	void UpdateCameraOrientation();
	void UpdateCameraPostion();
	void UpdateConsoleMenu();
	void UpdateHUD();
#pragma endregion 

#pragma region Submit
	void SubmitCamera();
	void SubmitDrawcallData3D();
	void SubmitConsoleMenu();
	void SubmitDrawcallData2D();
	void SubmitHUD();
#pragma endregion 
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

	// Initialize Console Menu
	Debug::ConsoleMenu::Initialize();

	//Make different cameras and pushback in cameras vector
#if FIRSTPERSONMODE
	Camera::Camera *fpsCam = new Camera::Camera(reinterpret_cast<Gameplay::cbController*>(Camera::FPSCameraController::Initialize(2000.0f)), Math::cVector(0.0, 100.0, 200.0));
	Camera::Camera::PushBackToVector(fpsCam);
#else
	Camera::Camera *tpsCam = new Camera::Camera(reinterpret_cast<Gameplay::cbController*>(Camera::TPSCameraController::Initialize(2000.0f)), Math::cVector(0.0, 100.0, 200.0));
	Camera::Camera::PushBackToVector(tpsCam);
#endif

	Camera::Camera *flyCam = new Camera::Camera(reinterpret_cast<Gameplay::cbController*>(Camera::FlyCameraController::Initialize()), Math::cVector(-5.0f, 5.0f, 50.0f));
	Camera::Camera::PushBackToVector(flyCam);

	//After adding all cameras, doing this is must
	Camera::Camera::UpdateMaxCameras();


#if FIRSTPERSONMODE
	Gameplay::GameObject* localPlayer = gameObjects.at("playerfirstperson");
	reinterpret_cast<Camera::TPSCameraController&>(fpsCam->GetController()).SetPlayerTransform(&localPlayer->GetTransformAddress());
	reinterpret_cast<Gameplay::TPSPlayerController&>(localPlayer->GetController()).SetCameraTransform(&fpsCam->GetTransformAddress());
#else
	Gameplay::GameObject* localPlayer = gameObjects.at("playerthirdperson");
	reinterpret_cast<Camera::TPSCameraController&>(tpsCam->GetController()).SetPlayerTransform(&localPlayer->GetTransformAddress());
	reinterpret_cast<Gameplay::TPSPlayerController*>(localPlayer->GetController())->SetCameraTransform(&tpsCam->GetTransformAddress());
	Audio::AudioManager::GetSingleton()->SetCameraTransform(&tpsCam->GetTransformAddress());
	Network::NetworkManager::AddToMap("playerthirdperson", localPlayer);
	Network::NetworkManager::AddToMap("playerthirdpersonremote", gameObjects.at("playerthirdpersonremote"));
	
	Network::NetworkManager::AddToMap("myteamflagserver", gameObjects.at("myteamflagserver"));
	Network::NetworkManager::AddToMap("otherteamflagserver", gameObjects.at("otherteamflagserver"));
	Network::NetworkManager::AddToMap("myteamflagclient", gameObjects.at("myteamflagclient"));
	Network::NetworkManager::AddToMap("otherteamflagclient", gameObjects.at("otherteamflagclient"));

	Network::NetworkManager::AddToMap("myscorezoneserver", gameObjects.at("myscorezoneserver"));
	Network::NetworkManager::AddToMap("otherscorezoneserver", gameObjects.at("otherscorezoneserver"));
	Network::NetworkManager::AddToMap("myscorezoneclient", gameObjects.at("myscorezoneclient"));
	Network::NetworkManager::AddToMap("otherscorezoneclient", gameObjects.at("otherscorezoneclient"));

	gameObjects.erase("playerthirdpersonremote");

	gameObjects.erase("myteamflagserver");
	gameObjects.erase("myteamflagclient");
	gameObjects.erase("otherteamflagserver");
	gameObjects.erase("otherteamflagclient");

	gameObjects.erase("myscorezoneserver");
	gameObjects.erase("otherscorezoneserver");
	gameObjects.erase("myscorezoneclient");
	gameObjects.erase("otherscorezoneclient");
#endif

	// Initialize Network Scene
	Network::NetworkScene::Initialize();

	return !wereThereErrors;
}

void eae6320::cMyGame::Update()
{
	ChangeCamera();
	if (Network::NetworkScene::currentGameState != Network::NetworkScene::RunSinglePlayer &&
		Network::NetworkScene::currentGameState != Network::NetworkScene::RunMultiplayer)
	{
		Network::NetworkScene::Update();
		return;
	}	
	UpdateGameObjectOrientation();
	UpdateGameObjectPosition();
	UpdateCameraOrientation();
	UpdateCameraPostion();
	UpdateConsoleMenu();
	UpdateHUD();
	Network::NetworkScene::Update();
}

void eae6320::cMyGame::Submit()
{
	SubmitCamera();
	if (Network::NetworkScene::currentGameState == Network::NetworkScene::RunSinglePlayer ||
		Network::NetworkScene::currentGameState == Network::NetworkScene::RunMultiplayer)
	{
		SubmitDrawcallData3D();
		SubmitConsoleMenu();
		SubmitHUD();
	}
	Network::NetworkScene::Draw();
	SubmitDrawcallData2D();
}

namespace
{
#pragma region Update

	void ChangeCamera()
	{
		eae6320::Camera::Camera::ChangeCurrentCamera();
	}
	void UpdateGameObjectOrientation()
	{
		for (auto const& gameObject : gameObjects)
		{
			if (gameObject.second)
			{
				gameObject.second->UpdateGameObjectOrientation();
			}
		}
	}
	void UpdateGameObjectPosition()
	{
		for (auto const& gameObject : gameObjects)
		{
			if (gameObject.second)
			{
				gameObject.second->UpdateGameObjectPosition();
			}
		}
	}
	void UpdateCameraOrientation()
	{
		eae6320::Camera::Camera::GetCurrentCamera()->UpdateCameraOrientation();
	}
	void UpdateCameraPostion()
	{
		eae6320::Camera::Camera::GetCurrentCamera()->UpdateCameraPosition();
	}
	void UpdateConsoleMenu()
	{
		eae6320::Debug::ConsoleMenu::Update();
	}
	void UpdateHUD()
	{
		size_t length = eae6320::Debug::UI::HUD.size();
		for (size_t i = 0; i < length; i++)
		{
			eae6320::Debug::UI::HUD[i]->Update();
		}
	}

#pragma endregion 

#pragma region Submit

	void SubmitCamera()
	{
		eae6320::Camera::Camera *currentCamera = eae6320::Camera::Camera::GetCurrentCamera();
		if (currentCamera)
		{
			eae6320::Graphics::SetCamera(currentCamera);
		}
		else
		{
			EAE6320_ASSERT(false);
			eae6320::Logging::OutputError("No Current Camera Exists");
		}
	}
	void SubmitDrawcallData3D()
	{
		for (auto const& gameObject : gameObjects)
		{
			if (gameObject.second)
			{
				eae6320::Graphics::SetGameObject(gameObject.second);
			}
		}
	}
	void SubmitConsoleMenu()
	{
		eae6320::Debug::ConsoleMenu::Draw();
	}
	void SubmitDrawcallData2D()
	{
		for (auto const& gameObject2D : gameObjects2D)
		{
			if (gameObject2D.second)
			{
				eae6320::Graphics::SetGameObject2D(gameObject2D.second);
			}
		}
	}
	void SubmitHUD()
	{
		size_t length = eae6320::Debug::UI::HUD.size();
		for (size_t i = 0; i < length; i++)
		{
			eae6320::Graphics::SetUIObjects(eae6320::Debug::UI::HUD[i]);
		}		
	}

#pragma endregion 
}

bool eae6320::cMyGame::CleanUp()
{
	bool wereThereErrors = false;

	// Deleting GameObjects
	{
		for (auto& gameObject : gameObjects)
		{
			if (gameObject.second)
			{
				delete gameObject.second;
				gameObject.second = nullptr;
			}
		}
		gameObjects.clear();
	}
	// Deleting DebugObjects
	{
#if defined(EAE6320_DEBUG_SHAPES_AREENABLED)
		const size_t length = Debug::Shapes::DebugObject::ms_debugObjects.size();
		for (size_t i = 0; i < length; i++)
		{
			delete Debug::Shapes::DebugObject::ms_debugObjects[i];
			Debug::Shapes::DebugObject::ms_debugObjects[i] = nullptr;
		}

		Debug::Shapes::DebugObject::ms_debugObjects.clear();
#endif
	}

	// Deleting Console Menu
	Debug::ConsoleMenu::CleanUp();

	// Deleting Network Scene
	Network::NetworkScene::CleanUp();

	// Deleteing Network Manager
	Network::NetworkManager::CleanUp();

	// Deleting HUD elements
	size_t length = Debug::UI::HUD.size();
	for (size_t i = 0; i < length; i++)
	{
		delete Debug::UI::HUD[i];
		Debug::UI::HUD[i] = nullptr;
	}

	// Deleting GameObjects2D
	{
		for (auto& gameObject2D : gameObjects2D)
		{
			if (gameObject2D.second)
			{
				delete gameObject2D.second;
				gameObject2D.second = nullptr;
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
				if (!strcmp(search_data.cFileName, "network"))continue;
				relativePaths.push_back((prefix + search_data.cFileName).c_str());
				fileNames.push_back(regex_replace(static_cast<std::string>(search_data.cFileName), pattern_match, pattern_replace));
			} while (FindNextFile(handle, &search_data));
		}
		FindClose(handle);
	}
}