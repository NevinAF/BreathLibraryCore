#include "pch.h"
#include "BreathSample.hpp"
#include <stdio.h>
#include <math.h>
#include "..\debugging\DebugCallbacks.hpp"

BreathSample::BreathSample()
{
	this->set(-1.0f, -1.0f, -1.0f, -1.0f, -1.0f);
}

BreathSample::BreathSample(float yes, float in, float nasal, float volume, float pitch)
{
	this->set(yes, in, nasal, volume, pitch);
}

void BreathSample::set(float yes, float in, float nasal, float volume, float pitch)
{
	this->yes = yes;
	this->in = in;
	this->nasal = nasal;
	this->volume = volume;
	this->pitch = pitch;
}

BreathSample::BreathSample(const BreathSample& other)
{
	this->set(other);
}

void BreathSample::set(const BreathSample& other)
{
	this->yes = other.yes;
	this->in = other.in;
	this->nasal = other.nasal;
	this->volume = other.volume;
	this->pitch = other.pitch;
}

float BreathSample::getYes() const { return yes; }
float BreathSample::getNo() const { return 1.0f - yes; }
float BreathSample::getIn() const { return in; }
float BreathSample::getOut() const { return 1.0f - in; }
float BreathSample::getNasal() const { return nasal; }
float BreathSample::getMouth() const { return 1.0f - nasal; }
float BreathSample::getVolume() const { return volume; }
float BreathSample::getPitch() const { return pitch; }

bool BreathSample::isYes() const { return yes >= 0.5f; }
bool BreathSample::isNo() const { return yes < 0.5f; }
bool BreathSample::isIn() const { return in >= 0.5f; }
bool BreathSample::isOut() const { return in < 0.5f; }
bool BreathSample::isNasal() const { return nasal >= 0.5f; }
bool BreathSample::isMouth() const { return nasal < 0.5f; }

bool BreathSample::hasYes() const { return yes >= 0.0f; }
bool BreathSample::hasNo() const { return yes >= 0.0f; }
bool BreathSample::hasIn() const { return in >= 0.0f; }
bool BreathSample::hasOut() const { return in >= 0.0f; }
bool BreathSample::hasNasal() const { return nasal >= 0.0f; }
bool BreathSample::hasMouth() const { return nasal >= 0.0f; }
bool BreathSample::hasVolume() const { return volume >= 0.0f; }
bool BreathSample::hasPitch() const { return pitch >= 0.0f; }

void BreathSample::setYes(float yes) { this->yes = yes; }
void BreathSample::setNo(float no) { this->yes = 1.0f - no; }
void BreathSample::setIn(float in) { this->in = in; }
void BreathSample::setOut(float out) { this->in = 1.0f - out; }
void BreathSample::setNasal(float nasal) { this->nasal = nasal; }
void BreathSample::setMouth(float mouth) { this->nasal = 1.0f - mouth; }
void BreathSample::setVolume(float volume) { this->volume = volume; }
void BreathSample::setPitch(float pitch) { this->pitch = pitch; }

bool BreathSample::hasValue(int index) const
{
	return getValue(index) >= 0.0f;
}

float BreathSample::getValue(int index) const
{
	ASSERT(index >= 0 && index < 5, -1.0f);

	switch (index)
	{
	case 0:
		return yes;
	case 1:
		return in;
	case 2:
		return nasal;
	case 3:
		return volume;
	case 4:
		return pitch;
	default:
		return -1.0f;
	}
}

void BreathSample::setValue(int index, float value)
{
	ASSERT(index >= 0 && index < 5, );

	switch (index)
	{
	case 0:
		yes = value;
		break;
	case 1:
		in = value;
		break;
	case 2:
		nasal = value;
		break;
	case 3:
		volume = value;
		break;
	case 4:
		pitch = value;
		break;
	default:
		break;
	}
}

bool BreathSample::operator==(const BreathSample& other) const
{
	return yes == other.yes && in == other.in && nasal == other.nasal && volume == other.volume && pitch == other.pitch;
}

int BreathSample::toString(char* buffer, int bufferSize) const
{
	return sprintf_s(buffer, bufferSize, "yes: %0.2f, in: %0.2f, nasal: %0.2f, volume: %0.2f, pitch: %0.2f", yes, in, nasal, volume, pitch);
}



