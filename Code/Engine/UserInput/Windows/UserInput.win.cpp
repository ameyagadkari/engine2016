// Header Files
//=============

#include "../UserInput.h"
#include <bitset>

// Helper Function Declarations
//=============================

namespace
{
	bool IsVirtualKeyPressed(const int i_virtualKeyCode);
	bool IsVirtualKeyPressedOnce(const int i_virtualKeyCode);
	bool IsVirtualKeyPressedAndReleased(const int i_virtualKeyCode);
	std::bitset<512>keyPress;
	std::bitset<512>keyRelease;
}

namespace eae6320
{
	namespace UserInput
	{
		bool isWindowInFocus = false;
	}
}

// Interface
//==========

bool eae6320::UserInput::GetKey(const int i_virtualKeyCode)
{
	return isWindowInFocus ? IsVirtualKeyPressed(i_virtualKeyCode) : false;
}

bool eae6320::UserInput::GetKeyDown(const int i_virtualKeyCode)
{
	return isWindowInFocus ? IsVirtualKeyPressedOnce(i_virtualKeyCode) : false;
}

bool eae6320::UserInput::GetKeyUp(const int i_virtualKeyCode)
{
	return  isWindowInFocus ? IsVirtualKeyPressedAndReleased(i_virtualKeyCode) : false;
}

bool eae6320::UserInput::IsMouseButtonPressed(const int i_virtualButtonCode)
{
	return isWindowInFocus ? IsVirtualKeyPressed(i_virtualButtonCode) : false;
}

// Helper Function Definitions
//============================

namespace
{
	bool IsVirtualKeyPressed(const int i_virtualKeyCode)
	{
		const short keyState = GetAsyncKeyState(i_virtualKeyCode);
		const short isKeyDownMask = ~1;
		return (keyState & isKeyDownMask) != 0;
	}

	bool IsVirtualKeyPressedOnce(const int i_virtualKeyCode)
	{
		const short keyState = GetAsyncKeyState(i_virtualKeyCode);
		const short isKeyDownMask = ~1;
		if ((keyState & isKeyDownMask) != 0 && !keyPress[i_virtualKeyCode])
		{
			keyPress.set(i_virtualKeyCode);
			return true;
		}
		if ((keyState & isKeyDownMask) != 0)return false;
		if ((keyState & isKeyDownMask) == 0 && keyPress[i_virtualKeyCode])keyPress.reset(i_virtualKeyCode);
		return false;
	}

	bool IsVirtualKeyPressedAndReleased(const int i_virtualKeyCode)
	{
		const short keyState = GetAsyncKeyState(i_virtualKeyCode);
		const short isKeyDownMask = ~1;
		if ((keyState & isKeyDownMask) != 0 && !keyRelease[i_virtualKeyCode])
		{
			keyRelease.set(i_virtualKeyCode);
			return false;
		}
		if ((keyState & isKeyDownMask) != 0)return false;
		if ((keyState & isKeyDownMask) == 0 && keyRelease[i_virtualKeyCode])
		{
			keyRelease.reset(i_virtualKeyCode);
			return true;
		}
		return false;
	}
}
