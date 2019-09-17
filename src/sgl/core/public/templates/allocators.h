#pragma once

#include "core_types.h"
#include "hal/platform_memory.h"
#include "misc/assert.h"

/**
 * Helper class to easily allocate space for one
 * or more objects
 */
template<typename T>
class ObjectAllocator
{
protected:
	/// Underlying allocator
	MallocBase * allocator;

public:
	/**
	 * Default constructor
	 */
	FORCE_INLINE ObjectAllocator(MallocBase * inAllocator)
		: allocator{inAllocator}
	{
		//
	}

	/**
	 * Allocate contigous space for one or more objects
	 * 
	 * @param [in] n number of objects
	 * @return pointer to allocated space
	 */
	FORCE_INLINE T * alloc(uint32 n = 1)
	{
		CHECK(allocator != nullptr)

		return reinterpret_cast<T*>(allocator->alloc(n * sizeof(T), alignof(T)));
	}

	/**
	 * Free memory. Same as freeing using the real
	 * allocator
	 * 
	 * @param [in] orig original memory
	 */
	FORCE_INLINE void free(T * orig)
	{
		allocator->free(orig);
	}
};