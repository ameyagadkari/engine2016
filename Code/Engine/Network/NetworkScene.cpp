#include "NetworkScene.h"
#include "NetworkManager.h"
#include "../../Game/Gameplay/GameObject2D.h"
#include "../../Game/Debug/Button.h"
#include "../../Game/Debug/Text.h"
#include "../Graphics/Graphics.h"
#include "../UserInput/UserInput.h"

#include <string>


namespace
{
	void SinglePlayerButtonPressed();
	void MultiPlayerButtonPressed();
	void ServerButtonPressed();
	void ClientButtonPressed();

	eae6320::Gameplay::GameObject2D* background;
	eae6320::Debug::UI::Text* gameName = nullptr;
	eae6320::Debug::UI::Button* singlePlayer = nullptr;
	eae6320::Debug::UI::Button* multiPlayer = nullptr;
	eae6320::Debug::UI::Button* server = nullptr;
	eae6320::Debug::UI::Button* client = nullptr;

	const std::string backgroundFilePath = "data/gameobjects2d/network/networkscreenbackground.bing2dobj";
}

namespace eae6320
{
	namespace Network
	{
		namespace NetworkScene
		{
			GameState currentGameState = SelectPlayType;
		}
	}
}

void eae6320::Network::NetworkScene::Initialize()
{
	currentGameState = SelectPlayType;
	background = Gameplay::GameObject2D::LoadGameObject2D(backgroundFilePath.c_str());
	gameName = new Debug::UI::Text({ -200,200 }, "Ameya Gadkari's: Capture The Flag", { 0.5f,1.0f,0.0f }, nullptr, true);
	singlePlayer = new Debug::UI::Button({ -200,100 }, "Single Player", { 0.0f,1.0f,0.0f }, 0.25f, &SinglePlayerButtonPressed, true);
	multiPlayer = new Debug::UI::Button({ -200,0 }, "Multi Player", { 0.0f,1.0f,0.0f }, 0.25f, &MultiPlayerButtonPressed, false);
	server = new Debug::UI::Button({ -200,100 }, "Server", { 0.0f,1.0f,0.0f }, 0.25f, &ServerButtonPressed, true);
	client = new Debug::UI::Button({ -200,0 }, "Client", { 0.0f,1.0f,0.0f }, 0.25f, &ClientButtonPressed, false);
}

void eae6320::Network::NetworkScene::Update()
{
	if (currentGameState == SelectPlayType)
	{
		if (UserInput::GetKeyDown(VK_DOWN) || UserInput::GetKeyDown(VK_UP))
		{
			singlePlayer->isSelected = !singlePlayer->isSelected;
			multiPlayer->isSelected = !multiPlayer->isSelected;
		}
		reinterpret_cast<Debug::UI::IUIController*>(singlePlayer)->Update();
		reinterpret_cast<Debug::UI::IUIController*>(multiPlayer)->Update();
	}
	else if (currentGameState == SelectModeType)
	{
		if (UserInput::GetKeyDown(VK_DOWN) || UserInput::GetKeyDown(VK_UP))
		{
			server->isSelected = !server->isSelected;
			client->isSelected = !client->isSelected;
		}
		if (UserInput::GetKeyDown(VK_BACK))
		{
			currentGameState = SelectPlayType;
			return;
		}
		reinterpret_cast<Debug::UI::IUIController*>(server)->Update();
		reinterpret_cast<Debug::UI::IUIController*>(client)->Update();
	}
}

void eae6320::Network::NetworkScene::Draw()
{
	if (currentGameState == SelectPlayType)
	{
		Graphics::SetGameObject2D(background);
		Graphics::SetUIObjects(gameName);
		Graphics::SetUIObjects(singlePlayer);
		Graphics::SetUIObjects(multiPlayer);
	}
	else if (currentGameState == SelectModeType)
	{
		Graphics::SetGameObject2D(background);
		Graphics::SetUIObjects(gameName);
		Graphics::SetUIObjects(server);
		Graphics::SetUIObjects(client);
	}
}
void eae6320::Network::NetworkScene::CleanUp()
{
	if (gameName)
	{
		delete gameName;
		gameName = nullptr;
	}
	if (singlePlayer)
	{
		delete singlePlayer;
		singlePlayer = nullptr;
	}
	if (multiPlayer)
	{
		delete multiPlayer;
		multiPlayer = nullptr;
	}
	if (server)
	{
		delete server;
		server = nullptr;
	}
	if (client)
	{
		delete client;
		client = nullptr;
	}
	if (background)
	{
		delete background;
		background = nullptr;
	}
}


namespace
{
	using namespace eae6320::Network::NetworkScene;
	void SinglePlayerButtonPressed()
	{
		currentGameState = Run;
	}

	void MultiPlayerButtonPressed()
	{
		currentGameState = SelectModeType;
	}

	void ServerButtonPressed()
	{
		eae6320::Network::NetworkManager::Initialize(true);
		currentGameState = Run;
	}

	void ClientButtonPressed()
	{
		eae6320::Network::NetworkManager::Initialize(false);
		currentGameState = Run;
	}
}