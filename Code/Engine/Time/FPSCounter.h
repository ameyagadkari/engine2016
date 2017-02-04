/*
This file counts FPS 
*/

#ifndef EAE6320_FPS_H
#define EAE6320_FPS_H

// Interface
//==========

namespace eae6320
{
	namespace Time
	{
		class FPSCounter
		{
		public:
			size_t static Getcount();
			static size_t count;
			static size_t retcount;
		private:
			static float currentTime;
			static float elaspsedTime;
		};
	}
}

#endif	// EAE6320_FPS_H
