#pragma once

#include "unix/unix_platform_math.h"

/**
 * Linux specific math functions
 */
struct LinuxPlatformMath : public UnixPlatformMath
{

};

using PlatformMath = LinuxPlatformMath;