#include "Keyframe.hpp"
#include <stdio.h>


Keyframe::Keyframe()
{
}

Keyframe::Keyframe(BreathSample sample, float time, Interpolators::Method method)
{
	this->sample = sample;
	this->time = time;
	this->method = method;
}

Keyframe::~Keyframe()
{
}

bool Keyframe::hasHold() const
{
	return method == Interpolators::Hold;
}

int Keyframe::toString(char* buffer, int bufferSize) const
{
	int written = 0;

	written += sprintf(buffer, "time: %0.2f, method: %d, sample: {", time, method);
	written += sample.toString(buffer + written, bufferSize - written);
	written += sprintf(buffer + written, "}");

	return written;
}