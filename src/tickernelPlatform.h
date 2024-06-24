#pragma once

// Platform macros:
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(_WIN64) || defined(__NT__)
#define PLATFORM_WINDOWS 1
#define PLATFORM_IPHONE 0
#define PLATFORM_OSX 0
#define PLATFORM_ANDROID 0
#define PLATFORM_LINUX 0
#define PLATFORM_UNIX 0
#define PLATFORM_POSIX 0
#elif __APPLE__
#include "TargetConditionals.h"
#if TARGET_OS_IPHONE
#define PLATFORM_IPHONE 1
#define PLATFORM_WINDOWS 0
#define PLATFORM_OSX 0
#define PLATFORM_ANDROID 0
#define PLATFORM_LINUX 0
#define PLATFORM_UNIX 0
#define PLATFORM_POSIX 1
#else
#define PLATFORM_IPHONE 0
#define PLATFORM_WINDOWS 0
#define PLATFORM_OSX 1
#define PLATFORM_ANDROID 0
#define PLATFORM_LINUX 0
#define PLATFORM_UNIX 0
#define PLATFORM_POSIX 1
#endif
#elif __ANDROID__
#define PLATFORM_WINDOWS 0
#define PLATFORM_IPHONE 0
#define PLATFORM_OSX 0
#define PLATFORM_ANDROID 1
#define PLATFORM_LINUX 1
#define PLATFORM_UNIX 0
#define PLATFORM_POSIX 1
#elif __linux__
#define PLATFORM_WINDOWS 0
#define PLATFORM_IPHONE 0
#define PLATFORM_OSX 0
#define PLATFORM_ANDROID 0
#define PLATFORM_LINUX 1
#define PLATFORM_UNIX 0
#define PLATFORM_POSIX 1
#elif __unix__ // all unices not caught above
#define PLATFORM_WINDOWS 0
#define PLATFORM_IPHONE 0
#define PLATFORM_OSX 0
#define PLATFORM_ANDROID 0
#define PLATFORM_LINUX 0
#define PLATFORM_UNIX 1
#define PLATFORM_POSIX 1
#else
#error "Unknown platform"
#endif
