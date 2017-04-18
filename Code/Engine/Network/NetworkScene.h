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
			enum GameState { SelectPlayType, SelectModeType, Run };
			extern GameState currentGameState;

			void Initialize();
			void Update();
			void Draw();
			void CleanUp();
		}
	}
}

#endif // !EAE6320_NETWORKSCENE_H