// BreathLibraryCore.cpp : Defines the exported functions for the DLL.
#include "BreathLibraryCore.hpp"

extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION DebugPrintLoadedPlatform()
{
	EDebug::Log(
#if defined(PLATFORM_WIN) && (PLATFORM_ARCH_64 == 1)
		"Breath Library Core: Loaded for Windows x64"
#elif defined(PLATFORM_WIN) && (PLATFORM_ARCH_32 == 1)
		"Breath Library Core: Loaded for Windows x86"
#elif defined(PLATFORM_OSX) && (PLATFORM_ARCH_64 == 1)
		"Breath Library Core: Loaded for OSX x64"
#elif defined(PLATFORM_OSX) && (PLATFORM_ARCH_32 == 1)
		"Breath Library Core: Loaded for OSX x86"
#elif defined(PLATFORM_ANDROID) && (PLATFORM_ARCH_64 == 1)
		"Breath Library Core: Loaded for Android x64"
#elif defined(PLATFORM_ANDROID) && (PLATFORM_ARCH_32 == 1)
		"Breath Library Core: Loaded for Android x86"
#elif defined(PLATFORM_LINUX) && (PLATFORM_ARCH_64 == 1)
		"Breath Library Core: Loaded for Linux x64"
#elif defined(PLATFORM_LINUX) && (PLATFORM_ARCH_32 == 1)
		"Breath Library Core: Loaded for Linux x86"
#else
		"Breath Library Core: Loaded for unknown platform"
#endif
	);
}

extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION OnRuntimeInitialized()
{
	// EDebug::Log("Breath Library Core: OnRuntimeInitialized");

	ReferenceManager::OnRuntimeInitialized();
}

extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION OnApplicationQuit()
{
	// EDebug::Log("Breath Library Core: OnApplicationQuit");

	ReferenceManager::OnApplicationQuit();
}

extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION OnApplicationSuspension()
{
	// EDebug::Log("Breath Library Core: OnApplicationSuspension");
}