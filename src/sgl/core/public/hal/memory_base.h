#pragma once

#include "core_types.h"

/**
 * Default alignments
 */
enum
{
	DEFAULT_ALIGNMENT	= 0x8,
	MIN_ALIGNMENT		= 0x8
};

/**
 * Base class for all allocator types
 */
class MallocBase
{
public:
	/**
	 * Memory allocation
	 * 
	 * @param [in] size size to allocate (in Bytes)
	 * @param [in] alignment memory alignment
	 * @return address of allocated memory or null
	 */
	virtual void * alloc(sizet size, sizet alignment = DEFAULT_ALIGNMENT) = 0;

	/**
	 * Memory reallocation
	 * 
	 * Behaviour is well defined only if orig
	 * was allocated by the same allocator
	 * 
	 * @param [in] orig memory previously allocated
	 * @param [in] size size to allocate (in Bytes)
	 * @param [in] alignment memory alignment
	 * @return address of allocated memory or null
	 */
	virtual void * realloc(void * orig, sizet size, sizet alignment = DEFAULT_ALIGNMENT) = 0;

	/**
	 * Memory deallocation
	 * 
	 * Behaviour is well defined only if orig
	 * was allocated by the same allocator
	 * 
	 * @param [in] orig memory previously allocated
	 */
	virtual void free(void * orig) = 0;
};