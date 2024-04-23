#ifndef TMAUDIOSAMPLER_H
#define TMAUDIOSAMPLER_H

#include "..\serialization\Serializable.hpp"
#include "..\types\Behaviour.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\types\BreathSample.hpp"
#include "..\serialization\ReferenceManager.hpp"
#include "tensorflow\lite\c\c_api.h"

class TMAudioSampler : public Behaviour, public IBreathSampler
{
private:
	DataStreamRef audioStream{};
	float inferenceInterval{};

	BreathSample lastSample{};
	float lastInference{};

	unsigned char* modelData{};
	UInt32 modelDataSize{};

	TfLiteInterpreter* tf_interpreter{};
	TfLiteModel* tf_model{};
	TfLiteInterpreterOptions* tf_options{};
	TfLiteTensor* inputTensor{};

	void CreateInterpreter();
	void DestroyInterpreter();
	void ExecuteInference();

protected:
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

public:
	SerializableProperties(
		TMAudioSampler,
		"Teachable Machine Audio",
		"Uses basic inference to detect breaths using the volume of the audio stream",
		ReferenceType::REF_Sampler | ReferenceType::REF_Behaviour)

	~TMAudioSampler();
	static void addParameterDefinition(ClassDefinition *definition);

	void update(float delta) override;
	BreathSample Sample() override;
};

#endif // TMAUDIOSAMPLER_H