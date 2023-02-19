#include "pch.h"

#include "SampleCorrelator.hpp"

CreateSerializable(SampleCorrelator);

float SampleCorrelator::getCorrelation()
{
	return BreathSample::Likeness(sampler_a.get()->getSample(), sampler_b.get()->getSample());
}

void SampleCorrelator::addParameterDefinition(SerializedTypes::ClassDefinition *definition)
{
	definition->addReferenceDefinition("Sampler A", "One of the samplers (object that has breath samples).", SerializedTypes::REF_Sampler);

	definition->addReferenceDefinition("Sampler B", "One of the samplers (object that has breath samples).", SerializedTypes::REF_Sampler);
}

void SampleCorrelator::setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references)
{
	SetReference(sampler_a);
	SetReference(sampler_b);
}