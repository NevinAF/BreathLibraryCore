#ifndef REFERENCEMANAGER_H
#define REFERENCEMANAGER_H

#include "..\types\BreathSample.hpp"
#include "..\types\DataStreamMeta.hpp"
#include "..\definitions\IntTypes.hpp"
#include "..\definitions\PlatformDefinitions.hpp"
#include "..\serialization\Serializable.hpp"
#include <vector>

namespace ReferenceManager
{
	typedef Serializable* (*SerializableConstructor)(unsigned char *savedData, unsigned char *runtimeData);
	typedef void (EXTERNAL_DSP_CALLBACK* UpdateCallback)(Serializable* obj, float delta_time);
	typedef int (EXTERNAL_DSP_CALLBACK* UpdateModeCallback)(Serializable* obj);
	typedef BreathSample(EXTERNAL_DSP_CALLBACK* BreathSampleCallback)(Serializable* obj);
	typedef float (EXTERNAL_DSP_CALLBACK* CorrelationCallback)(Serializable* obj);
	typedef DataStreamMeta (EXTERNAL_DSP_CALLBACK* StreamDataCallback)(Serializable* obj);
}

struct ClassFunctions
{
public:
	ReferenceManager::SerializableConstructor constructor{nullptr};
	ReferenceManager::UpdateCallback updateCallback{nullptr};
	ReferenceManager::UpdateModeCallback updateModeCallback{nullptr};
	ReferenceManager::BreathSampleCallback breathSampleCallback{nullptr};
	ReferenceManager::CorrelationCallback correlationCallback{nullptr};
	ReferenceManager::StreamDataCallback streamDataCallback{nullptr};

	ClassFunctions() = default;
};

struct RefObject
{
public:
	Serializable *reference{nullptr};
	ClassFunctions *functions{nullptr};

	RefObject() = default;

	inline void set(Serializable *ref, ClassFunctions *func)
	{
		reference = ref;
		functions = func;
	}

	inline void clear()
	{
		reference = nullptr;
		functions = nullptr;
	}
};

namespace ReferenceManager
{
	int releaseReference(UInt32 id);
	int getReference(UInt32 id, RefObject*& out_reference);

	std::vector<RefObject> *getReferenceObjects();
	std::vector<UInt32> *getFreeReferences();

	extern "C" BREATHLIBRARYCORE_API UInt16 EXTERNAL_CALLING_CONVENTION ManagedClassIndex();

	extern "C" BREATHLIBRARYCORE_API UInt32 EXTERNAL_CALLING_CONVENTION ReserveReferenceIndex();
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION ReleaseReferenceIndex(UInt32 id);

	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION BindNewNativeObject(UInt32 referenceIndex, char *base64Data, UInt32 length, unsigned char *runtimeData);
	int bindReference(UInt32 id, UInt16 classId, Serializable *reference);

	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION DestroyReferenceObject(UInt32 id);

	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetUpdateMode(UInt32 ref_id);
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION UpdateReference(UInt32 ref_id, float deltaTime);
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetBreathSample(UInt32 ref_id, float* *out_buffer);
	extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION GetCorrelation(UInt32 ref_id, float* out_value);

	void OnRuntimeInitialized();
	void OnApplicationQuit();
};

class Ref
{
public:
	UInt32 id{0xFFFFFFFF};
	virtual ~Ref() = default;
};

class SamplerRef : public Ref, public IBreathSampler
{
public:
	BreathSample Sample() override;
};

class CorrelatorRef : public Ref, ICorrelator
{
public:
	float Correlation() override;
};

class DataStreamRef : public Ref, IDataStream
{
public:
	DataStreamMeta getStreamData() override;
};

class Pattern;
class BreathStream;

class PatternRef : public Ref
{
public:
	Pattern *get();
};

class StreamRef : public Ref
{
public:
	BreathStream *get();
};

#endif // REFERENCEMANAGER_H