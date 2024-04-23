#ifndef RANGE_CORRELATOR_H
#define RANGE_CORRELATOR_H

#include "..\serialization\Serializable.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\serialization\ReferenceManager.hpp"

class RangeCorrelator : public Serializable, public ICorrelator, public IBreathSampler
{
private:
	SamplerRef sampler;

	bool useYes{true};
	float yesLower{0};
	float yesUpper{0};

	bool useIn{true};
	float inLower{0};
	float inUpper{0};

	bool useNasal{true};
	float nasalLower{0};
	float nasalUpper{0};

	bool useVolume{true};
	float volumeLower{0};
	float volumeUpper{0};

	bool usePitch{true};
	float pitchLower{0};
	float pitchUpper{0};

	double lastUpdate{0};
	BreathSample lastSample{};
	float lastCorrelation{0};

	void updateSample();

protected:
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

public:
	SerializableProperties(
		RangeCorrelator,
		"Range Correlator",
		"Correlates a sampler based on if its values are within a range. Good for checking specific static values, such as user is breathing in.",
		ReferenceType::REF_Correlator | ReferenceType::REF_Sampler)

	static void addParameterDefinition(ClassDefinition *definition);

	float Correlation() override;
	BreathSample Sample() override;
};

#endif // RANGE_CORRELATOR_H