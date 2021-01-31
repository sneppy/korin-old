#pragma once

#include "core_types.h"

/**
 * @brief Platform-independent time utilities. 
 */
struct GenericPlatformTime
{
	/**
	 * @brief Returns the current time as
	 * UNIX timestamp (i.e. number of
	 * seconds since the Epoch, 00:00:00
	 * UTC, January 1st, 1970). 
	 */
	static FORCE_INLINE uint64 getCurrentTimestamp()
	{
		return static_cast<uint64>(time(nullptr));
	}
};
