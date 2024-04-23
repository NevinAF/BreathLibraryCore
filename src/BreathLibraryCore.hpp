#ifndef BREATHLIBRARYCORE_HPP
#define BREATHLIBRARYCORE_HPP

/* Include all other files such that this one file can act as the main file. */
#include "definitions\PlatformDefinitions.hpp"
#include "debugging\DebugCallbacks.hpp"
#include "correlators\RangeCorrelator.hpp"
#include "correlators\SampleCorrelator.hpp"
#include "correlators\SyncedPatternCorrelator.hpp"
#include "debugging\DebugCallbacks.hpp"
#include "definitions\ErrorCodes.hpp"
#include "definitions\IntTypes.hpp"
#include "definitions\PlatformDefinitions.hpp"
#include "detectors\MergeSampler.hpp"
#include "detectors\SimpleAudioSampler.hpp"
#include "detectors\TMAudioSampler.hpp"
#include "detectors\PatternSampler.hpp"
#include "detectors\ManagedObject.hpp"
#include "detectors\TestingDetector.hpp"
#include "editor\EnumDefinitions.hpp"
#include "editor\ClassDefinitions.hpp"
#include "serialization\ReferenceManager.hpp"
#include "serialization\Serializable.hpp"
#include "serialization\SerializedTypes.hpp"
#include "types\BreathSample.hpp"
#include "types\BreathStream.hpp"
#include "types\Keyframe.hpp"
#include "types\Pattern.hpp"
#include "types\Behaviour.hpp"
#include "types\SimpleInterfaces.hpp"
#include "util\Base64.hpp"
#include "util\Timer.hpp"
#include "util\interpolators.hpp"
#include "rapidjson\document.h"

extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION DebugPrintLoadedPlatform();
extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION OnRuntimeInitialized();
extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION OnApplicationQuit();
extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION OnApplicationSuspension();

#endif // BREATHLIBRARYCORE_HPP