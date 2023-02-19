#pragma once

#include <vector>

namespace Timer
{
	extern const float DEFAULT_UPDATE_PERIOD;
	extern const unsigned long DWORD_DEFAULT_UPDATE_PERIOD;

	double getTime();

	typedef void(*UpdateFunction)(float);

	extern std::vector<UpdateFunction> updateFunctions;
	extern bool updateCycleRunning;

	void registerUpdateFunction(UpdateFunction func);
	void unregisterUpdateFunction(UpdateFunction func);

	void beginUpdateCycle(unsigned long updatePeriod);
	void endUpdateCycle();
}