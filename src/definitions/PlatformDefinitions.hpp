#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#   define PLATFORM_WIN 1
#   if defined(WINAPI_FAMILY) && (WINAPI_FAMILY == WINAPI_FAMILY_APP)
#       define PLATFORM_WINRT 1
#   endif
#elif defined(__MACH__) || defined(__APPLE__)
#   define PLATFORM_OSX 1
#elif defined(__ANDROID__)
#   define PLATFORM_ANDROID 1
#elif defined(__linux__)
#   define PLATFORM_LINUX 1
#endif

#if defined(_AMD64_) || defined(__LP64__) || defined(_M_ARM64)
#   define PLATFORM_ARCH_64 1
#   define PLATFORM_ARCH_32 0
#else
#   define PLATFORM_ARCH_64 0
#   define PLATFORM_ARCH_32 1
#endif

#if defined(__CYGWIN32__)
#define EXTERNAL_DSP_CALLBACK __stdcall
#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64)
#define EXTERNAL_DSP_CALLBACK __stdcall
#elif defined(__MACH__) || defined(__ANDROID__) || defined(__linux__)
#define EXTERNAL_DSP_CALLBACK
#else
#define EXTERNAL_DSP_CALLBACK
#endif

#if PLATFORM_WINRT
#define EXTERNAL_CALLING_CONVENTION __stdcall
#else
#define EXTERNAL_CALLING_CONVENTION
#endif

// The following ifdef block is the standard way of creating macros which make exporting
// from a DLL simpler. All files within this DLL are compiled with the BREATHLIBRARYCORE_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see
// BREATHLIBRARYCORE_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef BREATHLIBRARYCORE_EXPORTS
#define BREATHLIBRARYCORE_API __declspec(dllexport)
#else
#define BREATHLIBRARYCORE_API __declspec(dllimport)
#endif