#include "pch.h"

#include "Serializable.hpp"
#include "ReferenceManager.hpp"
#include "ClassDefinitions.hpp"

using namespace SerializedTypes;


void CanSerialize::initialize() {}
void CanSerialize::addParameterDefinition(SerializedTypes::ClassDefinition *definition)
{
	EDebug::Error("CanSerialize::addParameterDefinition() called on base class. This must be overridden.");
}

SerializedTypes::ClassDefinition *Serializable::createClassDefinition()
{
	EDebug::Error("Serializable::createClassDefinition() called on base class. This must be overridden.");
	return nullptr;
};


void Serializable::setParameters(unsigned char* values, UInt32 length, UInt32* references)
{
	unsigned char* end = values + length;

	ClassDefinition *classDef = nullptr;
	const Parameter *param = nullptr;

	if (ClassDefinitions::definitions != nullptr)
	{
		ASSERT(getSerializedClassIndex() < NUMBER_OF_SERIALIZABLE_CLASSES,)
		classDef = ClassDefinitions::definitions[getSerializedClassIndex()];

		const Parameter *param = classDef->params;
		ASSERT(param != nullptr,);
	}

	UInt16 paramIndex = 0;

	while (values < end)
	{
		Type type = (Type)*values;
		values++;

		if (classDef != nullptr)
		{
			ASSERT(paramIndex < classDef->numParams && type == param[paramIndex].type,);

			validateParameter(param + paramIndex, values);
		}

		UInt16 pi = paramIndex;
		setParameterIndex(pi, values, references);

		ASSERT(pi > paramIndex,)

		paramIndex++;
	}
}

int Serializable::setData(Type type, void *&target, unsigned char *data, UInt32*& references)
{
	switch (type)
	{
	case Type::Float:
		return setFloat(*(float*)target, data);
	case Type::Int:
		return setInt(*(SInt32*)target, data);
	case Type::Bool:
		return setBool(*(bool*)target, data);
	case Type::Enum:
		return setEnum(*(UInt32*)target, data);
	case Type::Flags:
		return setFlags(*(UInt32*)target, data);
	case Type::String:
		return setString(*(char**)target, data);
	case Type::Array:
		return setArray(target, data, references);
	case Type::Reference:
		return (*(Ref<Serializable>*)target).id = *references++;
	default:
		break;
	}
}

int Serializable::setFloat(float& target, unsigned char* data)
{
	target = *(float*)data;
	return sizeof(float);
}

int Serializable::setInt(SInt32& target, unsigned char* data)
{
	target = *(SInt32*)data;
	return sizeof(SInt32);
}

int Serializable::setBool(bool& target, unsigned char* data)
{
	target = *(bool*)data;
	return sizeof(bool);
}

int Serializable::setEnum(UInt32& target, unsigned char* data)
{
	target = *(UInt8*)data;
	return sizeof(UInt8);
}

int Serializable::setFlags(UInt32& target, unsigned char* data)
{
	target = *(UInt32*)data;
	return sizeof(UInt32);
}

int Serializable::setString(char*& target, unsigned char* data)
{
	UInt32 size = *(UInt32*)data;
	data += sizeof(UInt32);

	if (target != nullptr)
		delete[] target;

	target = new char[size + 1];
	memcpy(target, data, size);
	target[size] = 0;

	return sizeof(UInt32) + size;
}

int Serializable::setArray(void*& target, unsigned char* data, UInt32*& references)
{
	UInt32 size = *(UInt32*)data;
	data += sizeof(UInt32);
	unsigned char* end = data + size;

	if (target != nullptr)
		delete[] target;

	unsigned char *buffer = new unsigned char[size];
	UInt32 dataLen = 0;

	while (data < end)
	{
		Type type = (Type)*data;
		data++;

		UInt32 dataSize = setData(type, target, data, references);
		data += dataSize;
		dataLen += dataSize;
	}

	target = new char[dataLen];
	memcpy(target, data, dataLen);

	return sizeof(UInt32) + size;
}

void Serializable::validateParameter(const Parameter *param, unsigned char *value)
{
	ASSERT(param != nullptr,)

	switch (param->type)
	{
	case Type::Float:
		validateFloatParameter(param, *(float*)value);
		break;
	case Type::Int:
		validateIntParameter(param, *(SInt32*)value);
		break;
	case Type::Bool:
		validateBoolParameter(param, *(bool*)value);
		break;
	case Type::Enum:
		validateEnumParameter(param, *(UInt8*)value);
		break;
	case Type::Flags:
		validateFlagsParameter(param, *(UInt32*)value);
		break;
	case Type::String:
		validateStringParameter(param, value);
		break;
	case Type::Array:
		validateArrayParameter(param, value);
		break;
	}
}

void Serializable::validateFloatParameter(const Parameter *param, float& value)
{
	ASSERT(param != nullptr && param->type == Type::Float,)

	float min = *(float*)&param->data0;
	float max = *(float*)&param->data1;

	if (min != 0xFFFFFFFF && value < min)
		value = min;

	if (max != 0xFFFFFFFF && value > max)
		value = max;
}

void Serializable::validateIntParameter(const Parameter *param, SInt32& value)
{
	ASSERT(param != nullptr && param->type == Type::Int,)

	SInt32 min = *(SInt32*)&param->data0;
	SInt32 max = *(SInt32*)&param->data1;

	if (min != 0xFFFFFFFF && value < min)
		value = min;

	if (max != 0xFFFFFFFF && value > max)
		value = max;
}

void Serializable::validateBoolParameter(const Parameter *param, bool& value)
{
	ASSERT(param != nullptr && param->type == Type::Bool,);

	value = value != 0;
}

void Serializable::validateEnumParameter(const Parameter *param, UInt8& value)
{
	ASSERT(param != nullptr && param->type == Type::Enum,);

	UInt8 numOptions = (UInt8)param->data0;
	if (value >= numOptions)
		value = numOptions - 1;
}

void Serializable::validateFlagsParameter(const Parameter *param, UInt32& value)
{
	ASSERT(param != nullptr && param->type == Type::Flags,);

	UInt32 numOptions = param->data0;
	if (value >= numOptions)
		value = numOptions - 1;
}

void Serializable::validateStringParameter(const Parameter *param, unsigned char* value)
{
	ASSERT(param != nullptr && param->type == Type::String,);
}

void Serializable::validateArrayParameter(const Parameter *param, unsigned char* value)
{
	ASSERT(param != nullptr && param->type == Type::Array,);

	Type type = (Type)param->data0;

	// Todo: Maybe some validation on the size/data in the array?
}

void Serializable::validateReferenceParameter(const Parameter *param, const void *value)
{
	ASSERT(param != nullptr && param->type == Type::Reference,);
}