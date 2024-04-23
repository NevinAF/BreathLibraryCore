#ifndef SERIALIZABLE_H
#define SERIALIZABLE_H

#include "..\definitions\IntTypes.hpp"
#include "..\debugging\DebugCallbacks.hpp" // Needed for macros
#include "SerializedTypes.hpp"
#include "..\types\SimpleInterfaces.hpp"

constexpr UInt16 BreathStream_serializedIndex = 0;
constexpr UInt16 TestingDetector_serializedIndex = 1;
constexpr UInt16 PatternSampler_serializedIndex = 2;
constexpr UInt16 SampleCorrelator_serializedIndex = 3;
constexpr UInt16 SyncedPatternCorrelator_serializedIndex = 4;
constexpr UInt16 RangeCorrelator_serializedIndex = 5;
constexpr UInt16 MergeSampler_serializedIndex = 6;
constexpr UInt16 Pattern_serializedIndex = 7;
constexpr UInt16 SimpleAudioSampler_serializedIndex = 8;
constexpr UInt16 TMAudioSampler_serializedIndex = 9;


constexpr UInt16 ManagedObject_serializedIndex = 10;
constexpr UInt16 NUMBER_OF_SERIALIZABLE_CLASSES = ManagedObject_serializedIndex + 1;


constexpr UInt16 Interpolator_Method_serializedIndex = 0;
constexpr UInt16 Behaviour_UpdateMode_serializedIndex = 1;
constexpr UInt16 SyncedPatternCorrelator_MatchMode_serializedIndex = 2;

constexpr UInt16 NUM_ENUM_DEFS = 3;

#define SerializableProperties(className, readableName, description, referenceType)        \
	className() = default;                                                                 \
	static Serializable *create(unsigned char *savedData, unsigned char *runtimeData); \
	inline static ClassDefinition *createClassDefinition()                                 \
	{                                                                                      \
		return new ClassDefinition(                                                        \
			readableName,                                                                  \
			description,                                                                   \
			(ReferenceType)(referenceType));                                               \
	}

#define CreateSerializable(className)                                                         \
	Serializable *className::create(unsigned char *savedData, unsigned char *runtimeData) \
	{                                                                                         \
		className *obj = new className();                                                     \
		obj->setParameters(savedData, runtimeData);                                       \
		obj->initialize();                                                                    \
		return static_cast<Serializable *>(obj);                                              \
	}

#define serializable_SetEnum(enumVariable) \
	if (paramIndex-- == 0) \
		Serializable::setEnum((UInt32&)enumVariable, savedData)

#define serializable_SetFlags(flagsVariable) \
	if (paramIndex-- == 0) \
		Serializable::setFlags((UInt32&)flagsVariable, savedData)

#define serializable_SetFloat(floatVariable) \
	if (paramIndex-- == 0) \
		Serializable::setFloat(floatVariable, savedData)

#define serializable_SetInt(intVariable) \
	if (paramIndex-- == 0) \
		Serializable::setInt(intVariable, savedData)

#define serializable_SetBool(boolVariable) \
	if (paramIndex-- == 0) \
		Serializable::setBool(boolVariable, savedData)

#define serializable_SetString(stringVariable) \
	if (paramIndex-- == 0) \
		Serializable::setString(stringVariable, savedData)

#define serializable_CustomSetString(stringFunction) \
	if (paramIndex-- == 0) { \
		UInt16 size = *(UInt16*)savedData; \
		savedData += sizeof(UInt16); \
		char* filename = new char[size + 1]; \
		memcpy(filename, savedData, size); \
		filename[size] = '\0'; \
		savedData += size; \
		stringFunction(filename); \
		delete filename; \
	}

#define serializable_SetReference(referenceVariable) \
	if (paramIndex-- == 0) {             \
		referenceVariable.id = *(UInt32*)runtimeData; \
		runtimeData += sizeof(UInt32); \
	}

#define serializable_SetArray_Reference(arrayVariable, outSize) \
if (paramIndex-- == 0) { \
outSize = *(UInt32*)savedData; \
savedData += sizeof(UInt32); \
if (arrayVariable != nullptr) \
	delete[] arrayVariable; \
arrayVariable = new Ref[outSize]; \
for (UInt32 i = 0; i < outSize; i++) { \
	arrayVariable[i].id = *(UInt32*)runtimeData; \
	runtimeData += sizeof(UInt32); \
} \
}

#define serializable_SetArray_Bytes(arrayVariable, outSize) \
	if (paramIndex-- == 0) \
		Serializable::setArray_RuntimeBytes(arrayVariable, outSize, runtimeData)

class CanSerialize
{
protected:
	static void addParameterDefinition(ClassDefinition *definition);
	virtual void initialize();
	virtual void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) = 0;
};

class Serializable : public CanSerialize
{
protected:
	Serializable(){};

public:
	virtual ~Serializable() {}

	static ClassDefinition *createClassDefinition();

	void setParameters(unsigned char *savedData, unsigned char *&runtimeData);

	static void setFloat(float &target, unsigned char *&data);
	static void setInt(SInt32& target, unsigned char* &data);
	static void setBool(bool& target, unsigned char* &data);
	static void setEnum(UInt32& target, unsigned char* &data);
	static void setFlags(UInt32& target, unsigned char* &data);
	static void setString(char*& target, unsigned char* &data);
	static void setArray_RuntimeBytes(unsigned char*& target, UInt32& target_size, unsigned char *&data);

	static void validateParameter(const ParameterDefinition *param, unsigned char *value);

	static void validateFloatParameter(const ParameterDefinition *param, float &value);
	static void validateIntParameter(const ParameterDefinition *param, SInt32 &value);
	static void validateBoolParameter(const ParameterDefinition *param, bool &value);
	static void validateEnumParameter(const ParameterDefinition *param, UInt8 &value);
	static void validateFlagsParameter(const ParameterDefinition *param, UInt32 &value);
	static void validateStringParameter(const ParameterDefinition *param, unsigned char *value);
	static void validateArrayParameter(const ParameterDefinition *param, unsigned char *value);
	static void validateReferenceParameter(const ParameterDefinition *param, const void *value);

	virtual BreathSample Sample();
	virtual float Correlation();
};

#endif