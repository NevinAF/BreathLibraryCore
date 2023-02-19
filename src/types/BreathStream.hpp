#pragma once

#include "..\serialization\Serializable.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\serialization\ClassDefinitions.hpp"
#include "..\types\Behaviour.hpp"
#include "..\serialization\ReferenceManager.hpp"

#include <vector>

/**
 * @brief Data class for holding and manipulating Aggregate Breath Samples.
 * Contains a samples for the most recent in-the-moment data. Implements a circular buffer for storing historical data.
 */
class BreathStream : public IBreathSampler, Behaviour, Serializable
{
private:
	/** The number of samples that can fit in the stream. This is the size of the buffer. */
	SInt32 capacity{ -1 };

	/** The index for the end of the timeline. */
	UInt16 pushIndex{ 0 };

	/** Total number of samples that have been added to the stream over its entire lifetime. */
	UInt64 totalSamples{ 0 };

	/** The samples that are currently in the stream. */
	BreathSample* samples{ nullptr };

	/** Number of breath samplers */
	Ref<IBreathSampler> sampler{};

protected:
	/** Pushes a sample onto the end of the stream */
	void update(float delta) override;
	void setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references) override;

public:
	/**
	 * @brief Creates a new BreathStream with the given capacity and sampling period.
	 * @param capacity The number of samples that can fit in the stream. This is the size of the buffer.
	 * @param samplingPeriod The time in seconds between gathering breath samples into the breath stream.
	 */
	BreathStream(UInt16 capacity);

	/** @brief Destructor. This will delete all breath samples */
	~BreathStream();

	/** @return The number of samples that can fit in the stream. This is the size of the buffer. */
	int getCapacity() const;

	/** @return The index for the end of the timeline. */
	int getPushIndex();

	/** @return The number of samples that have been pushed into the stream. */
	UInt64 getTotalSamples();

	/** @return The sample that is "index" behind the latest sample */
	BreathSample *getSamplesAt(int index);

	/** @return The latest sample in the stream. */
	BreathSample *getLatestSample();

	
	BreathSample getSample() override;

	SerializableProperties(
		BreathStream,
		BreathStream_serializedIndex,
		"Breath Stream",
		"Collection of breath samples from a sampler. Used for storing historical data to be used for analysis, like with session data. Can be used a sampler itself, always returning the latest sample.",
		SerializedTypes::REF_SamplerStream)

	static void addParameterDefinition(SerializedTypes::ClassDefinition *definition);
};