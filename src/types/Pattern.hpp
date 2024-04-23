#ifndef PATTERN_H
#define PATTERN_H

#include "Keyframe.hpp"
#include "BreathSample.hpp"
#include "..\serialization\Serializable.hpp"


class Pattern : public Serializable
{
private:
	char *name{nullptr};
	float length{0};
	Keyframe *keyframes{ nullptr };
	int numKeyframes{0};

	const BreathSample* *samples{nullptr};

	Keyframe* *perData_keyframes{nullptr};
	int*perData_start{nullptr};
	void calculatePerDataKeyframes();

	void orderKeyframes();
	void applyHoldValues();

	void setFromJson(const char *jsonString);
	void clear();

protected:
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;
	void initialize() override;

public:
	SerializableProperties(
		Pattern,
		"Pattern",
		"Pattern for breathing",
		ReferenceType::REF_Pattern)

	Pattern(char *name, float length, Keyframe *keyframes, int numKeyframes);
	~Pattern();

	static void addParameterDefinition(ClassDefinition* definition);

	char *getName() const;
	float getLength() const;
	int getNumKeyframes() const;
	Keyframe *getKeyframes() const;

	const BreathSample* *getSamples();

	bool isValid() const;

	float toAbsoluteTime(float normTime) const;
	float toNormalizedTime(float absoluteTime) const;

	float static interpKeyframes(float normTime, int sampleDataIndex, const Keyframe* keyframe1, const Keyframe* keyframe2);

	int getKeyframeIndexAtTime(float normTime) const;
	bool getKeyframeAtTime(float normTime, int sampleDataIndex, Keyframe *&keyframe) const;
	bool getBoundingKeyframes(float normTime, int sampleDataIndex, Keyframe *&keyframe1, Keyframe *&keyframe2) const;
	bool getBoundingRange(float normTime1, float normTime2, int sampleDataIndex, float &min, float &max) const;

	bool getInterpolatedValue(float normTime, int sampleDataIndex, float& value) const;
	bool getBooleanValue(float normTime, int sampleDataIndex, bool& value) const;

	BreathSample getTarget(float normTime, bool overrideConstants = false) const;
	BreathSample getClosestSample(const BreathSample *target, float normTime1, float normTime2) const;

	int toString(char *buffer, int bufferSize) const;
};

#endif