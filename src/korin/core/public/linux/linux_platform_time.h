#pragma once

#include "core_types.h"
#include "unix/unix_platform_time.h"

/**
 * @brief Linux-specific time utilities.
 */
struct LinuxPlatformTime : public UnixPlatformTime
{
	//
};

using PlatformTime = LinuxPlatformTime;
