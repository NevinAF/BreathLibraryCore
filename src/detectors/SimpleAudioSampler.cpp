
#include "SimpleAudioSampler.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\types\DataStreamMeta.hpp"

constexpr UInt32 READ_COUNT = 1024;

CreateSerializable(SimpleAudioSampler);

void SimpleAudioSampler::update(float delta)
{
	// EDebug::Log("SimpleAudioSampler::update() - delta: %.2f", delta);

	DataStreamMeta streamData = audioStream.getStreamData();

	ASSERT_MSG(streamData.data != nullptr,, "SimpleAudioSampler::update() - streamData.data is null");
	ASSERT_MSG(streamData.capacity > 0,, "SimpleAudioSampler::update() - streamData.capacity is 0");
	ASSERT_MSG(streamData.putIndex < streamData.capacity,, "SimpleAudioSampler::update() - streamData.putIndex is out of bounds");

	float* data = (float*)streamData.data;

	// Average volume of audio using A-weighting
	// float dB = 0.0f;
	// UInt32 readCount = READ_COUNT > streamData.capacity ? streamData.capacity : READ_COUNT;
	// for (UInt32 i = 0; i < readCount; i++)
	// {
	// 	float sample = data[(streamData.putIndex - i) % streamData.capacity];
	// 	dB += 20.0f * log10f(sample);
	// }
	// dB /= readCount;

	float volume = 0.0f;
	UInt32 readCount = READ_COUNT > streamData.capacity ? streamData.capacity : READ_COUNT;
	for (UInt32 i = 0; i < readCount; i++)
	{
		float sample = data[(streamData.putIndex - i) % streamData.capacity];
		volume += sample * sample;
	}

	volume /= readCount;
	volume = (float)sqrt(volume);

	// if volume is nan, set it to 0
	if (volume != volume)
		volume = 0.0f;

	if (volume < min_lowpass)
		min_lowpass = volume;
	else
		min_lowpass = (volume - min_lowpass) * 0.005f + min_lowpass * 0.995f;

	if (volume > max_lowpass)
		max_lowpass = volume;
	else
		max_lowpass = (volume - max_lowpass) * 0.005f + max_lowpass * 0.995f;

	float norm = Interpolators::IInterp(min_lowpass, max_lowpass, volume, Interpolators::Linear);
	if (norm < 0.0f)
		norm = 0.0f;
	else if (norm > 1.0f)
		norm = 1.0f;

	lastSample.setVolume(norm);
	lastSample.setYes(norm > 0.05 ? 1.0f : 0.0f);

	// EDebug::Log("SimpleAudioSampler::update() - volume: %.2f, min: %.2f, max: %.2f", volume, min_lowpass, max_lowpass);
}

BreathSample SimpleAudioSampler::Sample()
{
	Behaviour::demandUpdate();

	return lastSample;
}

void SimpleAudioSampler::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);

	definition->addReferenceDefinition("Audio Stream", "A IDataStream to read audio input from", ReferenceType::REF_DataStream);
}

void SimpleAudioSampler::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	Behaviour::setParameterIndex(paramIndex, savedData, runtimeData);

	serializable_SetReference(audioStream);
}