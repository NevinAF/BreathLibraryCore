#pragma once

#include "BreathSample.hpp"

/**
 * @brief A class that can sample breath data. Usually used for creating a BreathStream.
 */
class IBreathSampler
{
public:
	/** @return The latest sample processed by sampler*/
	virtual BreathSample getSample() = 0;
};

class ICorrelator
{
public:
	/** @return The latest sample processed by sampler*/
	virtual float getCorrelation() = 0;
};