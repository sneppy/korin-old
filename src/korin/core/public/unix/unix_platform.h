#pragma once

#include "generic/generic_platform.h"

/**
 * Unix specific types
 */
struct UnixPlatformTypes : public GenericPlatformTypes
{
	//
};

// Platform definitions

#define PLATFORM_UNIX 1

#if defined(_LINUX64) || defined(_LP64)
	#define PLATFORM_64 1
#else
	#define PLATFORM_64 0
#endif

#define PLATFORM_LITTLE_ENDIAN 1
#define PLATFORM_USE_PTHREADS 1

#if defined(_M_IX86) || defined(__i386) || defined(_M_X64) || defined(__x86_64) || defined(__amd64)
	#define PLATFORM_USE_SIMD 1
#else
	#define PLATFORM_USE_SIMD 0
#endif

// Compiler macros

#if BUILD_DEBUG
	#define FORCE_INLINE inline
#else
	#define FORCE_INLINE __attribute__((always_inline)) inline
#endif

#define LIKELY(expr)	__builtin_expect(static_cast<bool>(expr), 1)
#define UNLIKELY(expr)	__builtin_expect(static_cast<bool>(expr), 0)

#define RESTRICT __restrict__

#define ALIGN(n) __attribute__((aligned(n)))
#define PACK(n) __attribute__((packed, aligned(n)))