#pragma once

#include "core_types.h"
#include "platform_memory.h"
#include "platform_math.h"

/**
 * A single pool allocator consists of
 * a single memory pool split in blocks
 * of fixed size. Blocks are linked via
 * a linked list embedded in the blocks
 * itself.
 * 
 * All blocks are aligned to the requested
 * alignment value.
 * 
 * If the pool is exhausted a null address
 * is returned.
 */
class MemoryPool
{
	friend class MallocPool;
	friend class MallocPooled;

protected:
	/// Fixed link information size
	static constexpr sizet blockLinkSize = sizeof(void*);

	/// Pool buffer
	void * buffer;

	/// Manages own buffer flag
	bool bHasOwnBuffer;

	/// Total number of blocks
	const uint32 numBlocks;

	/// Size of a block
	const sizet blockSize;

	/// Memory alignment
	const sizet blockAlignment;

	/// Number of free blocks
	uint32 numFreeBlocks;

	/// Logical block size (including linked list)
	const sizet logicalBlockSize;

	/// Actual buffer size (in Bytes)
	const sizet bufferSize;

private:
	/// Head of free blocks list
	void * head;

public:
	/**
	 * Pool constructor
	 * 
	 * @param [in] inNumBlocks number of blocks inside the pool
	 * @param [in] inBlockSize size of a single memory block
	 * @param [in] inBlockAlignment blocks alignment
	 */
	MemoryPool(uint32 inNumBlocks, sizet inBlockSize, sizet inBlockAlignment = DEFAULT_ALIGNMENT, void * inBuffer = nullptr);

	/**
	 * Destructor, deallocates own buffer
	 */
	FORCE_INLINE ~MemoryPool()
	{
		if (bHasOwnBuffer) ::free(buffer);
	}

	/**
	 * Returns numbe of free blocks
	 */
	FORCE_INLINE uint32 getNumFreeBlocks() const
	{
		return numFreeBlocks;
	}

	/**
	 * Returns true if block was acquired from
	 * this pool
	 * 
	 * @param [in] block block address
	 * @return true or false
	 */
	FORCE_INLINE bool hasBlock(void * block) const
	{
		return (uintp)block >= (uintp)buffer && (uintp)block < (uintp)buffer + bufferSize;
	}

protected:
	/**
	 * Returns address of block link
	 * 
	 * @param [in] block block address
	 * @return link
	 */
	void ** getBlockLink(void * block) const
	{
		return (void**)((uintp)block + blockSize);
	}

	/**
	 * Returns next free block
	 * 
	 * @param [in] block block address
	 * @return address of next free block
	 */
	void * getNextBlock(void * block) const
	{
		return *getBlockLink(block);
	}

public:
	/**
	 * Returns first free block from the pool
	 * 
	 * @return block address
	 */
	void * acquireBlock();

	/**
	 * Release a previously acquired block.
	 * The block is relinked in the free
	 * blocks list
	 * 
	 * @param [in] block previously acquired block
	 */
	void releaseBlock(void * block);
};

/**
 * Malloc wrapper for a single memory pool
 * 
 * If pool is exhausted backs up to c
 * standard allocation
 */
class MallocPool : public MallocBase
{
protected:
	/// Underlying memory pool
	MemoryPool pool;

public:
	/**
	 * Pool constructor
	 * 
	 * @see MemoryPool::MemoryPool
	 */
	FORCE_INLINE MallocPool(uint32 inNumBlocks, sizet inBlockSize, sizet inBlockAlignment = DEFAULT_ALIGNMENT, void * inBuffer = nullptr)
		: pool(inNumBlocks, inBlockSize, inBlockAlignment, inBuffer)
	{
		//
	}

	//////////////////////////////////////////////////
	// MallocBase interface
	//////////////////////////////////////////////////
	
	virtual void * alloc(sizet size, sizet alignment = DEFAULT_ALIGNMENT) override;
	virtual void * realloc(void * orig, sizet size, sizet alignment = DEFAULT_ALIGNMENT) override;
	virtual void free(void * orig) override;
};

/**
 * MallocPool with static block num, size
 * and alignment (can be used as template
 * argument)
 */
template<uint32 inNumBlocks, uint32 inBlockSize, sizet inBlockAlignment>
class MallocPoolInline : public MallocPool
{
public:
	/**
	 * Default constructor
	 * 
	 * @param [in] inBuffer optional buffer not owned by the allocator
	 */
	explicit FORCE_INLINE MallocPoolInline(void * inBuffer = nullptr)
		: MallocPool(inNumBlocks, inBlockSize, inBlockAlignment, inBuffer)
	{
		//
	}
};