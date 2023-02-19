#pragma once

#include "..\definitions\IntTypes.hpp"
#include "..\definitions\PlatformDefinitions.hpp"
#include "..\definitions\ErrorCodes.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\types\Pattern.hpp"
#include "..\serialization\SerializedTypes.hpp"
#include "..\serialization\ReferenceManager.hpp"

using namespace SerializedTypes;

#define SerializableProperties(className, number, readableName, description, referenceType) \
	className() {} \
	static Serializable *create(); \
	static UInt16 getStaticSerializedClassIndex() { return number; } \
	UInt16 getSerializedClassIndex() override { return number; } \
	static SerializedTypes::ClassDefinition *createClassDefinition() \
	{ \
		return new SerializedTypes::ClassDefinition( \
			readableName, \
			description, \
			referenceType); \
	}

#define CreateSerializable(className) \
	Serializable *className::create() \
	{ \
		return new className(); \
	}

#define SetEnum(enumVariable) \
	if (paramIndex-- == 0) \
		data += Serializable::setEnum((UInt32&)enumVariable, data)

#define SetFlags(flagsVariable) \
	if (paramIndex-- == 0) \
		data += Serializable::setFlags((UInt32&)flagsVariable, data)

#define SetFloat(floatVariable) \
	if (paramIndex-- == 0) \
		data += Serializable::setFloat(floatVariable, data)

#define SetInt(intVariable) \
	if (paramIndex-- == 0) \
		data += Serializable::setInt(intVariable, data)

#define SetBool(boolVariable) \
	if (paramIndex-- == 0) \
		data += Serializable::setBool(boolVariable, data)

#define SetString(stringVariable) \
	if (paramIndex-- == 0) \
		data += Serializable::setString(stringVariable, data)

#define SetReference(referenceVariable) \
	if (paramIndex-- == 0)              \
		referenceVariable.id = *references++

#define SetArray(referenceVariable, arraySize) \
	if (paramIndex-- == 0) \
		data += Serializable::setArray(referenceVariable, data, references)

class CanSerialize
{
protected:
	static void addParameterDefinition(SerializedTypes::ClassDefinition *definition);
	virtual void initialize();
	virtual void setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references) = 0;
};

class Serializable : CanSerialize
{
protected:
	Serializable(){}
	virtual UInt16 getSerializedClassIndex() = 0;

public:
	static SerializedTypes::ClassDefinition *createClassDefinition();

	void setParameters(unsigned char *values, UInt32 length, UInt32* references);


	static int setData(Type type, void *&target, unsigned char *data, UInt32*& references);

	static int setFloat(float& target, unsigned char* data);
	static int setInt(SInt32& target, unsigned char* data);
	static int setBool(bool& target, unsigned char* data);
	static int setEnum(UInt32& target, unsigned char* data);
	static int setFlags(UInt32& target, unsigned char* data);
	static int setString(char*& target, unsigned char* data);
	static int setArray(void*& target, unsigned char* data, UInt32*& references);

	static void validateParameter(const Parameter *param, unsigned char *value);

	static void validateFloatParameter(const Parameter *param, float &value);
	static void validateIntParameter(const Parameter *param, SInt32 &value);
	static void validateBoolParameter(const Parameter *param, bool &value);
	static void validateEnumParameter(const Parameter *param, UInt8 &value);
	static void validateFlagsParameter(const Parameter *param, UInt32 &value);
	static void validateStringParameter(const Parameter *param, unsigned char *value);
	static void validateArrayParameter(const Parameter *param, unsigned char *value);
	static void validateReferenceParameter(const Parameter *param, const void *value);
};