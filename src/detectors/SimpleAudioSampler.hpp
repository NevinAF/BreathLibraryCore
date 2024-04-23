#ifndef SIMPLEAUDIOSAMPLER_H
#define SIMPLEAUDIOSAMPLER_H

#include "..\serialization\Serializable.hpp"
#include "..\types\Behaviour.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\types\BreathSample.hpp"
#include "..\serialization\ReferenceManager.hpp"

class SimpleAudioSampler : public Behaviour, public IBreathSampler
{
private:
	DataStreamRef audioStream{};
	BreathSample lastSample{};

	float min_lowpass{ 0.0f };
	float max_lowpass{ 0.0f };

protected:
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

public:
	SerializableProperties(
		SimpleAudioSampler,
		"Simple Audio Sampler",
		"Uses basic inference to detect breaths using the volume of the audio stream",
		ReferenceType::REF_Sampler | ReferenceType::REF_Behaviour)

	static void addParameterDefinition(ClassDefinition *definition);

	void update(float delta) override;
	BreathSample Sample() override;
};

#endif // SIMPLEAUDIOSAMPLER_H