#pragma once

#include "Keyframe.hpp"
#include "BreathSample.hpp"
#include "..\serialization\Serializable.hpp"
#include "..\serialization\ClassDefinitions.hpp"


class Pattern : public Serializable
{
private:
	char *name;
	float length;
	Keyframe *keyframes;
	int numKeyframes;

	const BreathSample* *samples{nullptr};

	void orderKeyframes();
	void applyHoldValues();
	
	void setFromJson(const char *jsonString);
	void clear();

protected:
	void setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references) override;

public:
	SerializableProperties(
		Pattern,
		Pattern_serializedIndex,
		"Pattern",
		"Pattern for breathing",
		SerializedTypes::REF_Pattern)

	Pattern(char *name, float length, Keyframe *keyframes, int numKeyframes);
	~Pattern();

	static void addParameterDefinition(SerializedTypes::ClassDefinition* definition);

	char *getName() const;
	float getLength() const;
	int getNumKeyframes() const;
	Keyframe *getKeyframes() const;

	const BreathSample* *getSamples();

	bool isValid() const;

	float toAbsoluteTime(float normTime) const;
	float toNormalizedTime(float absoluteTime) const;

	int getKeyframeIndex(float normTime) const;
	void getBoundingKeyframes(float normTime, int& keyframeIndex1, int& keyframeIndex2) const;
	bool getBoundingKeyframes(float normTime, int sampleDataIndex, int& keyframeIndex1, int& keyframeIndex2) const;
	void getBoundingKeyframes(float normTime1, float normTime2, int& keyframeIndex1, int& keyframeIndex2) const;

	bool getInterpolatedValue(float normTime, int sampleDataIndex, float& value) const;
	bool getBooleanValue(float normTime, int sampleDataIndex, bool& value) const;

	BreathSample getTarget(float normTime, bool overrideConstants = false) const;

	int toString(char *buffer, int bufferSize) const;
};