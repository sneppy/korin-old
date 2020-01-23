#pragma once

// Sets undefined platforms to zero
#ifndef PLATFORM_WINDOWS
	#define PLATFORM_WINDOWS 0
#endif

#ifndef PLATFORM_APPLE
	#define PLATFORM_APPLE 0
#endif

#ifndef PLATFORM_LINUX
	#define PLATFORM_LINUX 0
#endif

// Include specific platform definitions
#if PLATFORM_WINDOWS
	#include "windows/windows_platform.h"
#elif PLATFORM_APPLE
	#include "apple/apple_platform.h"
#elif PLATFORM_LINUX
	#include "linux/linux_platform.h"
#else
	#error "Unknown platform"
#endif

// Set to false undefined platform properties

#ifndef PLATFORM_UNIX
	#define PLATFORM_UNIX 0
#endif

#ifndef PLATFORM_64
	#define PLATFORM_64 0
#endif

#ifndef PLATFORM_32
	#define PLATFORM_32 !PLATFORM_64
#endif

#ifndef PLATFORM_LITTLE_ENDIAN
	#define PLATFORM_LITTLE_ENDIAN 0
#endif

#ifndef PLATFORM_USE_SIMD
	#define PLATFORM_USE_SIMD 0
#endif

#ifndef PLATFORM_USE_PTHREADS
	#define PLATFORM_USE_PTHREADS 0
#endif

// Set to default undefined compiler properties

#ifndef FORCE_INLINE
	#define FORCE_INLINE inline
#endif

#ifndef LIKELY
	#define LIKELY(expr) (expr)
#endif

#ifndef UNLIKELY
	#define UNLIKELY(expr) (expr)
#endif

#ifndef RESTRICT
	#define RESTRICT restrict
#endif

#ifndef ALIGN
	#define ALIGN(n) alignas(n)
#endif

#ifndef PACK
	#define PACK(n) ALIGN(n)
#endif

// Platform independent types

using int8	= PlatformTypes::int8;
using int16	= PlatformTypes::int16;
using int32	= PlatformTypes::int32;
using int64	= PlatformTypes::int64;

using uint8		= PlatformTypes::uint8;
using uint16	= PlatformTypes::uint16;
using uint32	= PlatformTypes::uint32;
using uint64	= PlatformTypes::uint64;

using byte	= PlatformTypes::byte;
using ubyte	= PlatformTypes::ubyte;

using intp	= PlatformTypes::intp;
using uintp	= PlatformTypes::uintp;
using sizet	= PlatformTypes::sizet;

using float32	= PlatformTypes::float32;
using float64	= PlatformTypes::float64;
using float128	= PlatformTypes::float128;

using ansichar	= PlatformTypes::ansichar;
using widechar	= PlatformTypes::widechar;
using char8		= PlatformTypes::char8;
using char16	= PlatformTypes::char16;
using char32	= PlatformTypes::char32;