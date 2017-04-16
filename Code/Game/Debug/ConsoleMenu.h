#ifndef EAE6320_CONSOLEMENU_H
#define EAE6320_CONSOLEMENU_H

namespace eae6320
{
	namespace Debug
	{
		namespace UI
		{
			class IUIController;
		}
	}
}

namespace eae6320
{
	namespace Debug
	{
		namespace ConsoleMenu
		{
			void Initialize();
			void Update();
			void Draw();
			void CleanUp();
		}
	}
}

#endif // !EAE6320_CONSOLEMENU_H

