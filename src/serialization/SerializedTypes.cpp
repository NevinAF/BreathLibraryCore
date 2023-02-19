#include "pch.h"
#include "SerializedTypes.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\definitions\IntTypes.hpp"

using namespace SerializedTypes;

// int SerializedTypes::sizeOf(Type type)
// {
// 	switch (type)
// 	{
// 	case SerializedTypes::Float:
// 		return sizeof(float);
// 	case SerializedTypes::Int:
// 		return sizeof(UInt16);
// 	case SerializedTypes::Bool:
// 		return sizeof(bool);
// 	case SerializedTypes::RangeFloat:
// 		return sizeof(float) * 3;
// 	case SerializedTypes::RangeInt:
// 		return sizeof(UInt16) * 3;
// 	case SerializedTypes::Enum:
// 		return sizeof(UInt16) * 2;
// 	case SerializedTypes::Flags:
// 		return sizeof(UInt16) * 2;
// 	default:
// 		EDebug::Error("SerializedTypes::sizeOf: Invalid type");
// 		return 0;
// 	}
// }

std::vector<unsigned char> *currentValue;
std::vector<Parameter> *currentParameters;

Parameter::Parameter(const char *name, const char *tooltip, Type type, UInt32 data0, UInt32 data1, ParameterFlags flags)
{
	strncpy_s(this->name, name, 32);
	strncpy_s(this->tooltip, tooltip, 256);
	this->type = type;
	this->flags = flags;
	this->data0 = data0;
	this->data1 = data1;
}

ClassDefinition::ClassDefinition(const char *className, const char *description, ReferenceType type)
{
	strncpy_s(this->className, className, 32);
	strncpy_s(this->description, description, 256);
	this->type = (UInt16)type;

	if (currentValue == nullptr)
	{
		currentValue = new std::vector<unsigned char>();
		currentParameters = new std::vector<Parameter>();
	}

	if (currentValue->size() != 0 || currentParameters->size() != 0)
	{
		EDebug::Warning("SerializedDefinition::SerializedDefinition: currentValue is not null, meaning that the previous definition was not finalized or had the incorrect number of parameters");

		currentValue->clear();
		currentParameters->clear();
	}

	serializedClassIndex = -1;
	numParams = 0;
}

void ClassDefinition::setSerializedIndex(UInt16 index)
{
	EDebug::Log("Serialized Index: %d, Num Params: %d", index, numParams);
	ASSERT(numParams == 0,)
	serializedClassIndex = index;

	pushValue((unsigned char*)&serializedClassIndex, sizeof(UInt16));
}

void ClassDefinition::freeStaticData()
{
	if (currentValue != nullptr)
	{
		delete currentValue;
		currentValue = nullptr;
	}

	if (currentParameters != nullptr)
	{
		delete currentParameters;
		currentParameters = nullptr;
	}
}

void ClassDefinition::finalize()
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);
	ASSERT(currentParameters->size() < 0xFFFF, );
	numParams = (UInt16) currentParameters->size();

	void* data = malloc(sizeof(Parameter) * numParams);
	memcpy(data, currentParameters->data(), sizeof(Parameter) * numParams);
	params = (const Parameter *)data;

	defaultVal_size = (UInt16)currentValue->size();
	data = malloc(defaultVal_size);
	memcpy(data, currentValue->data(), defaultVal_size);
	defaultValue = (const unsigned char *)data;

	EDebug::Log("Finalized: %s, Num Params: %d, Def Len: %d", className, numParams, defaultVal_size);

	currentValue->clear();
	currentParameters->clear();
}

void ClassDefinition::pushValue(const unsigned char *value, UInt32 size)
{
	for (UInt32 i = 0; i < size; i++)
	{
		currentValue->push_back(value[i]);
	}
}

void ClassDefinition::addFloatDefinition(const char *name, const char *tooltip, float defaultValue, ParameterFlags flags)
{
	addFloatDefinition(name, tooltip, defaultValue, MAXED_FLOAT, MAXED_FLOAT, flags);
}

void ClassDefinition::addFloatDefinition(const char *name, const char *tooltip, float defaultValue, float min, float max, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	currentParameters->push_back(Parameter(name, tooltip, Type::Float, *(UInt32*)&min, *(UInt32*)&max, flags));

	currentValue->push_back((unsigned char)Type::Float);
	pushValue((unsigned char*)&defaultValue, sizeof(float));
}

void ClassDefinition::addIntDefinition(const char *name, const char *tooltip, SInt32 defaultValue, ParameterFlags flags)
{
	addIntDefinition(name, tooltip, defaultValue, MAXED_SINT32, MAXED_SINT32, flags);
}

void ClassDefinition::addIntDefinition(const char *name, const char *tooltip, SInt32 defaultValue, SInt32 min, SInt32 max, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);


	currentParameters->push_back(Parameter(name, tooltip, Type::Int, *(UInt32*)&min, *(UInt32*)&max, flags));

	currentValue->push_back((unsigned char)Type::Int);
	pushValue((unsigned char*)&defaultValue, sizeof(SInt32));
}

void ClassDefinition::addBoolDefinition(const char *name, const char *tooltip, bool defaultValue, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);


	currentParameters->push_back(Parameter(name, tooltip, Type::Bool, 0, 0, flags));

	currentValue->push_back((unsigned char)Type::Bool);
	currentValue->push_back((unsigned char)defaultValue);
}

void ClassDefinition::addEnumDefinition(const char *name, const char *tooltip, UInt8 defaultValue, UInt32 enumId, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	currentParameters->push_back(Parameter(name, tooltip, Type::Enum, enumId, 0, flags));


	currentValue->push_back((unsigned char)Type::Enum);
	currentValue->push_back((unsigned char)defaultValue);
}

void ClassDefinition::addFlagsDefinition(const char *name, const char *tooltip, UInt32 defaultValue, UInt32 enumId, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	currentParameters->push_back(Parameter(name, tooltip, Type::Flags, enumId, 0, flags));

	currentValue->push_back((unsigned char)Type::Flags);
	pushValue((unsigned char*)&defaultValue, sizeof(UInt32));
}

void ClassDefinition::addStringDefinition(const char *name, const char *tooltip, const char *defaultValue, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);
	ASSERT(strlen(defaultValue) <= 0xFFFF, );
	
	UInt16 length = (UInt16) strlen(defaultValue);

	currentParameters->push_back(Parameter(name, tooltip, Type::String, 0, 0, flags));

	currentValue->push_back((unsigned char)Type::String);

	pushValue((unsigned char*)&length, sizeof(UInt16));
	pushValue((unsigned char*)defaultValue, length);
}

void ClassDefinition::addArrayDefinition(const char *name, const char *tooltip, Type type, ParameterFlags flags)
{
	addArrayDefinition(name, tooltip, type, nullptr, 0);
}

void ClassDefinition::addArrayDefinition(const char *name, const char *tooltip, Type type, const unsigned char* defaultValue, UInt32 size, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	currentParameters->push_back(Parameter(name, tooltip, Type::Array, type, 0, flags));

	currentValue->push_back((unsigned char)Type::Array);
	currentValue->push_back((unsigned char)type);
	pushValue((unsigned char*)&size, sizeof(UInt32));

	if (defaultValue != nullptr)
		pushValue(defaultValue, size);
}

void ClassDefinition::addReferenceDefinition(const char *name, const char *tooltip, ReferenceType type, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	currentParameters->push_back(Parameter(name, tooltip, Type::Reference, type, 0, flags));

	currentValue->push_back((unsigned char)Type::Reference);
}