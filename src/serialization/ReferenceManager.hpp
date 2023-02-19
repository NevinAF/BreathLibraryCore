#pragma once

#include "..\types\BreathSample.hpp"
#include "..\definitions\IntTypes.hpp"
#include "..\definitions\PlatformDefinitions.hpp"
#include <vector>

namespace ReferenceManager
{
	// private?
	// extern std::vector<UInt64, void*> references;

	UInt32 addReference(void *reference); 

	void removeReference(UInt32 id);

	bool getReference(UInt32 id, void *& out_reference);

	extern "C" BREATHLIBRARYCORE_API UInt32 EXTERNAL_CALLING_CONVENTION ReserveReferenceIndex();

	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION CreateNativeObject(UInt32 referenceIndex, char* base64Data, UInt32 length, UInt32* class_refs);
	
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION UnregisterNativeObject(UInt32 id);

	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION DestroyNativeObject(UInt32 id);

	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetBreathSample(UInt32 ref_id, BreathSample* out_buffer);
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetCorrelation(UInt32 ref_id, float* out_value);


}

template <typename T>
class Ref
{
public:
	UInt32 id{0xFFFFFFFF};

	T *get()
	{
		void *reference;
		ReferenceManager::getReference(id, (void*&)reference);
		return (T*)reference;
	}
};