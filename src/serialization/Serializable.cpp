
#include "Serializable.hpp"
#include "..\debugging\DebugCallbacks.hpp"


void CanSerialize::initialize() {}
void CanSerialize::addParameterDefinition(ClassDefinition *definition)
{
	EDebug::Error("CanSerialize::addParameterDefinition() called on base class. This must be overridden.");
}

ClassDefinition *Serializable::createClassDefinition()
{
	EDebug::Error("Serializable::createClassDefinition() called on base class. This must be overridden.");
	return nullptr;
};


void Serializable::setParameters(unsigned char* values, unsigned char *&runtimeData)
{
	UInt16 paramIndex = 0;
	UInt16 pi = 0;

	while (pi != 0xFFFF)
	{
		pi = paramIndex;
		setParameterIndex(pi, values, runtimeData);

		ASSERT_MSG(pi > paramIndex,, "Set Parameters found index out of bounds! Class definition or serialization is not set up properly.")

		paramIndex++;
	}
}

void Serializable::setFloat(float& target, unsigned char* &data)
{
	target = *(float*)data;
	data += sizeof(float);
}

void Serializable::setInt(SInt32& target, unsigned char* &data)
{
	target = *(SInt32*)data;
	data += sizeof(SInt32);
}

void Serializable::setBool(bool& target, unsigned char* &data)
{
	target = *(bool*)data;
	data += sizeof(bool);
}

void Serializable::setEnum(UInt32& target, unsigned char* &data)
{
	target = *(UInt8*)data;
	data += sizeof(UInt8);
}

void Serializable::setFlags(UInt32& target, unsigned char* &data)
{
	target = *(UInt32*)data;
	data += sizeof(UInt32);
}

void Serializable::setString(char*& target, unsigned char* &data)
{
	UInt16 size = *(UInt16*)data;
	data += sizeof(UInt16);

	if (target != nullptr)
		delete[] target;

	target = new char[size + 1];
	memcpy(target, data, size);
	target[size] = 0;

	data += size;
}

void Serializable::setArray_RuntimeBytes(unsigned char*& target, UInt32& target_size, unsigned char* &data)
{
	target_size = *(UInt32*)data;
	data += sizeof(UInt32);

	if (target != nullptr)
		delete[] target;

	target = new unsigned char[target_size];
	memcpy(target, data, target_size);

	data += target_size;
}

// int Serializable::setArray(void*& target, unsigned char* data, UInt32*& references)
// {
// 	UInt32 size = *(UInt32*)data;
// 	data += sizeof(UInt32);
// 	unsigned char* end = data + size;

// 	if (target != nullptr)
// 		delete[] target;

// 	unsigned char *buffer = new unsigned char[size];
// 	UInt32 dataLen = 0;

// 	while (data < end)
// 	{
// 		ValueType type = (ValueType)*data;
// 		data++;

// 		UInt32 dataSize = setData(type, target, data, references);
// 		data += dataSize;
// 		dataLen += dataSize;
// 	}

// 	target = new char[dataLen];
// 	memcpy(target, data, dataLen);

// 	return sizeof(UInt32) + size;
// }

void Serializable::validateParameter(const ParameterDefinition *param, unsigned char *value)
{
	ASSERT(param != nullptr,)

	// EDebug::Warning("Validating parameter of type %d", param->type);

	switch (param->type)
	{
	case ValueType::Float:
		validateFloatParameter(param, *(float*)value);
		break;
	case ValueType::Int:
		validateIntParameter(param, *(SInt32*)value);
		break;
	case ValueType::Bool:
		validateBoolParameter(param, *(bool*)value);
		break;
	case ValueType::Enum:
		validateEnumParameter(param, *(UInt8*)value);
		break;
	case ValueType::Flags:
		validateFlagsParameter(param, *(UInt32*)value);
		break;
	case ValueType::String:
		validateStringParameter(param, value);
		break;
	case ValueType::Array:
		validateArrayParameter(param, value);
		break;
	}
}

void Serializable::validateFloatParameter(const ParameterDefinition *param, float& value)
{
	ASSERT(param != nullptr && param->type == ValueType::Float,)

	float min = *(float*)&param->data0;
	float max = *(float*)&param->data1;

	if (min != 0xFFFFFFFF && value < min)
		value = min;

	if (max != 0xFFFFFFFF && value > max)
		value = max;
}

void Serializable::validateIntParameter(const ParameterDefinition *param, SInt32& value)
{
	ASSERT(param != nullptr && param->type == ValueType::Int,)

	SInt32 min = *(SInt32*)&param->data0;
	SInt32 max = *(SInt32*)&param->data1;

	if (min != 0xFFFFFFFF && value < min)
		value = min;

	if (max != 0xFFFFFFFF && value > max)
		value = max;
}

void Serializable::validateBoolParameter(const ParameterDefinition *param, bool& value)
{
	ASSERT(param != nullptr && param->type == ValueType::Bool,);

	value = value != 0;
}

void Serializable::validateEnumParameter(const ParameterDefinition *param, UInt8& value)
{
	ASSERT(param != nullptr && param->type == ValueType::Enum,);

	// EDebug::Warning("Validating enum, %p, %d", &value, param->data0);

	UInt8 numOptions = (UInt8)param->data0;
	if (value >= numOptions)
		value = numOptions - 1;
}

void Serializable::validateFlagsParameter(const ParameterDefinition *param, UInt32& value)
{
	ASSERT(param != nullptr && param->type == ValueType::Flags,);

	UInt32 numOptions = param->data0;
	if (value >= numOptions)
		value = numOptions - 1;
}

void Serializable::validateStringParameter(const ParameterDefinition *param, unsigned char* value)
{
	ASSERT(param != nullptr && param->type == ValueType::String,);
}

void Serializable::validateArrayParameter(const ParameterDefinition *param, unsigned char* value)
{
	ASSERT(param != nullptr && param->type == ValueType::Array,);

	// ValueType type = (ValueType)param->data0;

	// Todo: Maybe some validation on the size/data in the array?
}

void Serializable::validateReferenceParameter(const ParameterDefinition *param, const void *value)
{
	ASSERT(param != nullptr && param->type == ValueType::Reference,);
}

BreathSample Serializable::Sample() {
	EDebug::Warning("Serializable::Sample has not been defined on an object being used as a BreathSampler. This is likely a bug.");
	return BreathSample();
}
float Serializable::Correlation() {
	EDebug::Warning("Serializable::Correlation has not been defined on an object being used as a Correlator. This is likely a bug.");
	return -1.0f;
}