
#include "SyncedPatternCorrelator.hpp"
#include "..\debugging\DebugCallbacks.hpp"

CreateSerializable(SyncedPatternCorrelator);

void SyncedPatternCorrelator::setComparingSample(const BreathSample& samplerSample, float delta)
{
	if (pattern->getNumKeyframes() == 0)
	{
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
		minTime = 0;
		maxTime = 1;
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

	comparingSample = pattern->getClosestSample(&samplerSample, minTime, maxTime);
}

bool SyncedPatternCorrelator::isHolding() const
{
	return _holdTime >= 0;
}

bool SyncedPatternCorrelator::_setComparingSample_HoldCheck(float delta)
{
	int holdKeyIndex = pattern->getKeyframeIndexAtTime(patternTime);
	
	// EDebug::Log("Got key: %d", holdKeyIndex);


	if (pattern->getKeyframes()[holdKeyIndex].method != Interpolators::Hold)
	{
		ASSERT(_holdTime < 0, false);
		return false;
	}

	int nextKeyIndex = (holdKeyIndex + 1) % pattern->getNumKeyframes();

	float holdLikeness = BreathSample::Likeness(
		sampler.Sample(),
		pattern->getKeyframes()[holdKeyIndex].sample);

	if (holdLikeness < holdThreshold)
	{
		comparingSample = pattern->getKeyframes()[nextKeyIndex].sample;
		patternTime = pattern->getKeyframes()[nextKeyIndex].time;
		_holdTime = -1;
		return true;
	}

	float nextKeyLikeness = BreathSample::Likeness(
		sampler.Sample(),
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

	ASSERT_MSG(sampler.id != 0xFFFFFFFF,, "SyncedPatternCorrelator::update: sampler is null!");
	ASSERT_MSG(pattern != nullptr,, "SyncedPatternCorrelator::update: pattern is null!");

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


	BreathSample samplerSample = sampler.Sample();

	setComparingSample(samplerSample, delta);

	comparingCorrelation = BreathSample::Likeness(samplerSample, comparingSample);

	setPatternTimeByMode(delta);
}

void SyncedPatternCorrelator::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);

	definition->addReferenceDefinition(
		"Breath Sampler",
		"Sampler to use for the correlator. The pattern will try to match it's own time based on the breath sampler.",
		ReferenceType::REF_Sampler);

	definition->addReferenceDefinition(
		"Pattern",
		"Pattern to use for the correlator.",
		ReferenceType::REF_Pattern);

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
		ParameterFlags::PAR_ReadOnlyWhilePlaying);

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

void SyncedPatternCorrelator::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	Behaviour::setParameterIndex(paramIndex, savedData, runtimeData);

	serializable_SetReference(sampler);
	serializable_SetReference(pattern_ref);
	serializable_SetEnum(progressMode);
	serializable_SetFloat(patternTime);
	serializable_SetFloat(absMatchRange_before);
	serializable_SetFloat(absMatchRange_after);
	serializable_SetFloat(holdThreshold);
	serializable_SetFloat(nextThreshold);
}

float SyncedPatternCorrelator::Correlation()
{
	demandUpdate();
	return comparingCorrelation;
}

BreathSample SyncedPatternCorrelator::Sample()
{
	demandUpdate();
	return comparingSample;
}
