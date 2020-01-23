#pragma once

#include "core_types.h"

#if PLATFORM_WINDOWS
	#include "windows/windows_platform_memory.h"
#elif PLATFORM_APPLE
	#include "apple/apple_platform_memory.h"
#elif PLATFORM_LINUX
	#include "linux/linux_platform_memory.h"
#endif

#include "memory_base.h"

/**
 * Generic wrapper for memory utilities
 */
struct Memory : public PlatformMemory
{

};