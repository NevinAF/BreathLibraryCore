#ifndef DEBUG_CALLBACKS_H
#define DEBUG_CALLBACKS_H

#include "..\definitions\PlatformDefinitions.hpp"

#if NDEBUG
#define ASSERT(condition, fallback) \
	if (!(condition)) \
	{ \
		EDebug::AssertionFailed(#condition, __FILE__, __LINE__); \
		return fallback; \
	}
#define ASSERT_MSG(condition, fallback, ...) \
	if (!(condition)) \
	{ \
		EDebug::AssertionFailedMessage(#condition, __FILE__, __LINE__, __VA_ARGS__); \
		return fallback; \
	}
#else
#define ASSERT(condition, fallback)
#define ASSERT_MSG(condition, fallback, ...)
#endif

namespace EDebug
{
	void Log(const char* format, ...);
	void Warning(const char* format, ...);
	void Error(const char* format, ...);
	void AssertionFailed(const char* condition, const char* file, int line);
	void AssertionFailedMessage(const char* condition, const char* file, int line, const char* message_format, ...);

	typedef void (EXTERNAL_DSP_CALLBACK* DebugCallback)(const char* message);
}

extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION SetDebugCallbacks(
	EDebug::DebugCallback log,
	EDebug::DebugCallback warning,
	EDebug::DebugCallback error);


#endif