#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "containers/containers_types.h"
#include "./malloc_pool.h"

class MallocMultiPool;

/**
 * 
 */
class MultiMemoryPool : public NonCopyable
{
	friend MallocMultiPool;

	using PoolLogicT = InlineList<MemoryPool>;

	MultiMemoryPool() = delete;

	/**
	 * Creates a new pool and pushes it
	 * onto the stack of pools.
	 */
	void tryCreatePool();

	/**
	 * Destroy a single pool.
	 * 
	 * @param pool ptr to pool to destroy
	 */
	void destroyPool(PoolLogicT * pool);

	/**
	 * Destroy all pools and reset state.
	 */
	void destroy();

	/**
	 * Returns ptr to pool that may
	 * own this block.
	 * 
	 * @param block ptr to memory block
	 * 	owned by the pool
	 * @return ptr to pool that may own
	 * 	the block or nullptr
	 */
	PoolLogicT * findPool(void * block);

public:
	/**
	 * Construct a new multi pool without
	 * creating any pool.
	 */
	FORCE_INLINE MultiMemoryPool(const MemoryPool::SetupInfo & inSetupInfo)
		: head{nullptr}
		, tail{nullptr}
		, setupInfo{inSetupInfo}
	{
		//
	}

	/**
	 * Move constructor.
	 */
	FORCE_INLINE MultiMemoryPool(MultiMemoryPool && other)
		: head{other.head}
		, tail{other.tail}
		, setupInfo{other.setupInfo}
	{
		other.head = other.tail = nullptr;
	}

	/**
	 * Move assignment.
	 */
	FORCE_INLINE MultiMemoryPool & operator=(MultiMemoryPool && other)
	{
		// Destroy this first
		destroy();

		// Move from other
		head = other.head;
		tail = other.tail;
		setupInfo = other.setupInfo;

		other.head = other.tail = nullptr;

		return *this;
	}

	/**
	 * Destructor, destroys all pools.
	 */
	FORCE_INLINE ~MultiMemoryPool()
	{
		destroy();
	}

	/**
	 * Acquire a block of memory from the
	 * first non-exhausted pool. If no
	 * suitable pool exists, creates a new
	 * one.
	 */
	void * acquire();

	/**
	 * Release a previously acquired block.
	 * The block must have been acquired by
	 * one the pools.
	 */
	void release(void * block);

	/**
	 * Returns true if block was acquired
	 * from this instance.
	 */
	bool isBlockInRange(void * block);

private:
	/// First non-exhausted pool
	PoolLogicT * head;

	/// Tail of pools stack
	PoolLogicT * tail;

	/// Pool setup info
	MemoryPool::SetupInfo setupInfo;
};

/**
 * Malloc implementation that uses a multi
 * memory pool instance.
 */
class MallocMultiPool : public MallocBase
{
public:
	/**
	 * Initialize memory pool
	 */
	FORCE_INLINE MallocMultiPool(const MemoryPool::SetupInfo & inSetupInfo)
		: multiPool{inSetupInfo}
	{
		//
	}

	//////////////////////////////////////////////////
	// MallocBase interface
	//////////////////////////////////////////////////
	
	virtual void * alloc(sizet, sizet = DEFAULT_ALIGNMENT) override;
	virtual void * realloc(void*, sizet, sizet = DEFAULT_ALIGNMENT) override;
	virtual void free(void*) override;

protected:
	/// The memory pool
	MultiMemoryPool multiPool;
};

FORCE_INLINE void * MallocMultiPool::alloc(sizet size, sizet alignment)
{
	CHECKF(size <= multiPool.setupInfo.dataSize, "Request size (%llu) exceeds max size of pool block (%llu)", size, multiPool.setupInfo.dataSize)
	CHECKF(alignment <= multiPool.setupInfo.dataAlignment, "Request alignment (%llu) exceeds alignment of pool block (%llu)", alignment, multiPool.setupInfo.dataAlignment)
	
	return multiPool.acquire();
}

FORCE_INLINE void * MallocMultiPool::realloc(void * orig, sizet size, sizet alignment)
{
	// We should never realloc in a pool
	CHECKF(false, "A memory pool cannot reallocate memory")
	return nullptr;
}

FORCE_INLINE void MallocMultiPool::free(void * orig)
{
	multiPool.release(orig);
}
