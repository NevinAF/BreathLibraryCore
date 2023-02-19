#include "..\definitions\PlatformDefinitions.hpp"
#include "..\definitions\IntTypes.hpp"

constexpr UInt16 Interpolator_Method_serializedIndex = 0;
constexpr UInt16 Behaviour_UpdateMode_serializedIndex = 1;
constexpr UInt16 SyncedPatternCorrelator_MatchMode_serializedIndex = 2;

constexpr UInt16 NUM_ENUM_DEFS = 3;

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