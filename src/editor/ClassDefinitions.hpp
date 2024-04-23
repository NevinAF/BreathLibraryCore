#ifndef CLASSDEFINITIONS_H
#define CLASSDEFINITIONS_H

#include "..\definitions\PlatformDefinitions.hpp"
#include "..\definitions\IntTypes.hpp"
#include "..\serialization\Serializable.hpp"
#include "..\serialization\SerializedTypes.hpp"

namespace ClassDefinitions
{
	bool isClassDefinitionsLoaded();
	Serializable *createObject(UInt16 index);
	ClassDefinition *getClassDefinition(UInt16 index);

	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION LoadClassDefinitions();
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetNumberClasses(UInt16 type);
	extern "C" BREATHLIBRARYCORE_API char* EXTERNAL_CALLING_CONVENTION GetClassDefinitionName(UInt16 index);
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetClassDefinition(ClassDefinition** pDef, int index);
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetClassDefinitionOfType(ClassDefinition** pDef, UInt16 type, int index);
}

#endif