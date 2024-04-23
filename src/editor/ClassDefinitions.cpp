
#include "ClassDefinitions.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\definitions\ErrorCodes.hpp"

#include "..\detectors\PatternSampler.hpp"
#include "..\detectors\TestingDetector.hpp"
#include "..\detectors\MergeSampler.hpp"
#include "..\types\BreathStream.hpp"
#include "..\types\Pattern.hpp"
#include "..\correlators\SampleCorrelator.hpp"
#include "..\correlators\SyncedPatternCorrelator.hpp"
#include "..\correlators\RangeCorrelator.hpp"
#include "..\detectors\SimpleAudioSampler.hpp"
#include "..\detectors\TMAudioSampler.hpp"

#define LoadClassDef(className) \
	if (definitions[className##_serializedIndex] != nullptr) { \
		EDebug::Warning("ClassDefinitions::LoadClassDefinitions: Class definition already loaded at index %d where class " #className " is trying to load. Aborting...", className##_serializedIndex); \
		return ErrorCodes::InternalAssertionFailure; \
	} \
	definitions[className##_serializedIndex] = className::createClassDefinition(); \
	definitions[className##_serializedIndex]->setSerializedIndex(className##_serializedIndex); \
	className::addParameterDefinition(definitions[className##_serializedIndex]); \
	definitions[className##_serializedIndex]->finalize()

ClassDefinition **definitions = nullptr;

ClassDefinition *ClassDefinitions::getClassDefinition(UInt16 index)
{
	if (index >= NUMBER_OF_SERIALIZABLE_CLASSES - 1)
	{
		EDebug::Warning("ClassDefinitions::getClassDefinition: Index %d is out of range.", index);
		return nullptr;
	}

	if (definitions == nullptr)
	{
		EDebug::Warning("ClassDefinitions::getClassDefinition: Class definitions not loaded. Loading now...");
		LoadClassDefinitions();
	}

	if (definitions[index] == nullptr)
	{
		EDebug::Warning("ClassDefinitions::getClassDefinition: No class definition found for index %d.", index);
		return nullptr;
	}

	return definitions[index];
}

bool ClassDefinitions::isClassDefinitionsLoaded()
{
	return definitions != nullptr;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ClassDefinitions::LoadClassDefinitions()
{
	if (definitions != nullptr)
	{
		EDebug::Log("ClassDefinitions::LoadClassDefinitions: Class definitions already loaded.");
		return ErrorCodes::OK;
	}

	definitions = new ClassDefinition*[NUMBER_OF_SERIALIZABLE_CLASSES - 1];
	std::fill(definitions, definitions + NUMBER_OF_SERIALIZABLE_CLASSES - 1, nullptr);

	LoadClassDef(BreathStream);
	LoadClassDef(TestingDetector);
	LoadClassDef(PatternSampler);
	LoadClassDef(SampleCorrelator);
	LoadClassDef(SyncedPatternCorrelator);
	LoadClassDef(RangeCorrelator);
	LoadClassDef(MergeSampler);
	LoadClassDef(Pattern);
	LoadClassDef(SimpleAudioSampler);
	LoadClassDef(TMAudioSampler);

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
			ClassDefinitions::LoadClassDefinitions();
		}

		lastType = type;
		lastNumClasses = 0;

		for (int i = 0; i < NUMBER_OF_SERIALIZABLE_CLASSES - 1; i++)
		{
			ASSERT(definitions[i] != nullptr, -ErrorCodes::InternalAssertionFailure)

			UInt16 classType = definitions[i]->type;
			ASSERT(classType <= ReferenceType::__REF_ALL__, -ErrorCodes::InvalidParameter);

			if ((classType & type) == type)
			{
				// EDebug::Log("ClassDefinitions::GetNumberClasses: Class %s is of type %d", definitions[i]->className, classType);
				lastNumClasses++;
			}
			else 
			{
				// EDebug::Log("ClassDefinitions::GetNumberClasses: Class %s is not of type %d", definitions[i]->className, classType);
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
		ClassDefinitions::LoadClassDefinitions();
	}

	if (index < 0 || index >= NUMBER_OF_SERIALIZABLE_CLASSES - 1)
		return ErrorCodes::IndexIsOutOfBounds;

	if (definitions[index] == nullptr)
		return -ErrorCodes::InternalAssertionFailure;

	*pDef = definitions[index];

	// EDebug::Log("Returning:\n\tIndex: %d\n\tName: %s\n\tType: %d\n\tnumparam: %d\n\tDefinition: %p",
	// 			index,
	// 			(*pDef)->className,
	// 			(*pDef)->type,
	// 			(*pDef)->numParams,
	// 			(*pDef));
	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API char* EXTERNAL_CALLING_CONVENTION GetClassDefinitionName(UInt16 index)
{
	static char noEditorName[32] = {"Native Object"};
	static char errorName[32] = {"Unknown Class"};

	if (definitions == nullptr)
	{
		return noEditorName;
	}

	ClassDefinition *def = ClassDefinitions::getClassDefinition(index);
	if (def == nullptr || def->className[0] == '\0')
	{
		return errorName;
	}

	return def->className;
}


extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ClassDefinitions::GetClassDefinitionOfType(ClassDefinition* *pDef, UInt16 type, int index)
{
	static UInt16 last_definition_index = -1;
	static UInt16 lastIndex = -1;

	if (definitions == nullptr)
	{
		EDebug::Warning("ClassDefinitions::GetNumberClasses: Class definitions not loaded yet. Loading now...");
		ClassDefinitions::LoadClassDefinitions();
	}

	if (last_definition_index != ((UInt16)-1) && definitions[last_definition_index]->type == type && lastIndex == index)
	{
		*pDef = definitions[last_definition_index];
		return ErrorCodes::OK;
	}

	last_definition_index = index;
	for (int i = 0; i < NUMBER_OF_SERIALIZABLE_CLASSES - 1; i++)
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