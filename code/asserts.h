#pragma once

#include "cleanwindows.h"
#include <intrin.h>

#include "types.h"

void ReportAssertionFailure(
    const char* condition, const char* message, const char* file, u32 line);

#if _DEBUG
#define DEBUG_BREAK() \
do { \
    if (IsDebuggerPresent()) \
    { \
        __debugbreak(); \
    } \
} while (0)

#define SOFTASSERT(expr, msg) \
do { \
    if (expr) \
    { \
    } \
    else \
    { \
        ReportAssertionFailure(#expr, msg, __FILE__, __LINE__); \
        DEBUG_BREAK(); \
    } \
} while(0) \

#else
#define DEBUG_BREAK()
#define SOFTASSERT(expr, msg)
#endif // _DEBUG

// NOTE(sbalse): Hard asserts are unrecoverable errors that we probably also want to keep in release builds.
#define HARDASSERT(expr, msg) \
do { \
    if (expr) \
    { \
    } \
    else \
    { \
        ReportAssertionFailure(#expr, msg, __FILE__, __LINE__); \
        *(volatile int*)0 = 0; \
    } \
} while (0)
