#ifndef MANAGEDOBJECT_H
#define MANAGEDOBJECT_H

#include "..\types\SimpleInterfaces.hpp"
#include "..\serialization\Serializable.hpp"
#include "..\definitions\PlatformDefinitions.hpp"
#include "..\types\DataStreamMeta.hpp"

namespace ManagedCallbacks
{
	typedef BreathSample (EXTERNAL_DSP_CALLBACK* SampleCallback)();
	typedef float (EXTERNAL_DSP_CALLBACK* CorrelationCallback)();
	typedef DataStreamMeta (EXTERNAL_DSP_CALLBACK* StreamDataCallback)();
}

class ManagedObject : public Serializable, public IBreathSampler, public ICorrelator, public IDataStream
{
private:
	ManagedCallbacks::SampleCallback sampleCallback;
	ManagedCallbacks::CorrelationCallback correlationCallback;
	ManagedCallbacks::StreamDataCallback streamDataCallback;

protected:
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

public:
	SerializableProperties(
		ManagedObject,
		"Managed Object",
		"Object that is managed by a managed application (i.e., not this)",
		ReferenceType::REF_Correlator | ReferenceType::REF_Sampler | ReferenceType::REF_DataStream)

	// static void addParameterDefinition(ClassDefinition *definition);

	BreathSample Sample() override;
	float Correlation() override;
	DataStreamMeta getStreamData() override;
};

#endif // MANAGEDOBJECT_H