#include "pch.h"
#include "ClassDefinitions.hpp"
#include "..\debugging\DebugCallbacks.hpp"

#include "..\detectors\PatternSampler.hpp"
#include "..\detectors\TestingDetector.hpp"
#include "..\detectors\MergeSampler.hpp"
#include "..\types\BreathStream.hpp"
#include "..\types\Pattern.hpp"
#include "..\correlators\SampleCorrelator.hpp"
#include "..\correlators\SyncedPatternCorrelator.hpp"
#include "..\correlators\RangeCorrelator.hpp"

using namespace SerializedTypes;

#define LoadClass(className) \
	EDebug::Log("ClassDefinitions::LoadClassDefinitions: Loading class " #className " at index %d", className::getStaticSerializedClassIndex()); \
	if (classConstructors[className::getStaticSerializedClassIndex()] != nullptr) { \
		EDebug::Warning("ClassDefinitions::LoadClassDefinitions: Class definition already loaded at index %d where class " #className " is trying to load. Aborting...", className::getStaticSerializedClassIndex()); \
		return ErrorCodes::InternalAssertionFailure; \
	} \
	if (includeParameterDefinitions) { \
		definitions[className::getStaticSerializedClassIndex()] = className::createClassDefinition(); \
		definitions[className::getStaticSerializedClassIndex()]->setSerializedIndex(className::getStaticSerializedClassIndex());\
		className::addParameterDefinition(definitions[className::getStaticSerializedClassIndex()]); \
		definitions[className::getStaticSerializedClassIndex()]->finalize(); \
	} \
	classConstructors[className::getStaticSerializedClassIndex()] = &className::create

ClassDefinition **ClassDefinitions::definitions = nullptr;
ClassDefinitions::SerializableConstructor *ClassDefinitions::classConstructors = nullptr;

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ClassDefinitions::LoadClassDefinitions(bool includeParameterDefinitions)
{
	if (definitions != nullptr && classConstructors != nullptr)
	{
		EDebug::Log("ClassDefinitions::LoadClassDefinitions: Class definitions already loaded.");
		return ErrorCodes::OK;
	}

	if (includeParameterDefinitions)
	{
		definitions = new ClassDefinition*[NUMBER_OF_SERIALIZABLE_CLASSES];
		std::fill(definitions, definitions + NUMBER_OF_SERIALIZABLE_CLASSES, nullptr);
	}

	classConstructors = new SerializableConstructor[NUMBER_OF_SERIALIZABLE_CLASSES];
	std::fill(classConstructors, classConstructors + NUMBER_OF_SERIALIZABLE_CLASSES, nullptr);

	ASSERT(classConstructors[0] == nullptr, ErrorCodes::InternalAssertionFailure);
	ASSERT(classConstructors[1] == nullptr, ErrorCodes::InternalAssertionFailure);

	LoadClass(BreathStream);
	LoadClass(TestingDetector);
	LoadClass(PatternSampler);
	LoadClass(SampleCorrelator);
	LoadClass(SyncedPatternCorrelator);
	LoadClass(RangeCorrelator);
	LoadClass(MergeSampler);
	// LoadClass(Pattern);

	for (int i = 0; i < NUMBER_OF_SERIALIZABLE_CLASSES; i++)
	{
		EDebug::Log("%s Loaded:\n\tIndex: %d\n\tName: %s\n\tType: %d\n\tnumparam: %d\n\tConstructor: %p\n\tDefinition: %p",
			(classConstructors[i] == nullptr) ? "Class" : "ClassDefinition",
			i,
			(classConstructors[i] == nullptr) ? "N/A" : definitions[i]->className,
			(classConstructors[i] == nullptr) ? 0 : definitions[i]->type,
			(classConstructors[i] == nullptr) ? 0 : definitions[i]->numParams,
			classConstructors[i],
			(classConstructors[i] == nullptr) ? nullptr : definitions[i]);
	}

	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ClassDefinitions::GetNumberClasses(UInt16 type)
{
	static int lastType = -1;
	static int lastNumClasses = -1;

	if (lastType != type || lastNumClasses == -1)
	{
		if (definitions == nullptr)
		{
			EDebug::Warning("ClassDefinitions::GetNumberClasses: Class definitions not loaded yet. Loading now...");
			ClassDefinitions::LoadClassDefinitions(true);
		}

		lastType = type;
		lastNumClasses = 0;

		for (int i = 0; i < NUMBER_OF_SERIALIZABLE_CLASSES; i++)
		{
			UInt16 classType = definitions[i]->type;
			ASSERT(classType < ReferenceType::__REF_ALL__, -ErrorCodes::InvalidParameter);

			if ((classType & type) == type)
			{
				EDebug::Log("ClassDefinitions::GetNumberClasses: Class %s is of type %d", definitions[i]->className, classType);
				lastNumClasses++;
			}
			else 
			{
				EDebug::Log("ClassDefinitions::GetNumberClasses: Class %s is not of type %d", definitions[i]->className, classType);
			}
		}
	}

	return lastNumClasses;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ClassDefinitions::GetClassDefinition(ClassDefinition* *pDef, int index)
{
	if (definitions == nullptr)
	{
		EDebug::Warning("ClassDefinitions::GetNumberClasses: Class definitions not loaded yet. Loading now...");
		ClassDefinitions::LoadClassDefinitions(true);
	}

	if (index < 0 || index >= NUMBER_OF_SERIALIZABLE_CLASSES)
		return ErrorCodes::IndexIsOutOfBounds;

	*pDef = definitions[index];

	EDebug::Log("Returning:\n\tIndex: %d\n\tName: %s\n\tType: %d\n\tnumparam: %d\n\tDefinition: %p",
				index,
				(*pDef)->className,
				(*pDef)->type,
				(*pDef)->numParams,
				(*pDef));
	return ErrorCodes::OK;
}


extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ClassDefinitions::GetClassDefinitionOfType(ClassDefinition* *pDef, UInt16 type, int index)
{
	static UInt16 last_definition_index = -1;
	static UInt16 lastIndex = -1;

	if (definitions == nullptr)
	{
		EDebug::Warning("ClassDefinitions::GetNumberClasses: Class definitions not loaded yet. Loading now...");
		ClassDefinitions::LoadClassDefinitions(true);
	}

	if (last_definition_index != -1 && definitions[last_definition_index]->type == type && lastIndex == index)
	{
		*pDef = definitions[last_definition_index];
		return ErrorCodes::OK;
	}

	last_definition_index = index;
	for (int i = 0; i < NUMBER_OF_SERIALIZABLE_CLASSES; i++)
	{
		if (definitions[i]->type == type)
		{
			if (index == 0)
			{
				*pDef = definitions[i];
				lastIndex = index;
				return ErrorCodes::OK;
			}
			else
			{
				index--;
			}
		}
	}

	last_definition_index = -1;
	return ErrorCodes::InvalidParameter;
}