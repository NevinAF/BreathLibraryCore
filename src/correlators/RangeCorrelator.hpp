#include "..\serialization\Serializable.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\serialization\ClassDefinitions.hpp"
#include "..\serialization\ReferenceManager.hpp"

class RangeCorrelator : public ICorrelator, public Serializable
{
private:
	Ref<IBreathSampler> sampler;

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

protected:
	void setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references) override;

public:
	SerializableProperties(
		RangeCorrelator,
		RangeCorrelator_serializedIndex,
		"Range Correlator",
		"Correlates a sampler based on if its values are within a range. Good for checking specific static values, such as user is breathing in.",
		SerializedTypes::REF_Correlator)

	static void addParameterDefinition(SerializedTypes::ClassDefinition *definition);

	float getCorrelation() override;
};