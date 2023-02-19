#include "pch.h"

#include "ReferenceManager.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\serialization\ClassDefinitions.hpp"
#include "..\serialization\Serializable.hpp"
#include "..\util\Base64.hpp"
#include <functional>

// template <typename T>
// T *Ref<T>::get()
// {
// 	void *reference;
// 	ReferenceManager::getReference(id, (void*&)reference);
// 	return (T*)reference;
// }

std::vector<void*> references = std::vector<void*>();
std::vector<UInt32> freeReferences = std::vector<UInt32>();

UInt32 ReferenceManager::addReference(void *reference)
{
	UInt32 id;

	if (freeReferences.size() > 0)
	{
		id = freeReferences.back();
		freeReferences.pop_back();
	}
	else
	{
		id = (UInt32)references.size();
		references.push_back(nullptr);
	}

	references[id] = reference;

	return id;
}

void ReferenceManager::removeReference(UInt32 id)
{
	references[id] = nullptr;
	freeReferences.push_back(id);
}

bool ReferenceManager::getReference(UInt32 id, void *& out_reference)
{
	if (id >= references.size())
	{
		out_reference = nullptr;
		return false;
	}

	out_reference = references[id];

	return out_reference != nullptr;
}


extern "C" BREATHLIBRARYCORE_API UInt32 EXTERNAL_CALLING_CONVENTION ReferenceManager::ReserveReferenceIndex()
{
	return ReferenceManager::addReference(nullptr);
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::CreateNativeObject(UInt32 referenceIndex, char* base64Data, UInt32 length, UInt32* class_refs)
{
	if (referenceIndex >= references.size())
		return ErrorCodes::IndexIsOutOfBounds;

#if NDEBUG
	if (references[referenceIndex] != nullptr)
		return ErrorCodes::InvalidParameter;
	for (int i = 0; i < freeReferences.size(); i++)
		if (freeReferences[i] == referenceIndex)
			return ErrorCodes::InvalidParameter;
#endif

	UInt16 classIndex = *(UInt16*)base64Data;
	Serializable *obj = ClassDefinitions::classConstructors[classIndex]();

	unsigned char *data;
	UInt32 len = Base64::decode(base64Data, length, data);
	obj->setParameters(data, len, class_refs);

	references[referenceIndex] = obj;

	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::DestroyNativeObject(UInt32 id)
{
	if (id >= references.size())
		return ErrorCodes::IndexIsOutOfBounds;

#if NDEBUG
	if (references[id] != nullptr)
		return ErrorCodes::InvalidParameter;
	for (int i = 0; i < freeReferences.size(); i++)
		if (freeReferences[i] == id)
			return ErrorCodes::InvalidParameter;
#endif

	void *obj = (void*)references[id];
	if (obj == nullptr)
		return ErrorCodes::InvalidParameter;

	delete obj;
	ReferenceManager::removeReference(id);

	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::UnregisterNativeObject(UInt32 id)
{
	if (id >= references.size())
		return ErrorCodes::IndexIsOutOfBounds;

#if NDEBUG
	if (references[id] != nullptr)
		return ErrorCodes::InvalidParameter;
	for (int i = 0; i < freeReferences.size(); i++)
		if (freeReferences[i] == id)
			return ErrorCodes::InvalidParameter;
#endif

	ReferenceManager::removeReference(id);
	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::GetBreathSample(UInt32 ref_id, BreathSample* out_buffer)
{

	if (ref_id >= references.size())
	{
		*out_buffer = BreathSample();
		return ErrorCodes::IndexIsOutOfBounds;
	}

#if NDEBUG
	if (references[ref_id] != nullptr)
	{
		*out_buffer = BreathSample();
		return ErrorCodes::InvalidParameter;
	}
	for (int i = 0; i < freeReferences.size(); i++)
		if (freeReferences[i] == ref_id)
		{
			*out_buffer = BreathSample();
			return ErrorCodes::InvalidParameter;
		}
#endif

	IBreathSampler *obj = (IBreathSampler*)references[ref_id];
	if (obj == nullptr)
	{
		*out_buffer = BreathSample();
		return ErrorCodes::InvalidParameter;
	}

	*out_buffer = obj->getSample();
	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::GetCorrelation(UInt32 ref_id, float* out_value)
{
	if (ref_id >= references.size())
	{
		*out_value = 0;
		return ErrorCodes::IndexIsOutOfBounds;
	}

#if NDEBUG
	if (references[ref_id] != nullptr)
	{
		*out_value = 0;
		return ErrorCodes::InvalidParameter;
	}
	for (int i = 0; i < freeReferences.size(); i++)
		if (freeReferences[i] == ref_id)
		{
			*out_value = 0;
			return ErrorCodes::InvalidParameter;
		}
#endif

	ICorrelator *obj = (ICorrelator*)references[ref_id];
	if (obj == nullptr)
	{
		*out_value = 0;
		return ErrorCodes::InvalidParameter;
	}

	*out_value = obj->getCorrelation();
	return ErrorCodes::OK;
}