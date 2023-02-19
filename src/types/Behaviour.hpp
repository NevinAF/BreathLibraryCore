#pragma once

#include "..\serialization\Serializable.hpp"
#include "..\util\Timer.hpp"

/**
 * @brief A class that can sample breath data. Usually used for creating a BreathStream.
 */
class Behaviour : protected CanSerialize
{
private:
	enum class UpdateMode
	{
		Immediate_OnDemand = 0,
		StepWise_OnDemand = 1,

		Fixed_Managed = 2,
		Dynamic_Managed = 3,

		Async_Unmanaged = 4,
		Sync_Unmanaged = 5,

		__LENGTH__,

		Default = Fixed_Managed
	};

	double lastUpdateTime{0};

protected:
	/** The type of sample that this sampler will produce. */
	UpdateMode updateMode{UpdateMode::Default};

	void demandUpdate();

	static void addParameterDefinition(SerializedTypes::ClassDefinition *definition);

	void setParameterIndex(UInt16& paramIndex, unsigned char*& data, UInt32*& references) override;

	void initialize() override;

	/** @return The sampling period of the sampler. */
	virtual void update(float delta);

public:
};