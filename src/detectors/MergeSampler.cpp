#include "MergeSampler.hpp"

CreateSerializable(MergeSampler);

#define SetSampleValue(name) \
	if (use_a_##name && use_b_##name) \
		currentSample.name = a.name * (1.0f - blend_weight) + b.name * blend_weight; \
	else if (use_a_##name) \
		currentSample.name = a.name; \
	else if (use_b_##name) \
		currentSample.name = b.name; \
	else currentSample.name = -1.0f;

void MergeSampler::update(float delta)
{
	// EDebug::Log("MergeSampler::update() - delta: %.2f", delta);

	while (true)
	{
		
	}

	BreathSample a = sampler_a.Sample();
	BreathSample b = sampler_b.Sample();
	
	SetSampleValue(yes);
	SetSampleValue(in);
	SetSampleValue(nasal);
	SetSampleValue(volume);
	SetSampleValue(pitch);
}

BreathSample MergeSampler::Sample()
{
	demandUpdate();
	return currentSample;
}

void MergeSampler::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);
	
	definition->addFloatDefinition("Blend Weight", "Weight for blending the two samples. 0.0 => only A, 0.5 => 50/50, 0.8 => 20/80, 1.0 => only B.", 0.0f, 0.0f, 1.0f);

	definition->addReferenceDefinition("Sampler A", "One of the samplers (object that has breath samples).", ReferenceType::REF_Sampler);

	definition->addBoolDefinition("Use A Yes", "Use the 'yes' value in the first sampler.", true);

	definition->addBoolDefinition("Use A In", "Use the 'in' value in the first sampler.", true);

	definition->addBoolDefinition("Use A Nasal", "Use the 'nasal' value in the first sampler.", true);

	definition->addBoolDefinition("Use A Volume", "Use the 'volume' value in the first sampler.", true);

	definition->addBoolDefinition("Use A Pitch", "Use the 'pitch' value in the first sampler.", true);

	definition->addReferenceDefinition("Sampler B", "One of the samplers (object that has breath samples).", ReferenceType::REF_Sampler);

	definition->addBoolDefinition("Use B Yes", "Use the 'yes' value in the second sampler.", true);

	definition->addBoolDefinition("Use B In", "Use the 'in' value in the second sampler.", true);

	definition->addBoolDefinition("Use B Nasal", "Use the 'nasal' value in the second sampler.", true);

	definition->addBoolDefinition("Use B Volume", "Use the 'volume' value in the second sampler.", true);

	definition->addBoolDefinition("Use B Pitch", "Use the 'pitch' value in the second sampler.", true);
}

void MergeSampler::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	Behaviour::setParameterIndex(paramIndex, savedData, runtimeData);

	serializable_SetFloat(blend_weight);

	serializable_SetReference(sampler_a);
	serializable_SetBool(use_a_yes);
	serializable_SetBool(use_a_in);
	serializable_SetBool(use_a_nasal);
	serializable_SetBool(use_a_volume);
	serializable_SetBool(use_a_pitch);

	serializable_SetReference(sampler_b);
	serializable_SetBool(use_b_yes);
	serializable_SetBool(use_b_in);
	serializable_SetBool(use_b_nasal);
	serializable_SetBool(use_b_volume);
	serializable_SetBool(use_b_pitch);
}