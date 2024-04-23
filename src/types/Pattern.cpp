
#include "Pattern.hpp"
#include "Keyframe.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\rapidjson\document.h"

CreateSerializable(Pattern);

Pattern::Pattern(char *name, float length, Keyframe *keyframes, int numKeyframes)
{
	this->name = name;
	this->length = length;
	this->keyframes = keyframes;
	this->numKeyframes = numKeyframes;

	initialize();
}

void Pattern::initialize()
{
	ASSERT(name != nullptr, );
	ASSERT(length > 0, );
	ASSERT(keyframes != nullptr, );
	ASSERT(numKeyframes > 0, );

	orderKeyframes();
	applyHoldValues();
	calculatePerDataKeyframes();

	// EDebug::Log("Pattern::setParameterIndex() - name: %s, length: %f, numKeyframes: %d", name, length, numKeyframes);
	// for (int i = 0; i < numKeyframes; i++)
	// {
	// 	EDebug::Log("\tkeyframe %d: time: %f, target: {Yes: %f, In: %f, Nasal: %f, Volume: %f, Pitch: %f}", i, keyframes[i].time, keyframes[i].sample.yes, keyframes[i].sample.in, keyframes[i].sample.nasal, keyframes[i].sample.volume, keyframes[i].sample.pitch);
	// }
}

void Pattern::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	if (paramIndex-- == 0)
	{
		unsigned char *json = nullptr;
		UInt32 jsonLength = 0;
		Serializable::setArray_RuntimeBytes(json, jsonLength, runtimeData);

		if (json != nullptr)
		{
			char *jsonString = new char[jsonLength + 1];
			memcpy(jsonString, json, jsonLength);
			jsonString[jsonLength] = '\0';

			setFromJson(jsonString);

			delete[] jsonString;
			delete[] json;
		}

	}
}

