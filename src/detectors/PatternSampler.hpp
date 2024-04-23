#ifndef PATTERNSAMPLER_H
#define PATTERNSAMPLER_H

#include "..\serialization\Serializable.hpp"
#include "..\types\Behaviour.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\types\Pattern.hpp"
#include "..\types\BreathSample.hpp"
#include "..\serialization\ReferenceManager.hpp"

class PatternSampler : public Behaviour, public IBreathSampler
{
private:
	float currentTime{0};
	PatternRef pattern{};

protected:
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

public:
	SerializableProperties(
		PatternSampler,
		"Pattern Sampler",
		"Turns a pattern into a sampler. This is the simple version of the Synced Pattern Sampler, where the progression is alway constant and has no holds.",
		ReferenceType::REF_Sampler | ReferenceType::REF_Behaviour)

	static void addParameterDefinition(ClassDefinition *definition);


	void update(float delta) override;
	BreathSample Sample() override;
};

#endif // PATTERNSAMPLER_H