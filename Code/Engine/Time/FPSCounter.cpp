#include "FPSCounter.h"
#include "Time.h"

size_t eae6320::Time::FPSCounter::count = 0;
size_t eae6320::Time::FPSCounter::retcount = 0;
float eae6320::Time::FPSCounter::currentTime = 0.0f;
float eae6320::Time::FPSCounter::elaspsedTime = 0.0f;

size_t eae6320::Time::FPSCounter::Getcount()
{
	currentTime = GetElapsedSecondCount_total();
	if (currentTime - elaspsedTime > 1.0f)
	{
		elaspsedTime = currentTime;
		retcount = count;
		count = 0;
	}
	return retcount;
}