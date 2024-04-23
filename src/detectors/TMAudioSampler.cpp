
#include "TMAudioSampler.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\types\DataStreamMeta.hpp"

CreateSerializable(TMAudioSampler);

TMAudioSampler::~TMAudioSampler()
{
	DestroyInterpreter();
}

void TMAudioSampler::update(float delta)
{
	lastInference += delta;

	if (lastInference >= inferenceInterval)
	{
		ExecuteInference();
		lastInference = 0.0f;
	}
}

BreathSample TMAudioSampler::Sample()
{
	Behaviour::demandUpdate();
	return lastSample;
}

void TMAudioSampler::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);

	definition->addReferenceDefinition("Audio Stream", "A IDataStream to read audio input from", ReferenceType::REF_DataStream);
	definition->addFileDefinition("Model File", "The .tflite model file to use for inference", "tflite", PAR_ReadOnlyWhilePlaying);
}

void TMAudioSampler::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	Behaviour::setParameterIndex(paramIndex, savedData, runtimeData);

	serializable_SetReference(audioStream);

	if (paramIndex-- == 0)
	{
		Serializable::setArray_RuntimeBytes(modelData, modelDataSize, runtimeData);
		CreateInterpreter();
	}
}

void TMAudioSampler::CreateInterpreter()
{
	if (tf_interpreter != nullptr)
	{
		EDebug::Warning("TMAudioSampler::CreateInterpreter() - interpreter already exists, destroying it. This should only ever happen manually if testing different models from within an editor");
	}

	tf_model = TfLiteModelCreate(modelData, modelDataSize);
	ASSERT_MSG(tf_model != nullptr,, "TMAudioSampler::CreateInterpreter() - model is null");

	tf_options = TfLiteInterpreterOptionsCreate();
	ASSERT_MSG(tf_options != nullptr,, "TMAudioSampler::CreateInterpreter() - options is null");

	// TODO does this need to be set to 1?
	TfLiteInterpreterOptionsSetNumThreads(tf_options, 1);

	tf_interpreter = TfLiteInterpreterCreate(tf_model, tf_options);
	ASSERT_MSG(tf_interpreter != nullptr,, "TMAudioSampler::CreateInterpreter() - interpreter is null");

	TfLiteInterpreterAllocateTensors(tf_interpreter);

	inputTensor = TfLiteInterpreterGetInputTensor(tf_interpreter, 0);
	ASSERT_MSG(inputTensor != nullptr,, "TMAudioSampler::CreateInterpreter() - inputTensor is null");
}

void TMAudioSampler::DestroyInterpreter()
{
	if (tf_interpreter != nullptr)
	{
		TfLiteInterpreterDelete(tf_interpreter);
		tf_interpreter = nullptr;
	}

	if (tf_model != nullptr)
	{
		TfLiteModelDelete(tf_model);
		tf_model = nullptr;
	}

	if (tf_options != nullptr)
	{
		TfLiteInterpreterOptionsDelete(tf_options);
		tf_options = nullptr;
	}
}

void TMAudioSampler::ExecuteInference()
{
	DataStreamMeta streamData = audioStream.getStreamData();

	ASSERT_MSG(streamData.data != nullptr,, "TMAudioSampler::update() - streamData.data is null");
	ASSERT_MSG(streamData.capacity > 0,, "TMAudioSampler::update() - streamData.capacity is 0");
	ASSERT_MSG(streamData.putIndex < streamData.capacity,, "TMAudioSampler::update() - streamData.putIndex is out of bounds");

	float* data = (float*)streamData.data;
	size_t readSize = TfLiteTensorByteSize(inputTensor) / sizeof(float);

	if (readSize > streamData.capacity)
	{
		EDebug::Warning("TMAudioSampler::update() - input size of model is larger than the stream capacity, this will cause issues with inference");
	}

	// "data" is circular, so we need to copy it into a temporary buffer
	// TODO this is a bit of a hack, but it works for now
	float* tempBuffer = new float[readSize];
	for (size_t i = 0; i < readSize; i++)
	{
		tempBuffer[i] = data[(streamData.putIndex + i) % streamData.capacity];
	}

	TfLiteTensorCopyFromBuffer(inputTensor, tempBuffer, readSize * sizeof(float));

	TfLiteInterpreterInvoke(tf_interpreter);

	const TfLiteTensor* outputTensor = TfLiteInterpreterGetOutputTensor(tf_interpreter, 0);
	ASSERT_MSG(outputTensor != nullptr,, "TMAudioSampler::update() - outputTensor is null");

	size_t outputSize = TfLiteTensorByteSize(outputTensor) / sizeof(float);

	if (outputSize > readSize)
	{
		delete[] tempBuffer;
		tempBuffer = new float[outputSize];
	}

	TfLiteTensorCopyToBuffer(outputTensor, tempBuffer, outputSize * sizeof(float));

	// char* debugString = new char[outputSize * 10 + 512];
	// sprintf_s(debugString, 512, "TMAudioSampler::update() - output: [");
	// for (size_t i = 0; i < outputSize; i++)
	// {
	// 	sprintf_s(debugString, outputSize * 10 + 512, "%s%.2f, ", debugString, tempBuffer[i]);
	// }
	// sprintf_s(debugString, outputSize * 10 + 512, "%s]", debugString);

	// EDebug::Log(debugString);
	// delete debugString;

	// Background = 0
	// Breath In = 1
	// Breath Out = 2

	// yes = ~Background
	// In = In | yes
	// Out is set automatically based on 'In'
	float yes = tempBuffer[1] + tempBuffer[2];
	float in = (tempBuffer[2] != 0) ? tempBuffer[1] / yes : 1.0f;

	lastSample.setYes(yes);
	lastSample.setIn(in);

	delete[] tempBuffer;
}