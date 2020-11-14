#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "misc/build.h"
#include "./platform.h"
#include "./platform_memory.h"
#include "./platform_math.h"

class MallocPool;

/**
 * A memory pool is a chunk of memory
 * that consists of memory blocks
 * organized in a linked list.
 */
struct MemoryPool final
{
	friend MallocPool;

	MemoryPool() = delete;

public:
	/**
	 * Memory pool setup data.
	 */
	struct SetupData
	{
		/// Number of blocks
		uint32 numBlocks;

		/// Size of the data part of a block
		sizet dataSize;

		/// Alignment of the data
		sizet dataAlignment;
	};

	/**
	 * Create a new pool with the given setup.
	 * 
	 * @param inSetupData pool setup data
	 * @param inBuffer if provided, a pointer
	 * 	to an external buffer managed by a
	 * 	third party
	 */
	explicit MemoryPool(const SetupData & inSetupData, void * inBuffer = nullptr);

	/**
	 * Destructor, destroys managed buffer
	 * if any.
	 */
	~MemoryPool();

	/**
	 * 
	 */
	void * acquire();

	/**
	 * 
	 */
	void release(void * block);

	/**
	 * 
	 */
	FORCE_INLINE bool isBlockInRange(const void * block)
	{
		return Memory::isInRange(block, buffer, reinterpret_cast<char*>(buffer) + (numBlocks * blockSize));
	}

private:
	/// Pool buffer
	void * buffer;

	/// True if pool manages its own buffer
	bool hasOwnBuffer;

	/// Pointer to the first free chunk
	void * head;

	/// Number of free blocks
	uint32 numFreeBlocks;

	/// Number of total blocks in the pool
	const uint32 numBlocks;

	/// Size of the data of a block
	const sizet blockDataSize;

	/// Size of the logical part of a block
	static constexpr sizet blockLogicSize = sizeof(void*);

	/// Required data alignment
	const sizet blockAlignment;

	/// Actual block size
	const sizet blockSize;
};

/**
 * Allocator implementation that uses
 * a single memory pool.
 */
struct MallocPool : MallocBase
{
	/**
	 * Initialize memory pool
	 */
	FORCE_INLINE MallocPool(const MemoryPool::SetupData & inSetup, void * inBuffer = nullptr)
		: pool{inSetup, inBuffer}
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
	MemoryPool pool;
};

FORCE_INLINE void * MallocPool::alloc(sizet size, sizet alignment)
{
	CHECKF(size <= pool.blockDataSize, "Request size (%llu) exceeds max size of pool block (%llu)", size, pool.blockDataSize)
	CHECKF(alignment <= pool.blockAlignment, "Request alignment (%llu) exceeds alignment of pool block (%llu)", alignment, pool.blockAlignment)
	
	return pool.acquire();
}

FORCE_INLINE void * MallocPool::realloc(void * orig, sizet size, sizet alignment)
{
	// We should never realloc in a pool
	CHECKF(false, "A memory pool cannot reallocate memory")
	return nullptr;
}

FORCE_INLINE void MallocPool::free(void * orig)
{
	pool.release(orig);
}
