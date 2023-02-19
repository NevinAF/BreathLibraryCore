#include "pch.h"

#include "SyncedPatternCorrelator.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\serialization\EnumDefinitions.hpp"

CreateSerializable(SyncedPatternCorrelator);

void SyncedPatternCorrelator::setComparingSample(const BreathSample& samplerSample, float delta)
{
	if (pattern->getNumKeyframes() == 0)
	{
		EDebug::Error("SyncedPatternCorrelator::update() - Pattern has no keyframes");
		return;
	}
	
	if (pattern->getNumKeyframes() == 1)
	{
		comparingSample = pattern->getKeyframes()[0].sample;
		return;
	}

	if (_setComparingSample_HoldCheck(delta))
	{
		return;
	}

	if (absMatchRange_before + absMatchRange_after == 0)
	{
		comparingSample = pattern->getTarget(patternTime);
		return;
	}
	
	float minTime;
	float maxTime;

	if (absMatchRange_before + absMatchRange_after >= pattern->getLength())
	{
		float minTime = 0;
		float maxTime = 1;
	}
	else
	{
		minTime = patternTime - pattern->toNormalizedTime(absMatchRange_before);
		maxTime = patternTime + pattern->toNormalizedTime(absMatchRange_after);

		if (minTime < 0)
		{
			minTime += 1;
		}
		if (maxTime > 1)
		{
			maxTime -= 1;
		}
	}

	int lowerBound;
	int upperBound;
	pattern->getBoundingKeyframes(minTime, maxTime, lowerBound, upperBound);

	ASSERT(lowerBound >= 0 && lowerBound < pattern->getNumKeyframes(),);
	ASSERT(upperBound >= 0 && upperBound < pattern->getNumKeyframes(),);

	comparingSample = BreathSample::ClosestValues_ptr(
		samplerSample,
		pattern->getSamples() + lowerBound,
		upperBound - lowerBound + 1);
}

bool SyncedPatternCorrelator::isHolding() const
{
	return _holdTime >= 0;
}

bool SyncedPatternCorrelator::_setComparingSample_HoldCheck(float delta)
{
	int holdKeyIndex = pattern->getKeyframeIndex(patternTime);
	
	if (pattern->getKeyframes()[holdKeyIndex].method != Interpolators::Hold)
	{
		ASSERT(_holdTime < 0, false);
		return false;
	}

	int nextKeyIndex = (holdKeyIndex + 1) % pattern->getNumKeyframes();

	float holdLikeness = BreathSample::Likeness(
		sampler->getSample(),
		pattern->getKeyframes()[holdKeyIndex].sample);

	if (holdLikeness < holdThreshold)
	{
		comparingSample = pattern->getKeyframes()[nextKeyIndex].sample;
		patternTime = pattern->getKeyframes()[nextKeyIndex].time;
		_holdTime = -1;
		return true;
	}

	float nextKeyLikeness = BreathSample::Likeness(
		sampler->getSample(),
		pattern->getKeyframes()[nextKeyIndex].sample);

	if (nextKeyLikeness >= holdLikeness)
	{
		comparingSample = pattern->getKeyframes()[nextKeyIndex].sample;
		patternTime = pattern->getKeyframes()[nextKeyIndex].time;
		_holdTime = -1;
		return true;
	}

	if (_holdTime < 0)
		_holdTime = 0;

	_holdTime += delta;
	comparingSample = pattern->getKeyframes()[holdKeyIndex].sample;
	return true;
}

void SyncedPatternCorrelator::setPatternTimeByMode(float delta)
{
	if (isHolding())
		return;

	switch (progressMode)
	{
	case ProgressMode::Constant:
		patternTime += delta / pattern->getLength();
		break;
	case ProgressMode::Wait:
		patternTime += comparingCorrelation * delta / pattern->getLength();
		break;

	default:
		EDebug::Error("SyncedPatternCorrelator::setPatternTimeByMode() - Invalid mode");
		return;
	}

	while (patternTime > 1)
		patternTime -= 1;
}

