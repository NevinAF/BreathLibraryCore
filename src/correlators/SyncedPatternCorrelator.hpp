#ifndef SYNCEDPATTERNCORRELATOR_H
#define SYNCEDPATTERNCORRELATOR_H

#include "..\types\BreathSample.hpp"
#include "..\types\SimpleInterfaces.hpp"
#include "..\types\Pattern.hpp"
#include "..\types\Behaviour.hpp"
#include "..\serialization\Serializable.hpp"
#include "..\serialization\ReferenceManager.hpp"


class SyncedPatternCorrelator : public Behaviour, public ICorrelator, public IBreathSampler
{
private:
	enum class ProgressMode
	{
		/** Pattern progresses at a constant rate of time, only slowing to match with holds when applicable. */
		Constant = 0,
		/** Pattern progresses only when the sampler matches the pattern. */
		Wait = 1,

		__LENGTH__
	};

	SamplerRef sampler{};
	PatternRef pattern_ref{};

	Pattern* pattern{nullptr};

	ProgressMode progressMode{ProgressMode::Constant};

	float patternTime{0};
	float absMatchRange_before{0};
	float absMatchRange_after{0};

	float holdThreshold{0.65f};
	float nextThreshold{0.65f};

	BreathSample comparingSample;
	float comparingCorrelation{0};

	void setComparingSample(const BreathSample& samplerSample, float delta);
	void setPatternTimeByMode(float delta);

	float _holdTime{-1};
	bool isHolding() const;
	bool _setComparingSample_HoldCheck(float delta);

protected:
	void update(float delta) override;
	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

public:
	SerializableProperties(
		SyncedPatternCorrelator,
		"Synced Pattern Correlator",
		"Uses a breath pattern to correlate with a breath sampler. The progression of the pattern will depend on the sampler's output.",
		ReferenceType::REF_Sampler | ReferenceType::REF_Correlator | ReferenceType::REF_Behaviour)

	static void addParameterDefinition(ClassDefinition *definition);

	float Correlation() override;
	BreathSample Sample() override;
};

#endif // SYNCEDPATTERNCORRELATOR_H