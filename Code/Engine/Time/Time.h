/*
	This file manages time-related functionality
*/

#ifndef EAE6320_TIME_H
#define EAE6320_TIME_H

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
	}
}

#endif	// EAE6320_TIME_H
