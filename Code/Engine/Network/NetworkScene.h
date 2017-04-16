#ifndef EAE6320_NETWORKSCENE_H
#define EAE6320_NETWORKSCENE_H

namespace eae6320
{
	namespace Debug
	{
		namespace UI
		{
			class Button;
		}
	}
}

namespace eae6320
{
	namespace Gameplay
	{
		class GameObject2D;
	}
}

namespace eae6320
{
	namespace Network
	{
		namespace NetworkScene
		{
			enum GameState;

			extern Debug::UI::Button* singlePlayer;
			extern Debug::UI::Button* multiPlayer;
			extern Debug::UI::Button* server;
			extern Debug::UI::Button* client;
			
			extern Gameplay::GameObject2D* background;		
			extern GameState currentGameState;

			bool Initialize();
			bool CleanUp();
			bool Update();
		}
	}
}

#endif // !EAE6320_NETWORKSCENE_H