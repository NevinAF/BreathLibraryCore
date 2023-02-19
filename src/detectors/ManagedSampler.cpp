#include "pch.h"

#include "ManagedSampler.hpp"
#include "..\serialization\ReferenceManager.hpp"

BreathSample ManagedSampler::Sampler::getSample()
{
	return callback();
}

extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION ManagedSampler::RegisterManagedSampler(SampleCallback callback)
{
	ManagedSampler::Sampler *sampler = new ManagedSampler::Sampler(callback);
	// TODO: Add reference to sampler
	// ReferenceManager::addReference(sampler);
}