#include "SerializedTypes.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\definitions\IntTypes.hpp"

std::vector<unsigned char> *currentValue;
std::vector<ParameterDefinition> *currentParameters;

ParameterDefinition::ParameterDefinition(const char *name, const char *tooltip, ValueType type, UInt32 data0, UInt32 data1, ParameterFlags flags)
{
	strncpy(this->name, name, 32);
	strncpy(this->tooltip, tooltip, 256);
	this->type = type;
	this->flags = flags;
	this->data0 = data0;
	this->data1 = data1;
}

ClassDefinition::ClassDefinition(const char *className, const char *description, ReferenceType type)
{
	strncpy(this->className, className, 32);
	strncpy(this->description, description, 256);
	this->type = (UInt16)type;

	if (currentValue == nullptr)
	{
		currentValue = new std::vector<unsigned char>();
		currentParameters = new std::vector<ParameterDefinition>();
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

	void* data = malloc(sizeof(ParameterDefinition) * numParams);
	memcpy(data, currentParameters->data(), sizeof(ParameterDefinition) * numParams);
	params = (const ParameterDefinition *)data;

	defaultVal_size = (UInt16)currentValue->size();
	data = malloc(defaultVal_size);
	memcpy(data, currentValue->data(), defaultVal_size);
	defaultValue = (const unsigned char *)data;

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

	currentParameters->push_back(ParameterDefinition(name, tooltip, ValueType::Float, *(UInt32*)&min, *(UInt32*)&max, flags));

	pushValue((unsigned char*)&defaultValue, sizeof(float));
}

void ClassDefinition::addIntDefinition(const char *name, const char *tooltip, SInt32 defaultValue, ParameterFlags flags)
{
	addIntDefinition(name, tooltip, defaultValue, MAXED_SINT32, MAXED_SINT32, flags);
}

void ClassDefinition::addIntDefinition(const char *name, const char *tooltip, SInt32 defaultValue, SInt32 min, SInt32 max, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);


	currentParameters->push_back(ParameterDefinition(name, tooltip, ValueType::Int, *(UInt32*)&min, *(UInt32*)&max, flags));

	pushValue((unsigned char*)&defaultValue, sizeof(SInt32));
}

void ClassDefinition::addBoolDefinition(const char *name, const char *tooltip, bool defaultValue, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);


	currentParameters->push_back(ParameterDefinition(name, tooltip, ValueType::Bool, 0, 0, flags));

	currentValue->push_back((unsigned char)defaultValue);
}

void ClassDefinition::addEnumDefinition(const char *name, const char *tooltip, UInt8 defaultValue, UInt32 enumId, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	currentParameters->push_back(ParameterDefinition(name, tooltip, ValueType::Enum, enumId, 0, flags));

	currentValue->push_back((unsigned char)defaultValue);
}

void ClassDefinition::addFlagsDefinition(const char *name, const char *tooltip, UInt32 defaultValue, UInt32 enumId, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	currentParameters->push_back(ParameterDefinition(name, tooltip, ValueType::Flags, enumId, 0, flags));

	pushValue((unsigned char*)&defaultValue, sizeof(UInt32));
}

void ClassDefinition::addStringDefinition(const char *name, const char *tooltip, const char *defaultValue, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);
	ASSERT(strlen(defaultValue) <= 0xFFFF, );
	
	UInt16 length = (UInt16) strlen(defaultValue);

	currentParameters->push_back(ParameterDefinition(name, tooltip, ValueType::String, 0, 0, flags));

	pushValue((unsigned char*)&length, sizeof(UInt16));
	pushValue((unsigned char*)defaultValue, length);
}

void ClassDefinition::addFileDefinition(const char *name, const char *tooltip, const char *fileExtension, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	UInt32 extention_part1 = 0;
	UInt32 extention_part2 = 0;

	if (fileExtension != nullptr)
	{
		size_t len = strlen(fileExtension);
		ASSERT(len <= 8,);

		if (len > 4)
		{
			memcpy(&extention_part1, fileExtension, 4);
			memcpy(&extention_part2, fileExtension + 4, len - 4);
		}
		else
		{
			memcpy(&extention_part1, fileExtension, len);
		}
	}

	currentParameters->push_back(ParameterDefinition(name, tooltip, ValueType::BinaryFile, extention_part1, extention_part2, flags));
}

void ClassDefinition::addArrayDefinition(const char *name, const char *tooltip, ValueType type, ParameterFlags flags)
{
	addArrayDefinition(name, tooltip, type, nullptr, 0);
}

void ClassDefinition::addArrayDefinition(const char *name, const char *tooltip, ValueType type, const unsigned char* defaultValue, UInt32 size, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	currentParameters->push_back(ParameterDefinition(name, tooltip, ValueType::Array, type, 0, flags));

	currentValue->push_back((unsigned char)type);
	pushValue((unsigned char*)&size, sizeof(UInt32));

	if (defaultValue != nullptr)
		pushValue(defaultValue, size);
}

void ClassDefinition::addReferenceDefinition(const char *name, const char *tooltip, ReferenceType type, ParameterFlags flags)
{
	ASSERT(currentValue != nullptr && currentParameters != nullptr,);

	currentParameters->push_back(ParameterDefinition(name, tooltip, ValueType::Reference, type, 0, flags));
}