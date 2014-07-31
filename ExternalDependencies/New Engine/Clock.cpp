#include "Clock.h"

Clock Clock::clockInstance;

Clock::Clock()
{
	dt = 0;
}

bool Clock::initialize()
{
	bool b = QueryPerformanceFrequency(&timeFrequency) != 0;
	if(!b)
		return false;

	return QueryPerformanceCounter(&timeLastFrame) != 0;
}

bool Clock::shutdown(){ return true;}

void Clock::newFrame()
{
	LARGE_INTEGER thisTime;
	QueryPerformanceCounter(&thisTime);
	LARGE_INTEGER delta;
	delta.QuadPart = thisTime.QuadPart - timeLastFrame.QuadPart;
	dt = ((float)delta.QuadPart)/timeFrequency.QuadPart;
	timeLastFrame.QuadPart = thisTime.QuadPart;
}

float Clock::timeElapsedLastFrame() const
{
	return dt;
}

Clock& Clock::getInstance()
{
	return clockInstance;
}