void Pattern::addParameterDefinition(ClassDefinition *definition)
{
	definition->addFileDefinition(
		"Pattern JSON",
		"JSON file which defines the pattern",
		"json",
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

#define BREATH_SAMPLE_SETTER(name)                                                                                                                         \
	if (sample_value.HasMember(#name))                                                                                                                     \
	{                                                                                                                                                      \
		ASSERT_MSG(sample_value[#name].IsNumber(), , "JSON file contains an invalid keyframe: '" #name "' value is not a number.");                        \
		float value = sample_value[#name].GetFloat();                                                                                                      \
		ASSERT_MSG(value <= 1.0f && value >= 0.0f, , "JSON file contains an invalid keyframe: '" #name "' is not between 0 and 1 inclusive (%f).", value); \
		sample.set##name(value);                                                                                                                           \
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
		EDebug::Error("JSON file does not contain a valid 'name' value.");
		return;
	}

	if (!document.HasMember("length") || !document["length"].IsNumber())
	{
		EDebug::Error("JSON file does not contain a valid 'length' value.");
		return;
	}

	if (!document.HasMember("keyframes") || !document["keyframes"].IsArray())
	{
		EDebug::Error("JSON file does not contain valid 'keyframes' values.");
		return;
	}

	name = new char[document["name"].GetStringLength() + 1];
	strcpy(name, document["name"].GetString());

	length = document["length"].GetFloat();

	const rapidjson::Value &keyframesArray = document["keyframes"];
	numKeyframes = keyframesArray.Size();
	keyframes = new Keyframe[numKeyframes];

	for (rapidjson::Value::ConstValueIterator itr = keyframesArray.Begin(); itr != keyframesArray.End(); ++itr)
	{

		if (!itr->HasMember("time") || !itr->HasMember("target"))
		{
			EDebug::Error("JSON file contains an invalid keyframe: one that does not contain both a 'time' and 'target' value.");
			return;
		}

		if (!(*itr)["time"].IsNumber())
		{
			EDebug::Error("JSON file contains an invalid keyframe: 'time' value is not a number.");
			return;
		}

		if (!(*itr)["target"].IsObject())
		{
			EDebug::Error("JSON file contains an invalid keyframe: 'target' value is not an object.");
			return;
		}

		float time = (*itr)["time"].GetFloat();
		Interpolators::Method method = Interpolators::Constant;

		if (itr->HasMember("transition"))
		{
			if ((*itr)["transition"].IsString())
			{
				if (!Interpolators::tryParseMethod((*itr)["transition"].GetString(), method))
				{
					EDebug::Error("JSON file contains an invalid keyframe: 'transition' value is not a valid interpolation method: %s.", (*itr)["transition"].GetString());
					return;
				}
			}
			else if ((*itr)["transition"].IsInt())
			{
				int transition = (*itr)["transition"].GetInt();

				if (transition == -1)
					method = Interpolators::Hold;
				else
					method = (Interpolators::Method)transition;
			}
			else
			{
				EDebug::Error("JSON file contains an invalid keyframe: 'transition' value is not a string or integer.");
				return;
			}
		}

		BreathSample sample = BreathSample(-1, -1, -1, -1, -1);

		const rapidjson::Value &sample_value = (*itr)["target"];

		BREATH_SAMPLE_SETTER(No)
		BREATH_SAMPLE_SETTER(Yes)
		BREATH_SAMPLE_SETTER(In)
		BREATH_SAMPLE_SETTER(Out)
		BREATH_SAMPLE_SETTER(Nasal)
		BREATH_SAMPLE_SETTER(Mouth)
		BREATH_SAMPLE_SETTER(Pitch)
		BREATH_SAMPLE_SETTER(Volume)

		if (sample_value.HasMember("No") && sample_value.HasMember("Yes"))
			EDebug::Warning("JSON file keyframe has both 'No' and 'Yes' values set, which will result in the 'no' value being ignored.");

		if (sample_value.HasMember("In") && sample_value.HasMember("Out"))
			EDebug::Warning("JSON file keyframe has both 'In' and 'Out' values set, which will result in the 'In' value being ignored.");

		if (sample_value.HasMember("Nasal") && sample_value.HasMember("Mouth"))
			EDebug::Warning("JSON file keyframe has both 'Nasal' and 'Mouth' values set, which will result in the 'Nasal' value being ignored.");

		keyframes[itr - keyframesArray.Begin()] = Keyframe(sample, time, method);
	}
}


Pattern::~Pattern()
{
	clear();
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
	return numKeyframes > 0 && keyframes != nullptr && length > 0.0f &&
		   perData_keyframes != nullptr && perData_start != nullptr;
}

float Pattern::toAbsoluteTime(float normTime) const
{
	return normTime * length;
}

float Pattern::toNormalizedTime(float absoluteTime) const
{
	ASSERT(length != 0, 0.0f);
	return absoluteTime / length;
}

int Pattern::getKeyframeIndexAtTime(float normTime) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, 0);
	ASSERT(isValid(), 0);

	int index = numKeyframes - 1;
	for (int i = 0; i < numKeyframes; i++)
	{
		if (keyframes[i].time > normTime)
			break;
		index = i;
	}
	return index;
}

bool Pattern::getKeyframeAtTime(float normTime, int sampleDataIndex, Keyframe*& keyframe) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, 0);
	ASSERT(isValid(), false);

	int start = perData_start[sampleDataIndex];
	int end = perData_start[sampleDataIndex + 1];

	if (end - start == 0)
		return false;

	int index;

	if (end - start == 1)
		index = start;
	else
	{
		index = end - 1;
		for (int i = start; i < end; i++)
		{
			if (perData_keyframes[i]->time > normTime)
				break;
			index = i;
		}
	}

	keyframe = perData_keyframes[index];
	return true;
}

bool Pattern::getBoundingKeyframes(float normTime, int sampleDataIndex, Keyframe*& keyframe1, Keyframe*& keyframe2) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, false);
	ASSERT(isValid(), false);

	int start = perData_start[sampleDataIndex];
	int end = perData_start[sampleDataIndex + 1];

	if (end - start == 0)
		return false;
	else if (end - start == 1)
	{
		keyframe1 = keyframe2 = perData_keyframes[start];
		return true;
	}

	int index = end - 1;

	for (int i = start; i < end; i++)
	{
		if (perData_keyframes[i]->time > normTime)
			break;
		index = i;
	}

	keyframe1 = perData_keyframes[index];

	if (index == end - 1)
		keyframe2 = perData_keyframes[start];
	else keyframe2 = perData_keyframes[index + 1];

	return true;
}

bool Pattern::getBoundingRange(float normTime1, float normTime2, int sampleDataIndex, float &min, float &max) const
{
	ASSERT(normTime1 >= 0.0f && normTime1 <= 1.0f, false);
	ASSERT(normTime2 >= 0.0f && normTime2 <= 1.0f, false);
	ASSERT(isValid(), false);

	min = 1.0f;
	max = 0.0f;

	int start = perData_start[sampleDataIndex];
	int end = perData_start[sampleDataIndex + 1];

	if (end - start == 0)
		return false;
	else if (end - start == 1)
	{
		min = max = perData_keyframes[start]->sample.getValue(sampleDataIndex);
		return true;
	}

	int index1 = end - 1;
	for (int i = start; i < end; i++)
	{
		if (perData_keyframes[i]->time > normTime1)
			break;
		index1 = i;
	}

	int index2 = start;
	for (int i = end - 1; i >= start; i--)
	{
		if (perData_keyframes[i]->time < normTime2)
			break;
		index2 = i;
	}

	int index1_next;
	if (index1 == end - 1)
		index1_next = start;
	else index1_next = index1 + 1;

	float val = interpKeyframes(normTime1, sampleDataIndex, perData_keyframes[index1], perData_keyframes[index1_next]);
	if (val < min) min = val;
	if (val > max) max = val;

	int index2_before;
	if (index2 == start)
		index2_before = end - 1;
	else index2_before = index2 - 1;

	val = interpKeyframes(normTime2, sampleDataIndex, perData_keyframes[index2_before], perData_keyframes[index2]);
	if (val < min) min = val;
	if (val > max) max = val;

	int between_index = index1_next;
	while(true)
	{
		if (between_index == end) // loop around
			between_index = start;
	
		if (between_index == index2)
			break;

		val = perData_keyframes[between_index]->sample.getValue(sampleDataIndex);
		if (val < min) min = val;
		if (val > max) max = val;
		between_index++;
	}

	return true;
}

BreathSample Pattern::getClosestSample(const BreathSample* target, float normTime1, float normTime2) const
{
	ASSERT(isValid(), BreathSample());

	BreathSample result = BreathSample(-1, -1, -1, -1, -1);

	for (int i = 0; i < BreathSample::NUM_VALUES; i++)
	{
		float min;
		float max;
		if (getBoundingRange(normTime1, normTime2, i, min, max))
		{
			if (target->getValue(i) < min)
				result.setValue(i, min);
			else if (target->getValue(i) > max)
				result.setValue(i, max);
			else
				result.setValue(i, target->getValue(i));
		}
	}

	return result;
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

	Keyframe* keyframeIndex1;
	Keyframe* keyframeIndex2;

	if (!getBoundingKeyframes(normTime, sampleDataIndex, keyframeIndex1, keyframeIndex2))
	{
		return false;
	}

	// int keyframeIndex1_index = (int)(keyframeIndex1 - keyframes);
	// int keyframeIndex2_index = (int)(keyframeIndex2 - keyframes);

	value = interpKeyframes(normTime, sampleDataIndex, keyframeIndex1, keyframeIndex2);

	return true;
}

float Pattern::interpKeyframes(float normTime, int sampleDataIndex, const Keyframe* keyframe1, const Keyframe* keyframe2)
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, 0.0f);
	ASSERT(keyframe1 != nullptr, 0.0f);
	ASSERT(keyframe2 != nullptr, 0.0f);
	ASSERT(keyframe1->sample.hasValue(sampleDataIndex), 0.0f);
	ASSERT(keyframe2->sample.hasValue(sampleDataIndex), 0.0f);

	// Override interpolation of constants
	if (sampleDataIndex < BreathSample::NUM_CONSTANTS)
	{
		return keyframe1->sample.getValue(sampleDataIndex);
	}

	if (keyframe1->time == keyframe2->time)
	{
		return keyframe2->sample.getValue(sampleDataIndex);
	}

	float time1 = keyframe1->time;
	float time2 = keyframe2->time;

	// Circular check (i.e., if the next keyframe is before the current keyframe)
	if (time2 < time1)
	{
		time2 += 1.0f;

		if (normTime < time1)
		{
			normTime += 1.0f;
		}
	}

	float value1 = keyframe1->sample.getValue(sampleDataIndex);
	float value2 = keyframe2->sample.getValue(sampleDataIndex);

	float t = (normTime - time1) / (time2 - time1);

	return Interpolators::Interp(value1, value2, t, keyframe1->method);
}

