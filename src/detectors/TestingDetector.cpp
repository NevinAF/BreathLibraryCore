#include "pch.h"
#include "TestingDetector.hpp"

using namespace SerializedTypes;

CreateSerializable(TestingDetector);

void TestingDetector::update(float delta)
{
	float _time = currentTime;

	currentTime += delta;
	if (currentTime > 1.0f) currentTime -= 1.0f;
}

BreathSample TestingDetector::getSample()
{
	demandUpdate();
	return BreathSample(currentTime, 0.5f, currentTime - 1, 0.0f, (currentTime / 2) -1);
}

void TestingDetector::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);

	definition->addFloatDefinition("Start Time|Current Time", "Normalized time of the testing detector", 0.0f, 0.0f, 1.0f);
}

void TestingDetector::setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references)
{
	Behaviour::setParameterIndex(paramIndex, data, references);

	SetFloat(currentTime);
}