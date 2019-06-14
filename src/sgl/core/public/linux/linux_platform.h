#pragma once

#include "unix/unix_platform.h"

/**
 * Linux specific types
 */
struct LinuxPlatformTypes : public UnixPlatformTypes
{
	//
};

using PlatformTypes = LinuxPlatformTypes;