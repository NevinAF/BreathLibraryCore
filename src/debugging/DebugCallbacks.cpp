#include "DebugCallbacks.hpp"
#include <stdio.h>
#include <stdarg.h>

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
		vsprintf(buffer, format, args);
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
		vsprintf(buffer, format, args);
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
		vsprintf(buffer, format, args);
		va_end(args);
		error(buffer);
	}
}

void EDebug::AssertionFailed(const char* condition, const char* file, int line)
{
	char buffer[256];
	sprintf(buffer, "Assertion failed: %s in %s at line %d", condition, file, line);
	Error(buffer);
}

void EDebug::AssertionFailedMessage(const char* condition, const char* file, int line, const char* message_format, ...)
{
	char buffer[1024];
	char message[1024];
	va_list args;
	va_start(args, message_format);
	vsprintf(message, message_format, args);
	va_end(args);
	sprintf(buffer, "Assertion failed: %s in %s at line %d: %s", condition, file, line, message);
	Error(buffer);
}

extern "C" BREATHLIBRARYCORE_API void EXTERNAL_CALLING_CONVENTION SetDebugCallbacks(
	EDebug::DebugCallback log,
	EDebug::DebugCallback warning,
	EDebug::DebugCallback error)
{
	EDebug::log = log;
	EDebug::warning = warning;
	EDebug::error = error;
}