#include "pch.h"
#include "MergeSampler.hpp"

CreateSerializable(MergeSampler);

void MergeSampler::update(float delta)
{
	// TODO: Implement
}

BreathSample MergeSampler::getSample()
{
	demandUpdate();
	return currentSample;
}

void MergeSampler::addParameterDefinition(ClassDefinition *definition)
{
	Behaviour::addParameterDefinition(definition);
	// TODO: Implement
}

void MergeSampler::setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references)
{
	Behaviour::setParameterIndex(paramIndex, data, references);
	// TODO: Implement
}