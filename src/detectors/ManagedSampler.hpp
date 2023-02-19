#pragma once

#include "..\types\SimpleInterfaces.hpp"
#include "..\definitions\PlatformDefinitions.hpp"

namespace ManagedSampler
{
	typedef BreathSample (EXTERNAL_DSP_CALLBACK* SampleCallback)();

	class Sampler : public IBreathSampler
	{
	private:
		SampleCallback callback;

	public:
		Sampler(SampleCallback callback) : callback(callback) {}
		BreathSample getSample();
	};

	extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION RegisterManagedSampler(SampleCallback callback);
}




