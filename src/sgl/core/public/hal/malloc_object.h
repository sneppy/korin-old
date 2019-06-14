#pragma once

#include "core_types.h"
#include "platform_memory.h"

/**
 * 
 */
template<typename T, typename MallocT>
class MallocObject
{
protected:
	/// Allocation unit size
	static constexpr sizet allocUnitSize = sizeof(T);

	/// Allocation alignment
	static constexpr sizet allocAlignment = alignof(T);

	/// Pointer to the used malloc
	MallocT * malloc;

public:
	/// Default constructor
	FORCE_INLINE MallocObject(MallocT * inMalloc)
		: malloc{inMalloc} {}
	
	/**
	 * Alloc space for n objects of type T
	 * 
	 * @param [in] n num of objects
	 * @return address of alloated memory
	 */
	FORCE_INLINE T * alloc(uint32 n)
	{
		return reinterpret_cast<T*>(malloc->alloc(allocUnitSize, allocAlignment));
	}
	
	/**
	 * Realloc space for n objects of type T
	 * 
	 * @param [in] orig previously allocated memory
	 * @param [in] n num of objects
	 * @return address of alloated memory
	 */
	FORCE_INLINE T * realloc(T * orig, uint32 n)
	{
		return reinterpret_cast<T*>(malloc->realloc((void*)orig, allocUnitSize, allocAlignment));
	}
	
	/**
	 * Free allocated space
	 * 
	 * @param [in] orig previously allocated memory
	 */
	FORCE_INLINE void free(T * orig)
	{
		malloc->free((void*)orig);
	}
};