void SyncedPatternCorrelator::update(float delta)
{
	pattern = pattern_ref.get();
	sampler = sampler_ref.get();

	if (sampler == nullptr || pattern == nullptr)
	{
		EDebug::Error("SyncedPatternCorrelator::update() - Sampler or pattern is null");
		return;
	}

	if (delta >= 1)
	{
		EDebug::Warning("SyncedPatternCorrelator::update() - The amount of time that passed before the last update is very large. This may cause the pattern time to skip ahead more than it should for matching the sampler. Consider changing the update method to prevent a large delta being passed to this method.");
	}

#if NDEBUG
	static bool printedAbsValueWarning = false;
	if (absMatchRange_after + absMatchRange_before >= pattern->getLength() && !printedAbsValueWarning)
	{
		EDebug::Warning("SyncedPatternCorrelator::update() - The absolute match range (bef: %.2f, aft: %.2f) is larger than the pattern length (%.2f). This means that the pattern will always match the sampler, as long as the sampler matches any point in the pattern. If this is intended, consider using a RangeCorrelator instead.", absMatchRange_before, absMatchRange_after, pattern->getLength());
		printedAbsValueWarning = true;
	}
#endif

	BreathSample samplerSample = sampler->getSample();
	setComparingSample(samplerSample, delta);

	comparingCorrelation = BreathSample::Likeness(samplerSample, comparingSample);
}

void SyncedPatternCorrelator::addParameterDefinition(SerializedTypes::ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);

	definition->addReferenceDefinition(
		"Breath Sampler",
		"Sampler to use for the correlator. The pattern will try to match it's own time based on the breath sampler.",
		SerializedTypes::REF_Sampler);

	definition->addReferenceDefinition(
		"Pattern",
		"Pattern to use for the correlator.",
		SerializedTypes::REF_Pattern);

	definition->addEnumDefinition(
		"Pattern Time Mode",
		"Determines how the pattern time will be updated.",
		(UInt8)ProgressMode::Constant,
		SyncedPatternCorrelator_MatchMode_serializedIndex);

	definition->addFloatDefinition(
		"Pattern Start Time|Pattern Time",
		"Normalized time of the pattern to use for the correlator. When not playing, this is the time the pattern will start at.",
		0.0f,
		0.0f,
		1.0f,
		SerializedTypes::PAR_ReadOnlyWhilePlaying);

	definition->addFloatDefinition(
		"Match Seconds Before",
		"The number of seconds back that the pattern will go to find the best match to the breath sampler.",
		0.0f,
		0.0f,
		MAXED_FLOAT);

	definition->addFloatDefinition(
		"Match Seconds After",
		"The number of seconds forward that the pattern will go to find the best match to the breath sampler.",
		0.0f,
		0.0f,
		MAXED_FLOAT);

	definition->addFloatDefinition(
		"Hold Threshold",
		"The threshold, in likeness, which a pattern hold will be maintained, given that Next Threshold is not met.",
		0.65f,
		0.0f,
		1.0f);

	definition->addFloatDefinition(
		"Next Threshold",
		"The threshold, in likeness, which a pattern must match in order to exit a hold and move to the next keyframe. ",
		0.65f,
		0.0f,
		1.0f);
}

void SyncedPatternCorrelator::setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references)
{
	Behaviour::setParameterIndex(paramIndex, data, references);

	SetReference(sampler_ref);
	SetReference(pattern_ref);
	SetFloat(patternTime);
	SetFloat(absMatchRange_before);
	SetFloat(absMatchRange_after);
	SetFloat(holdThreshold);
	SetFloat(nextThreshold);
}

float SyncedPatternCorrelator::getCorrelation()
{
	demandUpdate();
	return comparingCorrelation;
}

BreathSample SyncedPatternCorrelator::getSample()
{
	demandUpdate();
	return comparingSample;
}
