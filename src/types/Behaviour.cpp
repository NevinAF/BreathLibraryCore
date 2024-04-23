
#include "Behaviour.hpp"
#include "..\debugging\DebugCallbacks.hpp"

void Behaviour::addParameterDefinition(ClassDefinition *definition)
{
	definition->addEnumDefinition(
		"Sampling Mode",
		"The type of sample that this sampler will produce.",
		(UInt8)UpdateMode::Default,
		Behaviour_UpdateMode_serializedIndex,
		ParameterFlags::PAR_ReadOnlyWhilePlaying);
}

void Behaviour::setParameterIndex(UInt16 &paramIndex, unsigned char *&savedData, unsigned char *&runtimeData)
{
	serializable_SetEnum(updateMode);
}

void Behaviour::initialize()
{
	if (updateMode == UpdateMode::Sync_Unmanaged)
	{
		EDebug::Warning("Behaviour::initialize: Sync_Unmanaged mode is not yet implemented.");
		//Timer::registerUpdateFunction(
	}
}

Behaviour::~Behaviour()
{
}

void Behaviour::update(float delta) {
}

void Behaviour::demandUpdate()
{
	if (updateMode == UpdateMode::Immediate_OnDemand)
	{
		double currentTime = Timer::getTime();
		if (currentTime - lastUpdateTime > Timer::DEFAULT_UPDATE_PERIOD)
		{
			update((float)(currentTime - lastUpdateTime));
			lastUpdateTime = currentTime;
		}
	}
	else if (updateMode == UpdateMode::StepWise_OnDemand)
	{
		double currentTime = Timer::getTime();
		while (currentTime - lastUpdateTime > Timer::DEFAULT_UPDATE_PERIOD)
		{
			update(Timer::DEFAULT_UPDATE_PERIOD);
			lastUpdateTime += Timer::DEFAULT_UPDATE_PERIOD;
		}
	}
}