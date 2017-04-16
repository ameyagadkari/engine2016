/*
	This file manages time-related functionality
*/

#ifndef EAE6320_TIME_H
#define EAE6320_TIME_H
#include <string>

// Interface
//==========

namespace eae6320
{
	namespace Time
	{
		// Time
		//-----

		float GetElapsedSecondCount_total();
		float GetElapsedSecondCount_duringPreviousFrame();

		void OnNewFrame();

		// Initialization / Clean Up
		//--------------------------

		bool Initialize();
		bool CleanUp();

		void SetFPS(std::string& o_text);
	}
}

#endif	// EAE6320_TIME_H
