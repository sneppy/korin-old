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
class MallocObject<T, void> : public NonCopyable
{
public:
	/**
	 * Remove default constructor
	 */
	MallocObject() = delete;

	/**
	 * Creates object allocator with given malloc.
	 * 
	 * @param inMalloc allocator that will
	 * 	be used to allocate objects
	 */
	explicit FORCE_INLINE MallocObject(MallocBase * inMalloc)
		: malloc{inMalloc}
		, bHasOwnMalloc{inMalloc == nullptr}
	{
		CHECKF(!!inMalloc, "Provided allocator cannot be NULL")
	}

	/**
	 * Move constructor. Move allocator and reset
	 * own allocator flag.
	 */
	FORCE_INLINE MallocObject(MallocObject && other)
		: malloc{other.malloc}
		, bHasOwnMalloc{other.bHasOwnMalloc}
	{
		other.malloc = nullptr;
		other.bHasOwnMalloc = false;
	}

	/**
	 * Move operator. If has own allocator,
	 * delete it and move other allocator
	 * instead.
	 */
	FORCE_INLINE MallocObject & operator=(MallocObject && other)
	{
		// If has managed allocator, delete it
		if (bHasOwnMalloc) delete malloc;

		malloc = other.malloc;
		bHasOwnMalloc = other.bHasOwnMalloc;

		other.malloc = nullptr;
		other.bHasOwnMalloc = false;

		return *this;
	}

	/**
	 * Destructor, deallocates managed allocator
	 * and sets pointer to NULL.
	 */
	FORCE_INLINE ~MallocObject()
	{
		if (bHasOwnMalloc)
		{
			// Delete managed allocator
			delete malloc;
			bHasOwnMalloc = false;
		}

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

	/// Has own allocator, flag
	ubyte bHasOwnMalloc : 1;
};

/**
 * Generalization for type of allocator.
 * @see MallocObject
 */
template<typename T, typename MallocT>
struct MallocObject : public MallocObject<T, void>
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
