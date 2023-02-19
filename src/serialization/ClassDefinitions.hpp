#pragma once

#include "..\definitions\PlatformDefinitions.hpp"
#include "..\definitions\IntTypes.hpp"
#include "SerializedTypes.hpp"
#include "Serializable.hpp"

using namespace SerializedTypes;

constexpr UInt16 BreathStream_serializedIndex = 0;
constexpr UInt16 TestingDetector_serializedIndex = 1;
constexpr UInt16 PatternSampler_serializedIndex = 2;
constexpr UInt16 SampleCorrelator_serializedIndex = 3;
constexpr UInt16 SyncedPatternCorrelator_serializedIndex = 4;
constexpr UInt16 RangeCorrelator_serializedIndex = 5;
constexpr UInt16 MergerSampler_serializedIndex = 6;
constexpr UInt16 Pattern_serializedIndex = 7;

constexpr UInt16 NUMBER_OF_SERIALIZABLE_CLASSES = 8;

namespace ClassDefinitions
{
	typedef Serializable *(*SerializableConstructor)();

	extern ClassDefinition **definitions;
	extern SerializableConstructor *classConstructors;

	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION LoadClassDefinitions(bool includeParameterDefinitions);
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetNumberClasses(UInt16 type);
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetClassDefinition(ClassDefinition** pDef, int index);
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetClassDefinitionOfType(ClassDefinition** pDef, UInt16 type, int index);
}
