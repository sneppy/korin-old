#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "templates/utility.h"
#include "./platform_memory.h"

template<typename, typename = void> class MallocObject;

/**
 * Wrapper around a malloc class that
 * handles allocation for a specific
 * type of object T.
 * 
 * When provided, the second template
 * parameter specify an allocator class
 * to spawn, if none specified.
 * 
 * @param T type of the object
 * @param MallocT allocator type, or void
 */
template<typename T>
class MallocObject<T, void>
{
public:
	/**
	 * Creates object allocator with given malloc.
	 * If malloc is not provided, use global
	 * allocator instead.
	 * 
	 * @param inMalloc allocator that will
	 * 	be used to allocate objects
	 */
	explicit FORCE_INLINE MallocObject(MallocBase * inMalloc = gMalloc)
		: malloc{inMalloc}
	{
		CHECKF(!!inMalloc, "Provided allocator cannot be NULL")
	}

	/**
	 * Move constructor.
	 */
	FORCE_INLINE MallocObject(MallocObject && other)
		: malloc{other.malloc}
	{
		other.malloc = nullptr;
	}

	/**
	 * Move operator.
	 */
	FORCE_INLINE MallocObject & operator=(MallocObject && other)
	{
		malloc = other.malloc;
		other.malloc = nullptr;

		return *this;
	}

	/**
	 * Destructor.
	 */
	FORCE_INLINE ~MallocObject()
	{
		malloc = nullptr;
	}
	
	/**
	 * Alloc space for n objects of type T.
	 * 
	 * @param n num of objects
	 * @returns pointer to allocated memory
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
	 * @returns pointer to alloated memory
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

protected:
	/// Allocation unit size
	static constexpr const sizet allocUnitSize = sizeof(T);

	/// Allocation alignment
	static constexpr const sizet allocUnitAlign = alignof(T);

	/// Pointer to the used allocator
	MallocBase * malloc;
};

/**
 * Generalization for type of allocator.
 * @see MallocObject
 */
template<typename T, typename MallocT>
struct MallocObject : public MallocObject<T, void>, public NonCopyable
{
	using Base = MallocObject<T, void>;

	/**
	 * 
	 */
	template<typename ...MallocCreateArgsT>
	FORCE_INLINE explicit MallocObject(MallocCreateArgsT && ...createArgs)
		: Base{&managedMalloc}
		, managedMalloc{forward<MallocCreateArgsT>(createArgs)...}
	{
		//
	}

protected:
	/// Stack allocated allocator
	MallocT managedMalloc;
};
