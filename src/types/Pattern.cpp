#include "pch.h"

#include "Pattern.hpp"
#include "Keyframe.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\rapidjson\document.h"

Serializable *Pattern::create()
{
	return new Pattern();
}

Pattern::Pattern(char *name, float length, Keyframe *keyframes, int numKeyframes)
{
	this->name = name;
	this->length = length;
	this->keyframes = keyframes;
	this->numKeyframes = numKeyframes;

	orderKeyframes();
	applyHoldValues();
}

void Pattern::setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references)
{
	char *json;
	SetString(json);

	this->clear();
	this->setFromJson(json);
}

void Pattern::addParameterDefinition(ClassDefinition *definition)
{
	definition->addStringDefinition(
		"PatternJSON",
		"JSON string which defines the pattern",
		"{name: \"Test Pattern\", length: 10, keyframes: [{time: 0, target: {In: 0.5}}]}",
		PAR_ReadOnlyWhilePlaying
	);
}

void Pattern::clear()
{
	if (name != nullptr)
	{
		delete[] name;
		name = nullptr;
	}

	if (keyframes != nullptr)
	{
		delete[] keyframes;
		keyframes = nullptr;
	}

	if (samples != nullptr)
	{
		delete[] samples;
		samples = nullptr;
	}

	length = 0;
	numKeyframes = 0;
}

void Pattern::setFromJson(const char *jsonString)
{
	rapidjson::Document document;
	document.Parse(jsonString);

	if (document.HasParseError())
	{
		EDebug::Error("Could not parse JSON file");
		return;
	}

	if (!document.HasMember("name") || !document["name"].IsString())
	{
		EDebug::Error("JSON file does not contain a valid name");
		return;
	}

	if (!document.HasMember("length") || !document["length"].IsFloat())
	{
		EDebug::Error("JSON file does not contain a valid length");
		return;
	}

	if (!document.HasMember("keyframes") || !document["keyframes"].IsArray())
	{
		EDebug::Error("JSON file does not contain any keyframes");
		return;
	}

	name = new char[document["name"].GetStringLength() + 1];
	strcpy_s(name, document["name"].GetStringLength() + 1, document["name"].GetString());

	length = document["length"].GetFloat();

	const rapidjson::Value &keyframesArray = document["keyframes"];
	numKeyframes = keyframesArray.Size();
	keyframes = new Keyframe[numKeyframes];

	for (rapidjson::Value::ConstValueIterator itr = keyframesArray.Begin(); itr != keyframesArray.End(); ++itr)
	{

		if (!itr->HasMember("time") || !itr->HasMember("sample"))
		{
			EDebug::Error("JSON file contains an invalid keyframe");
			return;
		}

		float time = (*itr)["time"].GetFloat();
		Interpolators::Method method = Interpolators::Constant;

		if (itr->HasMember("transition"))
		{
			int transition = (*itr)["transition"].GetInt();

			if (transition == -1)
				method = Interpolators::Hold;
			else
				method = (Interpolators::Method)transition;
		}

		BreathSample sample = BreathSample(-1, -1, -1, -1, -1);

		const rapidjson::Value &sample_value = (*itr)["sample"];

		if (sample_value.HasMember("No")) sample.setNo(sample_value["No"].GetFloat());
		if (sample_value.HasMember("Yes")) sample.setYes(sample_value["Yes"].GetFloat());
		if (sample_value.HasMember("In")) sample.setIn(sample_value["In"].GetFloat());
		if (sample_value.HasMember("Out")) sample.setOut(sample_value["Out"].GetFloat());
		if (sample_value.HasMember("Nasal")) sample.setNasal(sample_value["Nasal"].GetFloat());
		if (sample_value.HasMember("Mouth")) sample.setMouth(sample_value["Mouth"].GetFloat());
		if (sample_value.HasMember("Pitch")) sample.setPitch(sample_value["Pitch"].GetFloat());
		if (sample_value.HasMember("Volume")) sample.setVolume(sample_value["Volume"].GetFloat());

		keyframes[itr - keyframesArray.Begin()] = Keyframe(sample, time, method);
	}
}


Pattern::~Pattern()
{
	delete[] keyframes;
}

char *Pattern::getName() const
{
	return name;
}

float Pattern::getLength() const
{
	return length;
}

int Pattern::getNumKeyframes() const
{
	return numKeyframes;
}

Keyframe *Pattern::getKeyframes() const
{
	return keyframes;
}

