#ifndef KEYFRAME_H
#define KEYFRAME_H

#include "BreathSample.hpp"
#include "..\util\interpolators.hpp"

struct Keyframe
{
	BreathSample sample;
	float time;
	Interpolators::Method method;

	Keyframe();
	Keyframe(BreathSample sample, float time, Interpolators::Method method = Interpolators::Constant);
	~Keyframe();

	bool hasHold() const;

	int toString(char *buffer, int bufferSize) const;
};

#endif