#include "pch.h"
#include "BreathStream.hpp"
#include "..\debugging\DebugCallbacks.hpp"

CreateSerializable(BreathStream);

BreathStream::BreathStream(UInt16 capacity)
{
	this->capacity = capacity;
	this->pushIndex = 0;
	this->totalSamples = 0;
	this->samples = new BreathSample[capacity];
}

BreathStream::~BreathStream()
{
	delete[] samples;
}

/** @return The number of samples that can fit in the stream. This is the size of the buffer. */
int BreathStream::getCapacity() const { return capacity; }

/** @return The index for the end of the timeline. */
int BreathStream::getPushIndex()
{
	demandUpdate();
	return pushIndex;
}

/** @return The number of samples that have been pushed into the stream. */
UInt64 BreathStream::getTotalSamples()
{
	demandUpdate();
	return totalSamples;
}

/** @return The sample that is "index" behind the latest sample */
BreathSample* BreathStream::getSamplesAt(int index)
{
	demandUpdate();
	if (index < 0 || index >= capacity || index >= totalSamples)
	{
		EDebug::Warning("BreathStream::getSamplesAt: Index out of bounds.");
		return nullptr;
	}

	int indexToGet = pushIndex - index;
	if (indexToGet < 0)
	{
		indexToGet += capacity;
	}
	return &samples[indexToGet];
}

/** @return The latest sample in the stream. */
BreathSample* BreathStream::getLatestSample()
{
	demandUpdate();
	return this->getSamplesAt(0);
}

void BreathStream::update(float delta)
{
	samples[pushIndex] = sampler.get()->getSample();
	pushIndex++;
	totalSamples++;
	if (pushIndex >= capacity)
	{
		pushIndex = 0;
	}
}

BreathSample BreathStream::getSample()
{
	demandUpdate();
	return *getLatestSample();
}

void BreathStream::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);

	definition->addReferenceDefinition(
		"Sampler",
		"The sampler that this stream will sample from.",
		SerializedTypes::REF_Sampler);

	definition->addIntDefinition(
		"Capacity",
		"The number of sample that this breath stream can hold",
		100,
		1,
		MAXED_SINT32,
		SerializedTypes::PAR_ReadOnlyWhilePlaying);
}

void BreathStream::setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references)
{
	Behaviour::setParameterIndex(paramIndex, data, references);

	SetReference(sampler);
	SetInt(capacity);
}

// DetectorController::DetectorController(int detectorCapacity)
// {
// 	this->detectorCapacity = detectorCapacity;
// 	this->detectorCount = 0;
// 	this->detectors = new BreathDetector*[detectorCapacity];
// }

// DetectorController::~DetectorController()
// {
// 	delete[] detectors;
// }

// void DetectorController::AddDetector(BreathDetector *detector)
// {
// 	ASSERT(detectorCount < detectorCapacity)

// 	detectors[detectorCount] = detector;
// 	detectorCount++;
// }

// void DetectorController::RemoveDetector(BreathDetector *detector)
// {
// 	for (int i = 0; i < detectorCount; i++)
// 	{
// 		if (detectors[i] == detector)
// 		{
// 			RemoveDetector(i);
// 			return;
// 		}
// 	}
// }

// void DetectorController::RemoveDetector(int index)
// {
// 	ASSERT(index >= 0 && index < detectorCount);

// 	// Shift all the detectors after the removed one down one index
// 	for (int i = index; i < detectorCount - 1; i++)
// 	{
// 		detectors[i] = detectors[i + 1];
// 	}

// 	detectorCount--;

// 	// Set the last detector to null
// 	detectors[detectorCount] = nullptr;
// }

// void DetectorController::RemoveAllDetectors()
// {
// 	for (int i = 0; i < detectorCount; i++)
// 	{
// 		detectors[i] = nullptr;
// 	}

// 	detectorCount = 0;
// }

// void DetectorController::Sample()
// {
// 	BreathSample sample;
// 	for (int i = 0; i < detectorCount; i++)
// 	{
// 		sample = detectors[i]->Sample(stream);
// 	}

// 	stream->pushSample(sample);
// }