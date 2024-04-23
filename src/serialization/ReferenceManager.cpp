
#include "ReferenceManager.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\serialization\Serializable.hpp"
#include "..\definitions\ErrorCodes.hpp"
#include "..\util\Base64.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\types\Behaviour.hpp"
#include <functional>

#include "..\detectors\PatternSampler.hpp"
#include "..\detectors\TestingDetector.hpp"
#include "..\detectors\MergeSampler.hpp"
#include "..\types\BreathStream.hpp"
#include "..\types\Pattern.hpp"
#include "..\correlators\SampleCorrelator.hpp"
#include "..\correlators\SyncedPatternCorrelator.hpp"
#include "..\correlators\RangeCorrelator.hpp"
#include "..\detectors\ManagedObject.hpp"
#include "..\detectors\SimpleAudioSampler.hpp"
#include "..\detectors\TMAudioSampler.hpp"

#ifdef NDEBUG
#define VALIDATE_REF_EXISTS(id) \
	ASSERT_MSG(getReferenceObjects() != nullptr, ErrorCodes::InvalidState, "VALIDATE_REF_EXISTS: reference_objects is null! (Did you call OnRuntimeInitialized()?)") \
	ASSERT_MSG(getFreeReferences() != nullptr, ErrorCodes::InvalidState, "VALIDATE_REF_EXISTS: free_references is null! (Did you call OnRuntimeInitialized()?)") \
	ASSERT_MSG(class_functions != nullptr, ErrorCodes::InvalidState, "VALIDATE_REF_EXISTS: class_functions is null! (Did you call OnRuntimeInitialized()?)") \
	ASSERT_MSG(id < getReferenceObjects()->size(), ErrorCodes::IndexIsOutOfBounds, "VALIDATE_REF_EXISTS: reference index %d is out of bounds (size: %d)!", id, getReferenceObjects()->size()) \
	ASSERT_MSG((*getReferenceObjects())[id].reference != nullptr && (*getReferenceObjects())[id].functions != nullptr, ErrorCodes::InvalidParameter, "VALIDATE_REF_EXISTS: reference index %d is null!", id) \
	for (int i = 0; i < getFreeReferences()->size(); i++) \
	{ \
		ASSERT_MSG((*getFreeReferences())[i] != id, ErrorCodes::InvalidState, "VALIDATE_REF_EXISTS: reference index %d is currently a freed id!", id) \
	}
#else
#define VALIDATE_REF_EXISTS(id)
#endif

#ifdef NDEBUG
#define VALIDATE_REF_REG(id) \
	ASSERT_MSG(getReferenceObjects() != nullptr, ErrorCodes::InvalidState, "VALIDATE_REF_REG: reference_objects is null! (Did you call OnRuntimeInitialized()?)") \
	ASSERT_MSG(getFreeReferences() != nullptr, ErrorCodes::InvalidState, "VALIDATE_REF_REG: free_references is null! (Did you call OnRuntimeInitialized()?)") \
	ASSERT_MSG(class_functions != nullptr, ErrorCodes::InvalidState, "VALIDATE_REF_EXISTS: class_functions is null! (Did you call OnRuntimeInitialized()?)") \
	ASSERT_MSG(id < getReferenceObjects()->size(), ErrorCodes::IndexIsOutOfBounds, "VALIDATE_REF_REG: reference index %d is out of bounds (size: %d)!", id, getReferenceObjects()->size()) \
	ASSERT_MSG((*getReferenceObjects())[id].reference == nullptr && (*getReferenceObjects())[id].functions == nullptr, ErrorCodes::InvalidParameter, "VALIDATE_REF_REG: reference index %d is not free! You MUST destroy the object before removing reference.", id) \
	for (int i = 0; i < getFreeReferences()->size(); i++) \
	{ \
		ASSERT_MSG((*getFreeReferences())[i] != id, ErrorCodes::InvalidState, "VALIDATE_REF_REG: reference index %d is currently a freed id!", id) \
	}
#else
#define VALIDATE_REF_REG(id)
#endif

