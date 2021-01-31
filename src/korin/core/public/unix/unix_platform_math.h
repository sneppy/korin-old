#pragma once

#include "generic/generic_platform_math.h"

/**
 * Unix specific math functions
 */
struct UnixPlatformMath : public GenericPlatformMath
{
	/**
	 * @see GenericPlatformMath::parity
	 */
	static constexpr FORCE_INLINE uint64 parity(const uint64 & n)
	{		
		return __builtin_parity(n);
	}
};