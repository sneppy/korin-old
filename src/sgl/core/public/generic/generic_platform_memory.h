#pragma once

#include "core_types.h"
#include "hal/platform_crt.h"

/**
 * Platform independent memory definitions
 */
struct GenericPlatformMemory
{
	//////////////////////////////////////////////////
	// Standard C memory utils
	//////////////////////////////////////////////////

	/**
	 * Copy memory from source to destination
	 * 
	 * @param [in] dst destination memory
	 * @param [in] src source memory
	 * @param [in] size memory size (in Bytes)
	 * @return destination address
	 */
	static FORCE_INLINE void * memcpy(void * RESTRICT dst, const void * RESTRICT src, sizet size)
	{
		return ::memcpy(dst, src, size);
	}
	
	/**
	 * Copy memory using an intermediate buffer
	 * allowing destination and source to overlap
	 * 
	 * @param [in] dst destination memory
	 * @param [in] src source memory
	 * @param [in] size memory size (in Bytes)
	 * @return destination address
	 */
	static FORCE_INLINE void * memmov(void * dst, const void * src, sizet size)
	{
		return ::memmove(dst, src, size);
	}
};