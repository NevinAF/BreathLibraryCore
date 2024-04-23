#include "TestingDetector.hpp"
#include "..\debugging\DebugCallbacks.hpp"

CreateSerializable(TestingDetector);

void TestingDetector::update(float delta)
{
	currentTime += delta;
	if (currentTime > 1.0f) currentTime -= 1.0f;
}

BreathSample TestingDetector::Sample()
{
	demandUpdate();
	return BreathSample(currentTime, 0.5f, currentTime - 1, 0.2f, (currentTime / 2));
}

void TestingDetector::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);

	definition->addFloatDefinition("Start Time|Current Time", "Normalized time of the testing detector", 0.0f, 0.0f, 1.0f);
}

void TestingDetector::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	Behaviour::setParameterIndex(paramIndex, savedData, runtimeData);

	serializable_SetFloat(currentTime);
}