#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "misc/build.h"
#include "templates/types.h"
#include "./platform.h"
#include "./platform_memory.h"
#include "./platform_math.h"

class MallocPool;

/**
 * A memory pool is a chunk of memory
 * that consists of memory blocks
 * organized in a linked list.
 */
struct MemoryPool : public NonCopyable
{
	friend MallocPool;
	friend class MultiMemoryPool;

	MemoryPool() = delete;

	/**
	 * Destroys pool and deallocates
	 * managed buffer.
	 */
	void destroy();

public:
	/**
	 * Memory pool setup data.
	 */
	struct SetupInfo
	{
		/// Number of blocks
		uint32 numBlocks;

		/// Size of the data part of a block
		sizet dataSize;

		/// Alignment of the data
		sizet dataAlignment = DEFAULT_ALIGNMENT;
	};

	/**
	 * 
	 */
	static constexpr FORCE_INLINE sizet getActualBlockSize(const SetupInfo & setupInfo)
	{
		return PlatformMath::align2Up(setupInfo.dataSize + blockLogicSize - 1, setupInfo.dataAlignment);
	}

	/**
	 * Create a new pool with the given setup.
	 * 
	 * @param inSetupInfo pool setup data
	 * @param inBuffer if provided, a pointer
	 * 	to an external buffer managed by a
	 * 	third party
	 */
	explicit MemoryPool(const SetupInfo & inSetupInfo, void * inBuffer = nullptr);

	/**
	 * Move constructor.
	 */
	FORCE_INLINE MemoryPool(MemoryPool && other)
		: buffer{other.buffer}
		, hasOwnBuffer{other.hasOwnBuffer}
		, head{other.head}
		, numFreeBlocks{other.numFreeBlocks}
		, numBlocks{other.numBlocks}
		, blockDataSize{other.blockDataSize}
		, blockAlignment{other.blockAlignment}
		, blockSize{other.blockSize}
	{
		other.buffer = other.head = nullptr;
		other.hasOwnBuffer = false;
		other.numFreeBlocks = 0;
	}

	/**
	 * Move assignment.
	 */
	FORCE_INLINE MemoryPool & operator=(MemoryPool && other)
	{
		// Destroy this pool first
		destroy();

		swap(buffer, other.buffer);
		swap(hasOwnBuffer, other.hasOwnBuffer);
		swap(head, other.head);
		swap(numFreeBlocks, other.numFreeBlocks);
		numBlocks = other.numBlocks;
		blockDataSize = other.blockDataSize;
		blockAlignment = other.blockAlignment;
		blockSize = other.blockSize;

		return *this;
	}

	/**
	 * Destructor, destroys managed buffer
	 * if any.
	 */
	FORCE_INLINE ~MemoryPool()
	{
		destroy();
	}

	/**
	 * Returns true if there are no free
	 * blocks available in this pool.
	 */
	FORCE_INLINE bool isExhausted()
	{
		return !head;
	}

	/**
	 * Returns the number of free blocks
	 * in this pool.
	 */
	FORCE_INLINE uint32 getNumFreeBlocks()
	{
		return numFreeBlocks;
	}

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
		return Memory::isInRange(block, buffer, reinterpret_cast<ubyte*>(buffer) + (numBlocks * blockSize));
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
	uint32 numBlocks;

	/// Size of the data of a block
	sizet blockDataSize;

	/// Size of the logical part of a block
	static constexpr sizet blockLogicSize = sizeof(void*);

	/// Required data alignment
	sizet blockAlignment;

	/// Actual block size
	sizet blockSize;
};

/**
 * Allocator implementation that uses
 * a single memory pool.
 */
struct MallocPool : MallocBase
{
public:
	/**
	 * Initialize memory pool
	 */
	FORCE_INLINE MallocPool(const MemoryPool::SetupInfo & inSetupInfo, void * inBuffer = nullptr)
		: pool{inSetupInfo, inBuffer}
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
