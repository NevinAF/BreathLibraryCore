#pragma once

#include "..\serialization\Serializable.hpp"
#include "..\serialization\ClassDefinitions.hpp"
#include "..\types\Behaviour.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\types\Pattern.hpp"
#include "..\types\BreathSample.hpp"
#include "..\serialization\ReferenceManager.hpp"

class PatternSampler : public IBreathSampler, public Serializable, Behaviour
{
private:
	float currentTime{0};
	Ref<Pattern> pattern{};

protected:
	void setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references) override;

public:
	SerializableProperties(
		PatternSampler,
		PatternSampler_serializedIndex,
		"Pattern Sampler",
		"Turns a pattern into a sampler. This is the simple version of the Synced Pattern Sampler, where the progression is alway constant and has no holds.",
		SerializedTypes::REF_Sampler)

	static void addParameterDefinition(SerializedTypes::ClassDefinition *definition);


	void update(float delta) override;
	BreathSample getSample() override;
};