const BreathSample* *Pattern::getSamples()
{
	ASSERT(isValid(), nullptr);

	if (samples != nullptr)
		return samples;

	samples = new const BreathSample*[numKeyframes];

	for (int i = 0; i < numKeyframes; i++)
	{
		samples[i] = &keyframes[i].sample;
	}
	return samples;
}

bool Pattern::isValid() const
{
	return numKeyframes > 0 && keyframes != nullptr && length > 0.0f;
}

float Pattern::toAbsoluteTime(float normTime) const
{
	return normTime * length;
}

float Pattern::toNormalizedTime(float absoluteTime) const
{
	return absoluteTime / length;
}

int Pattern::getKeyframeIndex(float normTime) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, 0);
	ASSERT(isValid(), 0);

	if (numKeyframes == 1)
	{
		return 0;
	}

	int index = 0;

	if (numKeyframes > 16)
	{
		// Do a binary search to find the keyframe index
		int min = 0;
		int max = numKeyframes - 1;
		int mid = (min + max) / 2;

		while (min < max)
		{
			if (keyframes[mid].time > normTime)
				max = mid - 1;
			else if (keyframes[mid].time < normTime)
				min = mid + 1;
			else
				break;

			mid = (min + max) / 2;
		}
		index = mid;
	}
	else
	{
		// Do a linear search to find the keyframe index
		for (int i = 0; i < numKeyframes; i++)
		{
			if (keyframes[i].time > normTime)
			{
				index = i - 1;
				break;
			}
		}
	}

	return index;
}

void Pattern::getBoundingKeyframes(float normTime, int& keyframeIndex1, int& keyframeIndex2) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f,);
	ASSERT(isValid(),);

	if (numKeyframes == 1)
	{
		keyframeIndex1 = 0;
		keyframeIndex2 = 0;
		return;
	}

	keyframeIndex1 = getKeyframeIndex(normTime);

	if (keyframeIndex1 == numKeyframes - 1)
		keyframeIndex2 = 0;
	else keyframeIndex2 = keyframeIndex1 + 1;
}

bool Pattern::getBoundingKeyframes(float normTime, int sampleDataIndex, int& keyframeIndex1, int& keyframeIndex2) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, false);
	ASSERT(isValid(), false);

	if (numKeyframes == 1)
	{
		if (keyframes[0].sample.hasValue(sampleDataIndex))
		{
			keyframeIndex1 = 0;
			keyframeIndex2 = 0;
			return true;
		}
		else
		{
			keyframeIndex1 = -1;
			keyframeIndex2 = -1;
			return false;
		}
	}

	keyframeIndex1 = -1;
	keyframeIndex2 = -1;

	for (int i = 0; i < numKeyframes; i++)
	{
		if (keyframes[i].sample.hasValue(sampleDataIndex))
		{
			if (keyframes[i].time > normTime)
			{
				keyframeIndex2 = i;
				break;
			}
			else
			{
				keyframeIndex1 = i;

				if (keyframeIndex2 == -1)
					keyframeIndex2 = i;
			}
		}
	}

	if (keyframeIndex1 == -1)
	{
		return false;
	}

	return true;
}

void Pattern::getBoundingKeyframes(float normTime1, float normTime2, int& keyframeIndex1, int& keyframeIndex2) const
{
	ASSERT(normTime1 >= 0.0f && normTime1 <= 1.0f,);
	ASSERT(normTime2 >= 0.0f && normTime2 <= 1.0f,);
	ASSERT(isValid(),);
	
	keyframeIndex1 = -1;
	keyframeIndex2 = -1;

	if (normTime1 == normTime2)
	{
		for (int i = 0; i < numKeyframes; i++)
		{
			if (keyframes[i].time == normTime1)
			{
				keyframeIndex1 = i;
				keyframeIndex2 = i;
				return;
			}
		}
	}
	else if (normTime1 < normTime2)
	{
		for (int i = 0; i < numKeyframes; i++)
		{
			if (keyframes[i].time >= normTime1)
			{
				if (keyframes[i].time < normTime2)
				{
					if (keyframeIndex1 == -1)
						keyframeIndex1 = keyframeIndex2 = i;
					else keyframeIndex2++;
				}
				else return;
			}
		}
	}
	else
	{
		for (int i = 0; i < numKeyframes; i++)
		{
			if (keyframes[i].time < normTime2)
			{
				keyframeIndex2 = i;
			}
			else if (keyframes[i].time > normTime1)
			{
				if (keyframeIndex1 == -1)
				{
					keyframeIndex1 = i;

					if (keyframeIndex2 != -1) return;
				}

				keyframeIndex2 = i;
			}
		}
	}
}

