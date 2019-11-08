#pragma once

#include "core_types.h"
#include "platform_memory.h"
#include "../templates/utility.h"

/**
 * 
 */
template<typename T, typename MallocT>
class MallocObject
{
protected:
	/// Allocation unit size
	constexpr static sizet allocUnitSize = sizeof(T);

	/// Allocation alignment
	constexpr static sizet allocUnitAlign = alignof(T);

	/// Pointer to the used malloc
	MallocT * malloc;

	/// Has own allocator flag
	bool bHasOwnMalloc;

public:
	/**
	 * Create object allocator with given malloc.
	 */
	explicit FORCE_INLINE MallocObject(MallocT * inMalloc = nullptr)
		: malloc{inMalloc}
		, bHasOwnMalloc{inMalloc == nullptr}
	{
		// Create default if none specified
		if (bHasOwnMalloc) malloc = new MallocT;
	}

	/**
	 * Move constructor.
	 */
	FORCE_INLINE MallocObject(MallocObject && other)
		: malloc{other.malloc}
		, bHasOwnMalloc{other.bHasOwnMalloc}
	{
		other.malloc = nullptr;
		other.bHasOwnMalloc = false;
	}

	/**
	 * Move operator.
	 */
	FORCE_INLINE MallocObject & operator=(MallocObject && other)
	{
		// Delete and replace managed allocator
		if (bHasOwnMalloc) delete malloc;
		malloc = other.malloc;
		bHasOwnMalloc = other.bHasOwnMalloc;

		other.malloc = nullptr;
		other.bHasOwnMalloc = false;

		return *this;
	}
	
	/**
	 * List constructor, arguments are passed
	 * to underlying allocator constructor.
	 */
	template<typename ... Args>
	FORCE_INLINE MallocObject(Args && ... args)
		: bHasOwnMalloc{true}
	{
		// Create allocator
		malloc = new MallocT{forward<Args>(args) ...};
	}

	/**
	 * Destructor.
	 */
	FORCE_INLINE ~MallocObject()
	{
		// Delete managed allocator
		if (bHasOwnMalloc) delete malloc;
	}
	
	/**
	 * Alloc space for n objects of type T.
	 * 
	 * @param n num of objects
	 * @returns address of alloated memory
	 */
	FORCE_INLINE T * alloc(uint64 n)
	{
		return reinterpret_cast<T*>(malloc->alloc(n * allocUnitSize, allocUnitAlign));
	}
	
	/**
	 * Realloc space for n objects of type T.
	 * 
	 * @param orig previously allocated memory
	 * @param n num of objects
	 * @returns address of alloated memory
	 */
	FORCE_INLINE T * realloc(T * orig, uint64 n)
	{
		return reinterpret_cast<T*>(malloc->realloc((void*)orig, n * allocUnitSize, allocUnitAlign));
	}
	
	/**
	 * Free allocated space.
	 * 
	 * @param orig previously allocated memory
	 */
	FORCE_INLINE void free(T * orig)
	{
		malloc->free((void*)orig);
	}
};

template<typename T, typename MallocT>
constexpr sizet MallocObject<T, MallocT>::allocUnitSize;

template<typename T, typename MallocT>
constexpr sizet MallocObject<T, MallocT>::allocUnitAlign;