#include "pch.h"

#include "RangeCorrelator.hpp"

CreateSerializable(RangeCorrelator);

float RangeCorrelator::getCorrelation()
{
	BreathSample rangeSample;
	BreathSample samplerSample = sampler.get()->getSample();

	if (useYes && samplerSample.hasYes())
	{
		if (samplerSample.getYes() < yesLower)
			rangeSample.setYes(yesLower);
		else if (samplerSample.getYes() > yesUpper)
			rangeSample.setYes(yesUpper);
		else
			rangeSample.setYes(samplerSample.getYes());
	}

	if (useIn && samplerSample.hasIn())
	{
		if (samplerSample.getIn() < inLower)
			rangeSample.setIn(inLower);
		else if (samplerSample.getIn() > inUpper)
			rangeSample.setIn(inUpper);
		else
			rangeSample.setIn(samplerSample.getIn());
	}

	if (useNasal && samplerSample.hasNasal())
	{
		if (samplerSample.getNasal() < nasalLower)
			rangeSample.setNasal(nasalLower);
		else if (samplerSample.getNasal() > nasalUpper)
			rangeSample.setNasal(nasalUpper);
		else
			rangeSample.setNasal(samplerSample.getNasal());
	}

	if (useVolume && samplerSample.hasVolume())
	{
		if (samplerSample.getVolume() < volumeLower)
			rangeSample.setVolume(volumeLower);
		else if (samplerSample.getVolume() > volumeUpper)
			rangeSample.setVolume(volumeUpper);
		else
			rangeSample.setVolume(samplerSample.getVolume());
	}

	if (usePitch && samplerSample.hasPitch())
	{
		if (samplerSample.getPitch() < pitchLower)
			rangeSample.setPitch(pitchLower);
		else if (samplerSample.getPitch() > pitchUpper)
			rangeSample.setPitch(pitchUpper);
		else
			rangeSample.setPitch(samplerSample.getPitch());
	}

	return BreathSample::Likeness(rangeSample, samplerSample);
}

void RangeCorrelator::addParameterDefinition(SerializedTypes::ClassDefinition *definition)
{
	definition->addReferenceDefinition("Sampler", "The sampler to correlate to the ranges.", SerializedTypes::REF_Sampler);
	definition->addBoolDefinition("Use Yes", "Whether to use the yes range.", true);
	definition->addFloatDefinition("Yes Lower", "The lower bound of the yes range.", 0.0f, 0.0f, 1.0f);
	definition->addFloatDefinition("Yes Upper", "The upper bound of the yes range.", 1.0f, 0.0f, 1.0f);
	definition->addBoolDefinition("Use In", "Whether to use the in range.", true);
	definition->addFloatDefinition("In Lower", "The lower bound of the in range.", 0.0f, 0.0f, 1.0f);
	definition->addFloatDefinition("In Upper", "The upper bound of the in range.", 1.0f, 0.0f, 1.0f);
	definition->addBoolDefinition("Use Nasal", "Whether to use the nasal range.", false);
	definition->addFloatDefinition("Nasal Lower", "The lower bound of the nasal range.", 0.0f, 0.0f, 1.0f);
	definition->addFloatDefinition("Nasal Upper", "The upper bound of the nasal range.", 1.0f, 0.0f, 1.0f);
	definition->addBoolDefinition("Use Volume", "Whether to use the volume range.", false);
	definition->addFloatDefinition("Volume Lower", "The lower bound of the volume range.", 0.0f, 0.0f, 1.0f);
	definition->addFloatDefinition("Volume Upper", "The upper bound of the volume range.", 1.0f, 0.0f, 1.0f);
	definition->addBoolDefinition("Use Pitch", "Whether to use the pitch range.", false);
	definition->addFloatDefinition("Pitch Lower", "The lower bound of the pitch range.", 0.0f, 0.0f, 1.0f);
	definition->addFloatDefinition("Pitch Upper", "The upper bound of the pitch range.", 1.0f, 0.0f, 1.0f);
}

void RangeCorrelator::setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references)
{
	SetReference(sampler);
	SetBool(useYes);
	SetFloat(yesLower);
	SetFloat(yesUpper);
	SetBool(useIn);
	SetFloat(inLower);
	SetFloat(inUpper);
	SetBool(useNasal);
	SetFloat(nasalLower);
	SetFloat(nasalUpper);
	SetBool(useVolume);
	SetFloat(volumeLower);
	SetFloat(volumeUpper);
	SetBool(usePitch);
	SetFloat(pitchLower);
	SetFloat(pitchUpper);
}