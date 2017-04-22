#ifndef EAE6320_NETWORKSCENE_H
#define EAE6320_NETWORKSCENE_H
#include <string>

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
			enum GameState { SelectPlayType, SelectModeType, WaitingForServer, RunSinglePlayer, RunMultiplayer };
			extern GameState currentGameState;

			void Initialize();
			void Update();
			void Draw();
			void CleanUp();

			void SetNetworkType(std::string& o_text, bool& o_updateString);
		}
	}
}

#endif // !EAE6320_NETWORKSCENE_H