bool Pattern::getBooleanValue(float normTime, int sampleDataIndex, bool& value) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, false);
	ASSERT(isValid(), false);

	Keyframe *keyframe;
	if (getKeyframeAtTime(normTime, sampleDataIndex, keyframe))
	{
		value = keyframe->sample.getValue(sampleDataIndex) >= 0.5f;
		return true;
	}
	else
	{
		return false;
	}
}

BreathSample Pattern::getTarget(float normTime, bool overrideConstants) const
{
	ASSERT(normTime >= 0.0f && normTime <= 1.0f, BreathSample());
	ASSERT(isValid(), BreathSample());

	BreathSample sample;
	// EDebug::Log("Pattern::getTarget: normTime = %f", normTime);

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
				// EDebug::Log("Pattern::getTarget: %d -> %s", i, bool_value ? "true" : "false");
			}
		}

		float float_value;
		for (int i = BreathSample::NUM_CONSTANTS; i < BreathSample::NUM_VALUES; i++)
		{
			if (getInterpolatedValue(normTime, i, float_value))
			{
				sample.setValue(i, float_value);
				// EDebug::Log("Pattern::getTarget: %d -> %.2f", i, float_value);
			}
		}
	}

	return sample;
}

void Pattern::orderKeyframes()
{
	ASSERT(numKeyframes > 0 && keyframes != nullptr && length > 0.0f,);

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

void Pattern::calculatePerDataKeyframes()
{
	ASSERT(numKeyframes > 0 && keyframes != nullptr && length > 0.0f,);

	if (perData_keyframes != nullptr)
	{
		EDebug::Warning("Pattern::calculatePerDataKeyframes: perData_keyframes != nullptr");
		delete[] perData_keyframes;
		perData_keyframes = nullptr;
	}

	std::vector<int> keyframeIndices;
	perData_start = new int[BreathSample::NUM_VALUES + 1];
	perData_start[0] = 0;
	int sum = 0;

	for (int dataIndex = 0; dataIndex < BreathSample::NUM_VALUES; dataIndex++)
	{
		for (int i = 0; i < numKeyframes; i++)
		{
			if (keyframes[i].sample.hasValue(dataIndex))
			{
				keyframeIndices.push_back(i);
				sum++;
			}
		}

		perData_start[dataIndex + 1] = sum;
	}

	perData_keyframes = new Keyframe*[sum];

	for (int dataIndex = 0; dataIndex < BreathSample::NUM_VALUES; dataIndex++)
	{
		int start = perData_start[dataIndex];
		int end = perData_start[dataIndex + 1];

		for (int i = start; i < end; i++)
		{
			perData_keyframes[i] = &keyframes[keyframeIndices[i]];
		}
	}
}

void Pattern::applyHoldValues()
{
	ASSERT(numKeyframes > 0 && keyframes != nullptr && length > 0.0f,);

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