#include "PatternSampler.hpp"
#include "..\debugging\DebugCallbacks.hpp"

CreateSerializable(PatternSampler);

void PatternSampler::update(float delta)
{
	// EDebug::Log("PatternSampler::update() - delta: %.2f", delta);
	Pattern *p = pattern.get();

	if (p == nullptr)
	{
		EDebug::Warning("PatternSampler::m_update() - Pattern is null");
		return;
	}

	currentTime += delta;

	float len = p->getLength();
	while (currentTime >= len)
	{
		currentTime -= len;
	}
}

BreathSample PatternSampler::Sample()
{
	Pattern *p = pattern.get();

	if (p == nullptr)
	{
		EDebug::Warning("PatternSampler::m_getSample() - Pattern is null");
		return BreathSample(0, 0, 0, 0, 0);
	}
	BreathSample sample = p->getTarget(p->toNormalizedTime(currentTime));
	return sample;
}

void PatternSampler::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);

	definition->addFloatDefinition("Start Time|Current Time", "Normalized time of the testing detector", 0.0f, 0.0f, 1.0f);
	definition->addReferenceDefinition("Pattern", "Pattern to use for the sampler", ReferenceType::REF_Pattern);
}

void PatternSampler::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	Behaviour::setParameterIndex(paramIndex, savedData, runtimeData);

	serializable_SetReference(pattern);
	serializable_SetFloat(currentTime);
}