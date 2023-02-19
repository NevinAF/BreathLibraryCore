#include "pch.h"
#include "PatternSampler.hpp"
#include "..\debugging\DebugCallbacks.hpp"

using namespace SerializedTypes;

CreateSerializable(PatternSampler);

void PatternSampler::update(float delta)
{
	Pattern *p = pattern.get();

	if (p == nullptr)
	{
		EDebug::Warning("PatternSampler::m_update() - Pattern is null");
		return;
	}

	float _time = currentTime;

	currentTime += delta;

	float len = p->getLength();
	if (currentTime >= len)
	{
		currentTime -= len;
	}
}

BreathSample PatternSampler::getSample()
{
	Pattern *p = pattern.get();

	if (p == nullptr)
	{
		EDebug::Warning("PatternSampler::m_getSample() - Pattern is null");
		return BreathSample(0, 0, 0, 0, 0);
	}
	return p->getTarget(p->toNormalizedTime(currentTime));
}

void PatternSampler::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);

	definition->addFloatDefinition("Start Time|Current Time", "Normalized time of the testing detector", 0.0f, 0.0f, 1.0f);
	definition->addReferenceDefinition("Pattern", "Pattern to use for the sampler", SerializedTypes::REF_Pattern);
}

void PatternSampler::setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references)
{
	Behaviour::setParameterIndex(paramIndex, data, references);

	SetReference(pattern);
	SetFloat(currentTime);
}