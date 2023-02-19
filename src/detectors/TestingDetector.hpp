#pragma once

#include "..\serialization\Serializable.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\serialization\ClassDefinitions.hpp"
#include "..\types\Behaviour.hpp"

class TestingDetector : public IBreathSampler, public Serializable, Behaviour
{
private:
	float currentTime{};

protected:
	void setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references) override;

public:
	SerializableProperties(
		TestingDetector,
		TestingDetector_serializedIndex,
		"Testing Detector",
		"Testing detector which returns specific values based on current time. Used for testing.",
		SerializedTypes::REF_Sampler)

	static void addParameterDefinition(SerializedTypes::ClassDefinition *definition);

	void update(float delta) override;
	BreathSample getSample() override;
};


