#pragma once

#include "core_types.h"

#if PLATFORM_WINDOWS
	#include "windows/windows_platform_time.h"
#elif PLATFORM_APPLE
	#include "apple/apple_platform_time.h"
#elif PLATFORM_LINUX
	#include "linux/linux_platform_time.h"
#else
	#error "Unknown platform"
#endif
