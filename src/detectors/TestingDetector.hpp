#ifndef TESTING_DETECTOR_HPP
#define TESTING_DETECTOR_HPP

#include "..\serialization\Serializable.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\types\Behaviour.hpp"

class TestingDetector : public Behaviour, public IBreathSampler
{
private:
	float currentTime{};

protected:
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

public:
	SerializableProperties(
		TestingDetector,
		"Testing Detector",
		"Testing detector which returns specific values based on current time. Used for testing.",
		ReferenceType::REF_Sampler | ReferenceType::REF_Behaviour)

	static void addParameterDefinition(ClassDefinition *definition);

	virtual ~TestingDetector() = default;
	void update(float delta) override;
	BreathSample Sample() override;
};

#endif // TESTING_DETECTOR_HPP