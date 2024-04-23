
#include "EnumDefinitions.hpp"
#include "..\definitions\ErrorCodes.hpp"
#include "..\debugging\DebugCallbacks.hpp"
#include "..\serialization\Serializable.hpp"


EnumDefinitions::EnumDefinition **EnumDefinitions::definitions;

EnumDefinitions::EnumDefinition::EnumDefinition(const char *name, const char *tooltip, EnumValue *values, UInt16 numValues)
{
	strncpy(this->name, name, 32);
	strncpy(this->tooltip, tooltip, 256);
	this->values = values;
	this->numValues = numValues;
}

EnumDefinitions::EnumValue::EnumValue(const char *name, const char *tooltip, UInt8 value)
{
	strncpy(this->name, name, 32);
	strncpy(this->tooltip, tooltip, 256);
	this->value = value;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION EnumDefinitions::LoadEnumDefinitions()
{
	if (definitions != nullptr)
	{
		EDebug::Log("EnumDefinitions::LoadEnumDefinitions() - Enum definitions already loaded.");
		return ErrorCodes::OK;
	}

	definitions = new EnumDefinition*[NUM_ENUM_DEFS];

	definitions[Interpolator_Method_serializedIndex] = new EnumDefinition(
		"Interpolator Method",
		"The method used to interpolate between values",
		new EnumValue[7] {
			EnumValue("Hold", "Special version of constant used for keyframes. Hold defines a keyframe that does not have a fixed length", 0xFF),
			EnumValue("Constant", "The value is constant between samples", 0),
			EnumValue("Linear", "The value is linearly interpolated between samples", 1),
			EnumValue("Cosine", "The value is interpolated using a cosine function", 2),
			EnumValue("Square", "The value is interpolated using a square function", 3),
			EnumValue("Cubic", "The value is interpolated using a cubic function", 4),
			EnumValue("SquareRoot", "The value is interpolated using a square root function", 5)
		}, 7);



	definitions[Behaviour_UpdateMode_serializedIndex] = new EnumDefinition(
		"Behaviour UpdateMode",
		"The update mode of a behaviour",
		new EnumValue[6] {
			EnumValue("Immediate OnDemand", "This will only update when values are needed, and the update will happen a single time, passing in the time that has passed since the last update.", 0),
			EnumValue("StepWise OnDemand", "This will only update when values are needed, but the update will be called for each fixed timestamp that has passed.", 1),
			EnumValue("Fixed Managed", "This will update at a fixed interval, using the managing platform (like Unity3d or Unreal Engine).", 2),
			EnumValue("Dynamic Managed", "This will update at a performance based interval using the managing platform (like Unity3d or Unreal Engine). Generally, a 'performance based interval' is an update every frame", 3),
			EnumValue("Async Unmanaged", "This will update asynchronously using a threading or task system (not recommended in most cases)(experimental, may cause rase conditions).", 4),
			EnumValue("Sync Unmanaged", "This will update at a fixed interval controlled by the BreathLibrary using a runnable thread (experimental, may cause rase conditions).", 5)
		}, 6);



	definitions[SyncedPatternCorrelator_MatchMode_serializedIndex] = new EnumDefinition(
		"SyncPtrnCor ProgressMode",
		"The mode at which the pattern time progresses for a of a SyncedPatternCorrelator",
		new EnumValue[2] {
			EnumValue("Constant", "Pattern progresses at a constant rate of time, only slowing to match with holds when applicable.", 0),
			EnumValue("Wait", "Pattern progresses only when the sampler matches the pattern.", 1)
		}, 2);

	return ErrorCodes::OK;
}

extern "C" BREATHLIBRARYCORE_API int EXTERNAL_CALLING_CONVENTION EnumDefinitions::GetEnumDefinition(EnumDefinition** pDef, int index)
{
	if (index < 0 || index >= NUM_ENUM_DEFS)
	{
		EDebug::Error("EnumDefinitions::GetEnumDefinition() - Index %d out of range", index);
		return -1;
	}

	if (definitions == nullptr)
	{
		EDebug::Warning("EnumDefinitions::GetEnumDefinition() - Enum definitions not loaded. Loading now...");
		int result = LoadEnumDefinitions();

		if (result != ErrorCodes::OK)
		{
			EDebug::Error("EnumDefinitions::GetEnumDefinition() - Failed to load enum definitions");
			return -1;
		}
	}

	*pDef = definitions[index];
	return ErrorCodes::OK;
}