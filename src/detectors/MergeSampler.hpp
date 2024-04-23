#ifndef MERGESAMPLER_H
#define MERGESAMPLER_H

#include "..\serialization\Serializable.hpp"
#include "..\serialization\ReferenceManager.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\types\Behaviour.hpp"

class MergeSampler : public Behaviour, public IBreathSampler
{
private:
	float blend_weight{0.5f};
	
	SamplerRef sampler_a{};
	bool use_a_yes{true};
	bool use_a_in{true};
	bool use_a_nasal{true};
	bool use_a_volume{true};
	bool use_a_pitch{true};

	SamplerRef sampler_b{};
	bool use_b_yes{true};
	bool use_b_in{true};
	bool use_b_nasal{true};
	bool use_b_volume{true};
	bool use_b_pitch{true};

	BreathSample currentSample{};

protected:
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

public:
	SerializableProperties(
		MergeSampler,
		"Merge Sampler",
		"Takes one or more samplers and merges them into one. Useful for combining multiple detectors into one with all of the best features.",
		ReferenceType::REF_Sampler | ReferenceType::REF_Behaviour)

	static void addParameterDefinition(ClassDefinition *definition);

	void update(float delta) override;
	BreathSample Sample() override;
};

#endif // MERGESAMPLER_H