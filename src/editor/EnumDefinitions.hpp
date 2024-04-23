#ifndef ENUMDEFINITIONS_H
#define ENUMDEFINITIONS_H

#include "..\definitions\PlatformDefinitions.hpp"
#include "..\definitions\IntTypes.hpp"

namespace EnumDefinitions
{
	struct EnumValue
	{
		char name[32];
		char tooltip[256];
		UInt8 value;

		EnumValue(const char *name, const char *tooltip, UInt8 value);
	};

	struct EnumDefinition
	{
		char name[32];
		char tooltip[256];
		EnumValue *values;
		UInt16 numValues;

		EnumDefinition(const char *name, const char *tooltip, EnumValue *values, UInt16 numValues);
	};

	extern EnumDefinition **definitions;

	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION LoadEnumDefinitions();
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetEnumDefinition(EnumDefinition** pDef, int index);
}

#endif