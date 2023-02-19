#pragma once

#include "..\serialization\Serializable.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\serialization\ClassDefinitions.hpp"
#include "..\serialization\ReferenceManager.hpp"

class SampleCorrelator : public ICorrelator, public Serializable
{
private:
	Ref<IBreathSampler> sampler_a{};
	Ref<IBreathSampler> sampler_b{};

protected:
	void setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references) override;

public:
	SerializableProperties(
		SampleCorrelator,
		SampleCorrelator_serializedIndex,
		"Sample Correlator",
		"Returns the correlation between two samplers. If the samples are close to each other, the correlation will be high.",
		SerializedTypes::REF_Correlator)

	static void addParameterDefinition(SerializedTypes::ClassDefinition *definition);

	float getCorrelation() override;
};