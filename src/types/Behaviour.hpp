#ifndef BEHAVIOUR_H
#define BEHAVIOUR_H

#include "..\serialization\Serializable.hpp"
#include "..\util\Timer.hpp"
#include <functional>

/**
 * @brief A class that can sample breath data. Usually used for creating a BreathStream.
 */
class Behaviour : public Serializable
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

	static void addParameterDefinition(ClassDefinition *definition);

	void setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData) override;

	void initialize() override;
	virtual void update(float delta);

	virtual ~Behaviour();

public:
	// Inline as this will be compiled out in release builds
	inline void managedUpdate(float delta)
	{
		ASSERT_MSG(updateMode == Behaviour::UpdateMode::Fixed_Managed ||
			updateMode == Behaviour::UpdateMode::Dynamic_Managed, ,
			"Trying to update native object with a non managed update mode. RefId: %d", updateMode)
		update(delta);
	}
	int getUpdateMode() const { return (int)updateMode; }
};

#endif // BEHAVIOUR_H