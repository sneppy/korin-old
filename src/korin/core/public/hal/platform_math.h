#pragma once

#if PLATFORM_WINDOWS
	#include "windows/windows_platform_math.h"
#elif PLATFORM_APPLE
	#include "apple/apple_platform_math.h"
#elif PLATFORM_LINUX
	#include "linux/linux_platform_math.h"
#endif