#ifndef BREATH_SAMPLE_H
#define BREATH_SAMPLE_H

#include "..\util\interpolators.hpp"

/** Data class for temporal breath data. Contains information of a breath at a specific time. */
struct BreathSample
{
	static const int NUM_VALUES = 5;
	static const int NUM_CONSTANTS = 3;

	/** 0.0f to 1.0f. Probability that the user is breathing at a given time-step.
	 * A value of -1 describes that the No is not being detected. The sum of In, Out, and No must be 1.0f.*/
	float yes;
	/** 0.0f to 1.0f. Probability that the user is breathing in [1] or out [0]  at a given time-step.
	 * A value of -1 describes that the In/Out is not being detected. The sum of In, Out, and No must be 1.0f.*/
	float in;
	/** 0.0f to 1.0f. Probability that a In/Out breath is coming through the nose (not mouth).
	 * A value of -1 describes that the Nasal is not being detected. Mid values like 0.5f show uncertainty or both nasal and mouth.*/
	float nasal;
	/** Volume is normalized to the range [0-1]. A value of null describes that the volume is not being detected. */
	float volume;
	/**
	 * In Hertz. The frequency of breath. A value of null describes that the pitch is not being detected.
	 * Value should reasonably correspond to the following ranges:
	 * null = Invalid / Pitch is not being detected.
	 * 0.00 = 0hz to 60hz = Not a breath detected.
	 * 0 to 0.25 = 60hz to 300hz = Low (pitched) breath detected.
	 * 0.25 to 0.50 = 300hz to 600hz = Middle (pitched) breath detected.
	 * 0.50 to 0.75 = 600hz to 1200hz = High (pitched) breath detected.
	 * 0.75 to 1.00 = 1200hz to 2800hz = Wheezing or Stridor
	 * 1.00 = 2800hz+ = Not a breath detected
	 */
	float pitch;

	BreathSample();
	BreathSample(float yes, float in, float nasal, float volume, float pitch);
	BreathSample(const BreathSample& other);

	void set(float yes, float in, float nasal, float volume, float pitch);
	void set(const BreathSample& other);

	float getYes() const;
	float getNo() const;
	float getIn() const;
	float getOut() const;
	float getNasal() const;
	float getMouth() const;
	float getVolume() const;
	float getPitch() const;

	bool isYes() const;
	bool isNo() const;
	bool isIn() const;
	bool isOut() const;
	bool isNasal() const;
	bool isMouth() const;

	bool hasYes() const;
	bool hasNo() const;
	bool hasIn() const;
	bool hasOut() const;
	bool hasNasal() const;
	bool hasMouth() const;
	bool hasVolume() const;
	bool hasPitch() const;


	void setYes(float yes);
	void setNo(float no);
	void setIn(float in);
	void setOut(float out);
	void setNasal(float nasal);
	void setMouth(float mouth);
	void setVolume(float volume);
	void setPitch(float pitch);

	bool hasValue(int index) const;
	float getValue(int index) const;
	void setValue(int index, float value);

	bool operator==(const BreathSample& other) const;

	/** Returns a string representation of the data. */
	int toString(char* buffer, int bufferSize) const;


	static BreathSample AndInterp(const BreathSample& a, const BreathSample& b, float t, Interpolators::Method interpolator = Interpolators::Linear);

	static BreathSample OrInterp(const BreathSample &a, const BreathSample &b, float t, Interpolators::Method interpolator = Interpolators::Linear);

	static float Likeness(const BreathSample& a, const BreathSample& b);

	static BreathSample ClosestValues_val(const BreathSample& a, const BreathSample* samples, int size);

	static BreathSample ClosestValues_ptr(const BreathSample& a, const BreathSample* *samples, int size);

	float ClosedValue(float value, float min, float max) const;
};

#endif