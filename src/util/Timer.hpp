#ifndef TIMER_H
#define TIMER_H

#include <vector>

namespace Timer
{
	extern const float DEFAULT_UPDATE_PERIOD;
	extern const unsigned long DWORD_DEFAULT_UPDATE_PERIOD;

	double getTime();
	unsigned long long getUnixSeconds();

	typedef void(*UpdateFunction)(float);

	extern std::vector<UpdateFunction> updateFunctions;
	extern bool updateCycleRunning;

	void registerUpdateFunction(UpdateFunction func);
	void unregisterUpdateFunction(UpdateFunction func);

	void beginUpdateCycle(unsigned long updatePeriod);
	void endUpdateCycle();
}

#endif // TIMER_H