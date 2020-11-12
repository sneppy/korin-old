#pragma once

#include "unix/unix_platform_atomics.h"

/**
 * Linux specific atomic operations
 */
struct LinuxPlatformAtomics final : public UnixPlatformAtomics
{
	//
};

using PlatformAtomics = LinuxPlatformAtomics;