#define LoadClassConstructor(className) \
	class_functions[className##_serializedIndex].constructor = &className::create

#define LoadClassBreathSampleCallback(className) \
	static BreathSampleCallback className##_breathSampleCallback = [](Serializable *self) { \
		/*EDebug::Log("BreathSampleCallback called for %s", #className);*/ \
		return reinterpret_cast<className*>(self)->Sample(); \
	}; \
	class_functions[className##_serializedIndex].breathSampleCallback = className##_breathSampleCallback;

#define LoadClassCorrelationCallback(className) \
	static CorrelationCallback className##_correlationCallback = [](Serializable *self) { \
		/*EDebug::Log("CorrelationCallback called for %s", #className);*/ \
		return reinterpret_cast<className*>(self)->Correlation(); \
	}; \
	class_functions[className##_serializedIndex].correlationCallback = className##_correlationCallback;

#define LoadClassUpdateCallback(className) \
	static UpdateCallback className##_updateCallback = [](Serializable *self, float delta) { \
		/*EDebug::Log("UpdateCallback called for %s", #className);*/ \
		reinterpret_cast<className*>(self)->managedUpdate(delta); \
	}; \
	static UpdateModeCallback className##_updateModeCallback = [](Serializable *self) { \
		/*EDebug::Log("UpdateModeCallback called for %s", #className);*/ \
		return reinterpret_cast<className*>(self)->getUpdateMode(); \
	}; \
	class_functions[className##_serializedIndex].updateCallback = className##_updateCallback; \
	class_functions[className##_serializedIndex].updateModeCallback = className##_updateModeCallback;

#define LoadClassStreamDataCallback(className) \
	static StreamDataCallback className##_streamDataCallback = [](Serializable *self) { \
		/*EDebug::Log("StreamDataCallback called for %s", #className);*/ \
		return reinterpret_cast<className*>(self)->getStreamData(); \
	}; \
	class_functions[className##_serializedIndex].streamDataCallback = className##_streamDataCallback;

std::vector<RefObject> *reference_objects = nullptr;
std::vector<UInt32> *free_references = nullptr;
ClassFunctions *class_functions = nullptr;

std::vector<RefObject> *ReferenceManager::getReferenceObjects()
{
	return reference_objects;
}

std::vector<UInt32> *ReferenceManager::getFreeReferences()
{
	return free_references;
}

void ReferenceManager::OnRuntimeInitialized()
{
	ASSERT_MSG(reference_objects == nullptr,, "ReferenceManager::OnRuntimeInitialized: reference_objects is not null! (Did you call this twice?)")
	ASSERT_MSG(free_references == nullptr,, "ReferenceManager::OnRuntimeInitialized: free_references is not null! (Did you call this twice?)")

	reference_objects = new std::vector<RefObject>();
	free_references = new std::vector<UInt32>();

	class_functions = new ClassFunctions[NUMBER_OF_SERIALIZABLE_CLASSES];

	LoadClassConstructor(BreathStream);
	LoadClassUpdateCallback(BreathStream);
	LoadClassBreathSampleCallback(BreathStream);

	LoadClassConstructor(TestingDetector);
	LoadClassUpdateCallback(TestingDetector);
	LoadClassBreathSampleCallback(TestingDetector);

	LoadClassConstructor(PatternSampler);
	LoadClassUpdateCallback(PatternSampler);
	LoadClassBreathSampleCallback(PatternSampler);

	LoadClassConstructor(SampleCorrelator);
	LoadClassCorrelationCallback(SampleCorrelator);

	LoadClassConstructor(SyncedPatternCorrelator);
	LoadClassUpdateCallback(SyncedPatternCorrelator);
	LoadClassCorrelationCallback(SyncedPatternCorrelator);
	LoadClassBreathSampleCallback(SyncedPatternCorrelator);

	LoadClassConstructor(RangeCorrelator);
	LoadClassBreathSampleCallback(RangeCorrelator);
	LoadClassCorrelationCallback(RangeCorrelator);

	LoadClassConstructor(MergeSampler);
	LoadClassUpdateCallback(MergeSampler);
	LoadClassBreathSampleCallback(MergeSampler);

	LoadClassConstructor(Pattern);

	

	LoadClassConstructor(SimpleAudioSampler);
	LoadClassUpdateCallback(SimpleAudioSampler);
	LoadClassBreathSampleCallback(SimpleAudioSampler);

	LoadClassConstructor(TMAudioSampler);
	LoadClassUpdateCallback(TMAudioSampler);
	LoadClassBreathSampleCallback(TMAudioSampler);
	
	LoadClassConstructor(ManagedObject);
	LoadClassBreathSampleCallback(ManagedObject);
	LoadClassCorrelationCallback(ManagedObject);
	LoadClassStreamDataCallback(ManagedObject);
}

int ReferenceManager::bindReference(UInt32 id, UInt16 classId, Serializable * reference)
{
	VALIDATE_REF_REG(id)
	ASSERT_MSG(classId < NUMBER_OF_SERIALIZABLE_CLASSES, ErrorCodes::IndexIsOutOfBounds, "ReferenceManager::bindReference: classId %d is out of bounds (size: %d)!", classId, NUMBER_OF_SERIALIZABLE_CLASSES)

	(*getReferenceObjects())[id].set(reference, &class_functions[classId]);
	return ErrorCodes::OK;
}

int ReferenceManager::getReference(UInt32 id, RefObject*& out_reference)
{
	VALIDATE_REF_EXISTS(id)

	out_reference = &(*getReferenceObjects())[id];
	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API UInt16 EXTERNAL_CALLING_CONVENTION ReferenceManager::ManagedClassIndex() {
	return ManagedObject_serializedIndex;
}


extern "C" BREATHLIBRARYCORE_API UInt32 EXTERNAL_CALLING_CONVENTION ReferenceManager::ReserveReferenceIndex()
{
	ASSERT_MSG(getReferenceObjects() != nullptr, -1, "ReferenceManager::ReserveReferenceIndex: reference_objects is null! (Did you call OnRuntimeInitialized()?)")
	ASSERT_MSG(getFreeReferences() != nullptr, -1, "ReferenceManager::ReserveReferenceIndex: free_references is null! (Did you call OnRuntimeInitialized()?)")

	UInt32 id;

	std::vector<RefObject> *references = getReferenceObjects();
	std::vector<UInt32> *freed = getFreeReferences();

	if (freed->size() > 0)
	{
		id = freed->back();
		freed->pop_back();

		ASSERT_MSG(id < references->size(), -1, "ReferenceManager::ReserveReferenceIndex: free reference index %d is out of bounds!", id)
		ASSERT_MSG((*references)[id].reference == nullptr, -1, "ReferenceManager::ReserveReferenceIndex: free reference index %d is not free!", id)
		ASSERT_MSG((*references)[id].functions == nullptr, -1, "ReferenceManager::ReserveReferenceIndex: free reference index %d is not free!", id)
	}
	else
	{
		id = (UInt32)references->size();
		references->push_back(RefObject());
	}

	return id;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::ReleaseReferenceIndex(UInt32 id)
{
	VALIDATE_REF_REG(id)

	// (*getReferenceObjects())[id].clear(); // Should already be cleared in order to pass the validation
	getFreeReferences()->push_back(id);
	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::BindNewNativeObject(UInt32 referenceIndex, char* base64Data, UInt32 length, unsigned char * runtimeData)
{
	VALIDATE_REF_REG(referenceIndex)

	unsigned char *savedData;
	Base64::decode(base64Data, length, savedData);

	UInt16 classIndex = *(UInt16*)savedData;

	if (classIndex >= NUMBER_OF_SERIALIZABLE_CLASSES)
	{
		EDebug::Warning("ClassDefinitions::createObject: Class Index %d is out of range.", classIndex);
		return ErrorCodes::InvalidParameter;
	}

	if (class_functions == nullptr)
	{
		EDebug::Warning("ClassDefinitions::createObject: Class definitions not loaded when trying to create class index %d.", classIndex);
		return ErrorCodes::InvalidParameter;
	}

	if (class_functions[classIndex].constructor == nullptr)
	{
		EDebug::Warning("ClassDefinitions::createObject: No class constructor found for class index %d.", classIndex);
		return ErrorCodes::InvalidParameter;
	}

	Serializable *obj = class_functions[classIndex].constructor(savedData + 2, runtimeData);

	// EDebug::Log("ReferenceManager::BindNewNativeObject: Created object of type %d at index %d with ptr %p", classIndex, referenceIndex, obj);

	return ReferenceManager::bindReference(referenceIndex, classIndex, obj);
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::DestroyReferenceObject(UInt32 id)
{
	VALIDATE_REF_EXISTS(id)

	RefObject obj = (*getReferenceObjects())[id];
	delete obj.reference;
	(*getReferenceObjects())[id].clear();

	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::GetUpdateMode(UInt32 ref_id)
{
	VALIDATE_REF_EXISTS(ref_id)

	RefObject obj = (*getReferenceObjects())[ref_id];

	if (obj.functions->updateModeCallback == nullptr)
	{
		// EDebug::Error("ReferenceManager::GetUpdateFunction: Reference as index is not a Behaviour: %d", ref_id);
		return -1;
	}

	return obj.functions->updateModeCallback(obj.reference);
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::UpdateReference(UInt32 ref_id, float deltaTime)
{
	VALIDATE_REF_EXISTS(ref_id)

	RefObject obj = (*getReferenceObjects())[ref_id];

	if (obj.functions->updateCallback == nullptr)
	{
		EDebug::Error("ReferenceManager::GetUpdateFunction: Reference as index is not a Behaviour: %d", ref_id);
		return -1;
	}

	obj.functions->updateCallback(obj.reference, deltaTime);

	return ErrorCodes::OK;
}


extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::GetBreathSample(UInt32 ref_id, float* *out_buffer)
{
	VALIDATE_REF_EXISTS(ref_id)

	RefObject obj = (*getReferenceObjects())[ref_id];

	if (obj.functions->breathSampleCallback == nullptr)
	{
		EDebug::Error("ReferenceManager::GetBreathSample: Reference should always be a IBreathSampler (via base Serializable): %d", ref_id);
		return ErrorCodes::InvalidParameter;
	}

	BreathSample sample = obj.functions->breathSampleCallback(obj.reference);

	for (int i = 0; i < BreathSample::NUM_VALUES; i++)
		(*out_buffer)[i] = sample.getValue(i);

	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReferenceManager::GetCorrelation(UInt32 ref_id, float* out_value)
{
	VALIDATE_REF_EXISTS(ref_id)

	RefObject obj = (*getReferenceObjects())[ref_id];
	if (obj.functions->correlationCallback == nullptr)
	{
		EDebug::Error("ReferenceManager::GetCorrelation: Reference should always be a ICorrelator (via base Serializable): %d", ref_id);
		return ErrorCodes::InvalidParameter;
	}

	*out_value = obj.functions->correlationCallback(obj.reference);
	return ErrorCodes::OK;
}

void ReferenceManager::OnApplicationQuit()
{
	// EDebug::Log("ReferenceManager::OnApplicationQuit: Cleaning up references");
	ASSERT_MSG(getReferenceObjects() != nullptr,, "ReferenceManager::OnApplicationQuit: reference_objects is null! (Did you call OnRuntimeInitialized()?)")
	ASSERT_MSG(getFreeReferences() != nullptr,, "ReferenceManager::OnApplicationQuit: free_references is null! (Did you call OnRuntimeInitialized()?)")

	for (int i = 0; i < getReferenceObjects()->size(); i++)
	{
		RefObject obj = (*getReferenceObjects())[i];
		if (obj.reference != nullptr)
		{
			EDebug::Warning("ReferenceManager::OnApplicationQuit: Reference id %d is not null (%p)! Destroying...", i, obj.reference);
			DestroyReferenceObject(i);
			ReleaseReferenceIndex(i);
		}
		if (obj.functions != nullptr)
		{
			EDebug::Warning("ReferenceManager::OnApplicationQuit: Reference id %d has non-null functions! Clearing...", i);
			obj.functions = nullptr;
		}
		else
		{
			// Assert that the index is in the free list
			bool found = false;
			for (int j = 0; j < getFreeReferences()->size(); j++)
			{
				if ((*getFreeReferences())[j] == i)
				{
					found = true;
					break;
				}
			}

			if (!found)
				EDebug::Warning("ReferenceManager::OnApplicationQuit: Reference %d is null, but id was not released!", i);
		}
	}

	delete reference_objects;
	reference_objects = nullptr;
	delete free_references;
	free_references = nullptr;

	delete[] class_functions;
	class_functions = nullptr;
}

BreathSample SamplerRef::Sample()
{
	RefObject *reference = nullptr;
	ReferenceManager::getReference(id, reference);
	ASSERT(reference != nullptr, BreathSample());
	ASSERT(reference->functions->breathSampleCallback != nullptr, BreathSample());
	return reference->functions->breathSampleCallback(reference->reference);
}

float CorrelatorRef::Correlation()
{
	RefObject *reference = nullptr;
	ReferenceManager::getReference(id, reference);
	ASSERT(reference != nullptr, -1.0f);
	ASSERT(reference->functions->correlationCallback != nullptr, -1.0f);
	return reference->functions->correlationCallback(reference->reference);
}

DataStreamMeta DataStreamRef::getStreamData()
{
	RefObject *reference = nullptr;
	ReferenceManager::getReference(id, reference);
	ASSERT(reference != nullptr, DataStreamMeta());
	ASSERT(reference->functions->streamDataCallback != nullptr, DataStreamMeta());
	return reference->functions->streamDataCallback(reference->reference);
}

Pattern *PatternRef::get()
{
	RefObject *reference = nullptr;
	ReferenceManager::getReference(id, reference);
	ASSERT(reference != nullptr, nullptr);
	return (Pattern*)(reference->reference);
}

BreathStream *StreamRef::get()
{
	RefObject *reference = nullptr;
	ReferenceManager::getReference(id, reference);
	ASSERT(reference != nullptr, nullptr);
	return (BreathStream*)(reference->reference);
}