BreathSample BreathSample::AndInterp(const BreathSample& a, const BreathSample& b, float t, Interpolators::Method interpolator)
{
	BreathSample result;

	for (int i = 0; i < NUM_VALUES; i++)
	{
		if (a.hasValue(i) && b.hasValue(i))
		{
			float value = Interpolators::Interp(a.getValue(i), b.getValue(i), t, interpolator);
			result.setValue(i, value);
		}
	}

	return result;
}

BreathSample BreathSample::OrInterp(const BreathSample& a, const BreathSample& b, float t, Interpolators::Method interpolator)
{
	BreathSample result;
	for (int i = 0; i < NUM_VALUES; i++)
	{
		if (a.hasValue(i) && b.hasValue(i))
		{
			float value = Interpolators::Interp(a.getValue(i), b.getValue(i), t, interpolator);
			result.setValue(i, value);
		}
		else if (a.hasValue(i))
		{
			result.setValue(i, a.getValue(i));
		}
		else if (b.hasValue(i))
		{
			result.setValue(i, b.getValue(i));
		}
	}

	return result;
}

float BreathSample::Likeness(const BreathSample& a, const BreathSample& b)
{
	// Likeness using the RMSE (Root Mean Squared Error) metric

	float sum = 0.0f;
	int count = 0;

	for (int i = 1; i < NUM_VALUES; i++)
	{
		if (a.hasValue(i) && b.hasValue(i))
		{
			float diff = a.getValue(i) - b.getValue(i);
			sum += diff * diff;
			count++;
		}
	}

	if (count == 0)
	{
		EDebug::Warning("BreathSample::Likeness: No values to compare");

		return 0.0f;
	}

	// The "no" value is a special case where non of the other values will matter.

	float noScalar = 1.0f;
	float noDiff = 0.0f;
	if (a.hasNo() && b.hasNo())
	{
		noScalar = a.getYes() * b.getYes();
		noDiff = fabsf(a.getNo() - b.getNo());
	}

	return 1.0f - (sqrtf(sum / count) * noScalar + noDiff * (1.0f - noScalar));
}

BreathSample BreathSample::ClosestValues_val(const BreathSample& a, const BreathSample* samples, int size)
{
	if (size == 0)
	{
		EDebug::Warning("BreathSample::ClosestValues: No samples to compare");

		return BreathSample();
	}
	else if (size == 1)
	{
		return samples[0];
	}

	BreathSample result = samples[0];

	for (int data_index = 0; data_index < NUM_VALUES; data_index++)
	{
		if (!a.hasValue(data_index))
		{
			if (result.hasValue(data_index))
				result.setValue(data_index, -1.0f);
			continue;
		}

		// result.getValue(data_index) = closest value
		float best_diff = fabsf(a.getValue(data_index) - result.getValue(data_index));

		for (int sample_index = 1; sample_index < size; sample_index++)
		{
			if (samples[sample_index].hasValue(data_index))
			{
				float diff = fabsf(a.getValue(data_index) - samples[sample_index].getValue(data_index));

				if (diff < best_diff)
				{
					best_diff = diff;
					result.setValue(data_index, samples[sample_index].getValue(data_index));
				}
			}
		}
	}

	return result;
}

BreathSample BreathSample::ClosestValues_ptr(const BreathSample& a, const BreathSample** samples, int size)
{
	if (size == 0)
	{
		EDebug::Warning("BreathSample::ClosestValues: No samples to compare");

		return BreathSample();
	}
	else if (size == 1)
	{
		return *samples[0];
	}

	BreathSample result = *samples[0];

	for (int data_index = 0; data_index < NUM_VALUES; data_index++)
	{
		if (!a.hasValue(data_index))
		{
			if (result.hasValue(data_index))
				result.setValue(data_index, -1.0f);
			continue;
		}

		// result.getValue(data_index) = closest value
		float best_diff = fabsf(a.getValue(data_index) - result.getValue(data_index));

		for (int sample_index = 1; sample_index < size; sample_index++)
		{
			if (samples[sample_index]->hasValue(data_index))
			{
				float diff = fabsf(a.getValue(data_index) - samples[sample_index]->getValue(data_index));

				if (diff < best_diff)
				{
					best_diff = diff;
					result.setValue(data_index, samples[sample_index]->getValue(data_index));
				}
			}
		}
	}

	return result;
}