#pragma once

#include "..\serialization\Serializable.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\serialization\ClassDefinitions.hpp"
#include "..\types\Behaviour.hpp"

class MergeSampler : public IBreathSampler, public Serializable, Behaviour
{
private:
	IBreathSampler *samplers{nullptr};

	BreathSample currentSample{};

protected:
	void setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references) override;

public:
	SerializableProperties(
		MergeSampler,
		MergerSampler_serializedIndex,
		"Merge Sampler",
		"Takes one or more samplers and merges them into one. Useful for combining multiple detectors into one with all of the best features.",
		SerializedTypes::REF_Sampler)

	static void addParameterDefinition(SerializedTypes::ClassDefinition *definition);

	void update(float delta) override;
	BreathSample getSample() override;
};


