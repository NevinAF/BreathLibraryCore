#ifndef SAMPLECORRELATOR_H
#define SAMPLECORRELATOR_H

#include "..\serialization\Serializable.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\serialization\ReferenceManager.hpp"

class SampleCorrelator : public Serializable, public ICorrelator
{
private:
	SamplerRef sampler_a{};
	SamplerRef sampler_b{};

protected:
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

public:
	SerializableProperties(
		SampleCorrelator,
		"Sample Correlator",
		"Returns the correlation between two samplers. If the samples are close to each other, the correlation will be high.",
		ReferenceType::REF_Correlator)

	static void addParameterDefinition(ClassDefinition *definition);

	float Correlation() override;
};

#endif // SAMPLECORRELATOR_H