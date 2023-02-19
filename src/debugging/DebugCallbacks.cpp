#include "pch.h"
#include "DebugCallbacks.hpp"
#include <stdio.h>

namespace EDebug
{
	DebugCallback log;
	DebugCallback warning;
	DebugCallback error;
}

void EDebug::Log(const char* format, ...)
{
	if (log != nullptr)
	{
		char buffer[1024];
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, format, args);
		va_end(args);
		log(buffer);
	}
}

void EDebug::Warning(const char* format, ...)
{
	if (warning != nullptr)
	{
		char buffer[1024];
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, format, args);
		va_end(args);
		warning(buffer);
	}
}

void EDebug::Error(const char* format, ...)
{
	if (error != nullptr)
	{
		char buffer[1024];
		va_list args;
		va_start(args, format);
		vsprintf_s(buffer, format, args);
		va_end(args);
		error(buffer);
	}
}

void EDebug::AssertionFailed(const char* condition, const char* file, int line)
{
	char buffer[256];
	sprintf_s(buffer, "Assertion failed: %s in %s at line %d", condition, file, line);
	Error(buffer);
}

extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION SetDebugCallbacks(
	EDebug::DebugCallback log,
	EDebug::DebugCallback warning,
	EDebug::DebugCallback error)
{
	log("Setting Debug Callbacks...");

	EDebug::log = log;
	EDebug::warning = warning;
	EDebug::error = error;
}