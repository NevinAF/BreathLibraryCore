
#include "ManagedObject.hpp"
#include "..\debugging\DebugCallbacks.hpp"

using namespace ManagedCallbacks;

CreateSerializable(ManagedObject)

void ManagedObject::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	ASSERT(paramIndex == 0,)

	// The data represents three function pointers, which are stored as 64-bit integers. The first is the sample callback, the second is the correlation callback, and the third is the stream data callback.

	// The first callback is the sample callback. It is a function that returns a BreathSample.
	sampleCallback = (SampleCallback)*(UInt64*)runtimeData;
	runtimeData += sizeof(UInt64);

	correlationCallback = (CorrelationCallback)*(UInt64*)runtimeData;
	runtimeData += sizeof(UInt64);

	streamDataCallback = (StreamDataCallback)*(UInt64*)runtimeData;
	runtimeData += sizeof(UInt64);

	paramIndex--;
}

// void ManagedObject::addParameterDefinition(ClassDefinition *definition)
// {
// 	EDebug::Error("ManagedObject::setParameterIndex(): Managed Objects cannot be serialized in the usual way and should never have a class definition.");
// }


BreathSample ManagedObject::Sample()
{
	ASSERT_MSG(sampleCallback != nullptr, BreathSample(), "ManagedObject::Sample(): The sample callback is null! Chances are that a managed object is being referenced as a breath sampler when it is not.");
	return sampleCallback();
}

float ManagedObject::Correlation()
{
	ASSERT_MSG(correlationCallback != nullptr, 0.0f, "ManagedObject::Correlation(): The correlation callback is null! Chances are that a managed object is being referenced as a correlator when it is not.");
	return correlationCallback();
}

DataStreamMeta ManagedObject::getStreamData()
{
	ASSERT_MSG(streamDataCallback != nullptr, DataStreamMeta(), "ManagedObject::getStreamData(): The stream data callback is null! Chances are that a managed object is being referenced as a data stream when it is not.");
	return streamDataCallback();
}