bool Pattern::getInterpolatedValue(float normTime, int sampleDataIndex, float& value) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, false);
	ASSERT(isValid(), false);

	if (numKeyframes == 1)
	{
		if (keyframes[0].sample.hasValue(sampleDataIndex))
		{
			value = keyframes[0].sample.getValue(sampleDataIndex);
			return true;
		}
		else
		{
			return false;
		}
	}

	int keyframeIndex1 = -1;
	int keyframeIndex2 = -1;

	if (!getBoundingKeyframes(normTime, sampleDataIndex, keyframeIndex1, keyframeIndex2))
	{
		return false;
	}

	if (keyframeIndex1 == keyframeIndex2)
	{
		value = keyframes[keyframeIndex1].sample.getValue(sampleDataIndex);
		return true;
	}

	float time1 = keyframes[keyframeIndex1].time;
	float time2 = keyframes[keyframeIndex2].time;

	float value1 = keyframes[keyframeIndex1].sample.getValue(sampleDataIndex);
	float value2 = keyframes[keyframeIndex2].sample.getValue(sampleDataIndex);

	float t = (normTime - time1) / (time2 - time1);

	value = Interpolators::Interp(value1, value2, t, keyframes[keyframeIndex1].method);

	return true;
}

bool Pattern::getBooleanValue(float normTime, int sampleDataIndex, bool& value) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, false);
	ASSERT(isValid(), false);

	if (numKeyframes == 1)
	{
		if (keyframes[0].sample.hasValue(sampleDataIndex))
		{
			value = keyframes[0].sample.hasValue(sampleDataIndex);
			return true;
		}
		else
		{
			return false;
		}
	}

	int keyframeIndex = getKeyframeIndex(normTime);

	ASSERT(keyframeIndex >= 0 && keyframeIndex < numKeyframes, false);

	if (keyframes[keyframeIndex].sample.hasValue(sampleDataIndex))
	{
		value = keyframes[keyframeIndex].sample.getValue(sampleDataIndex) >= 0.5f;
		return true;
	}
	else return false;
}

BreathSample Pattern::getTarget(float normTime, bool overrideConstants) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, BreathSample());
	ASSERT(isValid(), BreathSample());

	BreathSample sample;

	if (overrideConstants)
	{
		float value;
		for (int i = 0; i < BreathSample::NUM_VALUES; i++)
		{
			if (getInterpolatedValue(normTime, i, value))
			{
				sample.setValue(i, value);
			}
		}
	}
	else
	{
		bool bool_value;
		for (int i = 0; i < BreathSample::NUM_CONSTANTS; i++)
		{
			if (getBooleanValue(normTime, i, bool_value))
			{
				sample.setValue(i, bool_value ? 1.0f : 0.0f);
			}
		}

		float float_value;
		for (int i = BreathSample::NUM_CONSTANTS; i < BreathSample::NUM_VALUES; i++)
		{
			if (getInterpolatedValue(normTime, i, float_value))
			{
				sample.setValue(i, float_value);
			}
		}
	}

	return sample;
}

void Pattern::orderKeyframes()
{
	ASSERT(isValid(),);

	for (int i = 0; i < numKeyframes - 1; i++)
	{
		for (int j = i + 1; j < numKeyframes; j++)
		{
			if (keyframes[i].time > keyframes[j].time)
			{
				Keyframe temp = keyframes[i];
				keyframes[i] = keyframes[j];
				keyframes[j] = temp;
			}
		}
	}
}

void Pattern::applyHoldValues()
{
	ASSERT(isValid(),);

	for (int i = 0; i < numKeyframes; i++)
	{
		if (keyframes[i].method == Interpolators::Hold)
		{
			for (int dataIndex = 0; dataIndex < BreathSample::NUM_VALUES; dataIndex++)
			{
				if (keyframes[i].sample.hasValue(dataIndex))
					continue;

				int j = i - 1;
				while (true)
				{
					if (j < 0)
						j = numKeyframes - 1;

					if (j == i)
						break;

					if (keyframes[j].sample.hasValue(dataIndex))
					{
						keyframes[i].sample.setValue(dataIndex, keyframes[j].sample.getValue(dataIndex));
						break;
					}

					j--;
				}
			}

			int nextIndex = (i + 1) % numKeyframes;
			for (int dataIndex = 0; dataIndex < BreathSample::NUM_VALUES; dataIndex++)
			{
				if (!keyframes[nextIndex].sample.hasValue(dataIndex))
					keyframes[nextIndex].sample.setValue(dataIndex, keyframes[i].sample.getValue(dataIndex));
			}
		}
	}
}