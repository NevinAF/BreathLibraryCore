#ifndef SERIALIZEDTYPES_H
#define SERIALIZEDTYPES_H

#include <vector>
#include "..\util\Base64.hpp"
#include "..\definitions\IntTypes.hpp"

constexpr UInt32 MAXED_VALUE = 0xFFFFFFFF;
#define MAXED_FLOAT *(float*)&MAXED_VALUE
#define MAXED_SINT32 *(SInt32*)&MAXED_VALUE

enum ValueType
{
	Float,
	Int,
	Bool,
	Enum,
	Flags,
	String,
	Array,
	Reference,
	BinaryFile,
	Custom,
};

enum ReferenceType
{
	REF_NONE = 0,
	REF_Sampler = 1,
	REF_Correlator = 2,
	REF_Pattern = 4,
	REF_Stream = 8,
	REF_Behaviour = 16,
	REF_DataStream = 32,

	__REF_ALL__ = REF_Sampler | REF_Correlator | REF_Pattern | REF_Stream | REF_Behaviour | REF_DataStream
};

enum ParameterFlags
{
	PAR_None = 0,
	PAR_Hidden = 1,
	PAR_ReadOnly = 2,
	PAR_ReadOnlyWhilePlaying = 4,
};

struct ParameterDefinition
{
	char name[32];
	char tooltip[256];
	UInt8 type;
	UInt8 flags;
	UInt32 data0;
	UInt32 data1;

	ParameterDefinition(const char *name, const char *tooltip, ValueType type, UInt32 data0, UInt32 data1, ParameterFlags flags = ParameterFlags::PAR_None);
};

struct ClassDefinition
{
private:
	void pushValue(const unsigned char *value, UInt32 size);

public:
	static void freeStaticData();

	UInt16 serializedClassIndex;
	char className[32];
	char description[256];

	UInt16 type;
	UInt16 numParams;

	const ParameterDefinition* params;
	const unsigned char *defaultValue;

	UInt16 defaultVal_size;

	ClassDefinition(const char *className, const char *description, ReferenceType type);
	void setSerializedIndex(UInt16 index);

	void addFloatDefinition(const char *name, const char *tooltip, float defaultValue, ParameterFlags flags = PAR_None);
	void addFloatDefinition(const char *name, const char *tooltip, float defaultValue, float min, float max, ParameterFlags flags = PAR_None);
	void addIntDefinition(const char *name, const char *tooltip, SInt32 defaultValue, ParameterFlags flags = PAR_None);
	void addIntDefinition(const char *name, const char *tooltip, SInt32 defaultValue, SInt32 min, SInt32, ParameterFlags flags = PAR_None);
	void addBoolDefinition(const char *name, const char *tooltip, bool defaultValue, ParameterFlags flags = PAR_None);
	void addEnumDefinition(const char *name, const char *tooltip, UInt8 defaultValue, UInt32 enumId, ParameterFlags flags = PAR_None);
	void addFlagsDefinition(const char *name, const char *tooltip, UInt32 defaultValue, UInt32 enumId, ParameterFlags flags = PAR_None);
	void addStringDefinition(const char *name, const char *tooltip, const char *defaultValue, ParameterFlags flags = PAR_None);
	void addArrayDefinition(const char *name, const char *tooltip, ValueType type, ParameterFlags flags = PAR_None);
	void addArrayDefinition(const char *name, const char *tooltip, ValueType type, const unsigned char *defaultValues, UInt32 size, ParameterFlags flags = PAR_None);
	void addReferenceDefinition(const char *name, const char *tooltip, ReferenceType refType, ParameterFlags flags = PAR_None);
	void addFileDefinition(const char *name, const char *tooltip, const char* fileExtention, ParameterFlags flags = PAR_None);

	void finalize();
};

#endif