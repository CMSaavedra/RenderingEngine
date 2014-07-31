#pragma once
#ifndef CLOCK_H
#define CLOCK_H

#include "ExportHeader.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class ENGINE_SHARED Clock
{
public:
	Clock();

	float dt;

	bool initialize();
	bool shutdown();
	void newFrame();
	float timeElapsedLastFrame() const;

	static Clock& getInstance();

private:
	Clock(const Clock&);
	Clock operator=(const Clock&);

	LARGE_INTEGER timeFrequency;
	LARGE_INTEGER timeLastFrame;

	static Clock clockInstance;
	
};

#define clockManager Clock::getInstance()

#endif

