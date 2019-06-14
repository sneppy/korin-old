#pragma once

#include "unix/unix_platform_memory.h"

/**
 * Linux specific memory utlities
 */
struct LinuxPlatformMemory : public UnixPlatformMemory
{

};

using PlatformMemory = LinuxPlatformMemory;