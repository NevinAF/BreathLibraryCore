
#include "RangeCorrelator.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\util\Timer.hpp"

CreateSerializable(RangeCorrelator);

float RangeCorrelator::Correlation()
{
	updateSample();

	return lastCorrelation;
}

BreathSample RangeCorrelator::Sample()
{
	updateSample();

	return lastSample;
}

void RangeCorrelator::updateSample()
{
	double currentTime = Timer::getTime();
	if (lastUpdate > currentTime - 0.02)
		return;
	lastUpdate = currentTime;

	BreathSample samplerSample = sampler.Sample();

	if (useYes && samplerSample.hasYes())
	{
		if (samplerSample.getYes() < yesLower)
			lastSample.setYes(yesLower);
		else if (samplerSample.getYes() > yesUpper)
			lastSample.setYes(yesUpper);
		else
			lastSample.setYes(samplerSample.getYes());
	}
	else lastSample.setYes(-1.0f);

	if (useIn && samplerSample.hasIn())
	{
		if (samplerSample.getIn() < inLower)
			lastSample.setIn(inLower);
		else if (samplerSample.getIn() > inUpper)
			lastSample.setIn(inUpper);
		else
			lastSample.setIn(samplerSample.getIn());
	}
	else lastSample.setIn(-1.0f);

	if (useNasal && samplerSample.hasNasal())
	{
		if (samplerSample.getNasal() < nasalLower)
			lastSample.setNasal(nasalLower);
		else if (samplerSample.getNasal() > nasalUpper)
			lastSample.setNasal(nasalUpper);
		else
			lastSample.setNasal(samplerSample.getNasal());
	}
	else lastSample.setNasal(-1.0f);

	if (useVolume && samplerSample.hasVolume())
	{
		if (samplerSample.getVolume() < volumeLower)
			lastSample.setVolume(volumeLower);
		else if (samplerSample.getVolume() > volumeUpper)
			lastSample.setVolume(volumeUpper);
		else
			lastSample.setVolume(samplerSample.getVolume());
	}
	else lastSample.setVolume(-1.0f);

	if (usePitch && samplerSample.hasPitch())
	{
		if (samplerSample.getPitch() < pitchLower)
			lastSample.setPitch(pitchLower);
		else if (samplerSample.getPitch() > pitchUpper)
			lastSample.setPitch(pitchUpper);
		else
			lastSample.setPitch(samplerSample.getPitch());
	}
	else lastSample.setPitch(-1.0f);

	lastCorrelation = BreathSample::Likeness(lastSample, samplerSample);
}

void RangeCorrelator::addParameterDefinition(ClassDefinition *definition)
{
	definition->addReferenceDefinition("Sampler", "The sampler to correlate to the ranges.", ReferenceType::REF_Sampler);
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

void RangeCorrelator::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	serializable_SetReference(sampler);
	serializable_SetBool(useYes);
	serializable_SetFloat(yesLower);
	serializable_SetFloat(yesUpper);
	serializable_SetBool(useIn);
	serializable_SetFloat(inLower);
	serializable_SetFloat(inUpper);
	serializable_SetBool(useNasal);
	serializable_SetFloat(nasalLower);
	serializable_SetFloat(nasalUpper);
	serializable_SetBool(useVolume);
	serializable_SetFloat(volumeLower);
	serializable_SetFloat(volumeUpper);
	serializable_SetBool(usePitch);
	serializable_SetFloat(pitchLower);
	serializable_SetFloat(pitchUpper);
}