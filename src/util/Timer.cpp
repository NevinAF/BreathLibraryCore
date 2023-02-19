#include "pch.h"

#include "Timer.hpp"
#include <ctime>

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

const float Timer::DEFAULT_UPDATE_PERIOD = 0.033f; // 30 FPS
const unsigned long Timer::DWORD_DEFAULT_UPDATE_PERIOD = (unsigned long)(DEFAULT_UPDATE_PERIOD * 1000.0f);
std::vector<Timer::UpdateFunction> Timer::updateFunctions;
bool Timer::updateCycleRunning = false;

double Timer::getTime()
{
	static clock_t start = clock();

	return (double)(clock() - start) / CLOCKS_PER_SEC;
}

void Timer::registerUpdateFunction(UpdateFunction func)
{
	updateFunctions.push_back(func);
}

void Timer::unregisterUpdateFunction(UpdateFunction func)
{
	for (auto it = updateFunctions.begin(); it != updateFunctions.end(); it++)
	{
		if (*it == func)
		{
			updateFunctions.erase(it);
			break;
		}
	}
}

void Timer::beginUpdateCycle(unsigned long updatePeriod)
{
	if (updatePeriod <= 0)
	{
		updatePeriod = DWORD_DEFAULT_UPDATE_PERIOD;
	}

	updateCycleRunning = true;
	double lastTime = getTime();
	while (updateCycleRunning)
	{
		Sleep(updatePeriod);
		double currentTime = getTime();

		for (auto it = updateFunctions.begin(); it != updateFunctions.end(); it++)
		{
			(*it)((float)(currentTime - lastTime));
		}

		lastTime = currentTime;
	}
}

void Timer::endUpdateCycle()
{
	updateCycleRunning = false;
}