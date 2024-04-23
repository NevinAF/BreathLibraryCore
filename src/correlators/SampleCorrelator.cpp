
#include "SampleCorrelator.hpp"
#include "..\debugging\DebugCallbacks.hpp"

CreateSerializable(SampleCorrelator);

float SampleCorrelator::Correlation()
{
	return BreathSample::Likeness(
		sampler_a.Sample(),
		sampler_b.Sample());
}

void SampleCorrelator::addParameterDefinition(ClassDefinition *definition)
{
	definition->addReferenceDefinition("Sampler A", "One of the samplers (object that has breath samples).", ReferenceType::REF_Sampler);

	definition->addReferenceDefinition("Sampler B", "One of the samplers (object that has breath samples).", ReferenceType::REF_Sampler);
}

void SampleCorrelator::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	serializable_SetReference(sampler_a);
	serializable_SetReference(sampler_b);
}