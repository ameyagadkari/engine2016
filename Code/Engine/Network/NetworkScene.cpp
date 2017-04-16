#include "NetworkScene.h"
#include "../../Game/Gameplay/GameObject2D.h"
#include "../../Game/Debug/Button.h"

#include <string>

namespace
{
	std::string backgroundFilePath = "data/gameobjects2d/networkscreenbackground.bing2dobj";
}

namespace eae6320
{
	namespace Network
	{
		namespace NetworkScene
		{
			enum GameState { SelectPlayType, SelectModeType, Run };
			GameState currentGameState = SelectPlayType;			
			Gameplay::GameObject2D* background = nullptr;
			Debug::UI::Button* singlePlayer = nullptr;
			Debug::UI::Button* multiPlayer = nullptr;
			Debug::UI::Button* server = nullptr;
			Debug::UI::Button* client = nullptr;
		}
	}
}

bool eae6320::Network::NetworkScene::Initialize()
{
	currentGameState = SelectPlayType;
	const bool doNotUsePixelCoordinatesFromFile = false;
	background = Gameplay::GameObject2D::LoadGameObject2D(backgroundFilePath.c_str(), doNotUsePixelCoordinatesFromFile, 0, 0);
	singlePlayer = new Debug::UI::Button({ -200,-200 }, "Single Player", { 0,1,0 });
	multiPlayer = new Debug::UI::Button({ -200,-100 }, "Multi Player", { 0,1,0 });
	server = new Debug::UI::Button({ -200,-200 }, "Server", { 0,1,0 });
	client = new Debug::UI::Button({ -200,-100 }, "Client", { 0,1,0 });
	return true;
}