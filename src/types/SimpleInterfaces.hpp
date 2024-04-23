#ifndef SIMPLEINTERFACES_H
#define SIMPLEINTERFACES_H

#include "BreathSample.hpp"
#include "DataStreamMeta.hpp"

/**
 * @brief A class that can sample breath data. Usually used for creating a BreathStream.
 */
class IBreathSampler
{
public:
	/** @return The latest sample processed by sampler */
	virtual BreathSample Sample() = 0;
};

class ICorrelator
{
public:
	/** @return The latest accuracy/correlation */
	virtual float Correlation() = 0;
};

class IDataStream
{
public:
	virtual DataStreamMeta getStreamData() = 0;
};

#endif