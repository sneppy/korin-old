#pragma once

#include "core_types.h"
#include "platform_memory.h"
#include "platform_math.h"
#include "containers/array.h"
#include "containers/list.h"
#include "containers/tree.h"

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

protected:
	/**
	 * Computes logical block size
	 * 
	 * @param [in] blockSize required block size
	 * @param [in] blockAlignment required block alignment
	 * @return aligned block size
	 */
	static constexpr FORCE_INLINE sizet computeLogicalBlockSize(sizet blockSize, sizet blockAlignment)
	{
		return PlatformMath::align2Up(blockSize + blockLinkSize - 1, blockAlignment);
	}

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
	 * Returns number of free blocks
	 */
	FORCE_INLINE uint32 getNumFreeBlocks() const
	{
		return numFreeBlocks;
	}

	/**
	 * Returns true if pool is exhausted
	 */
	FORCE_INLINE bool isExhausted() const
	{
		return head == nullptr;
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
 * A MallocPool with static block num, size
 * and alignment (can be used as template
 * argument)
 */
template<uint32 inNumBlocks, uint32 inBlockSize, sizet inBlockAlignment = DEFAULT_ALIGNMENT>
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

/**
 * Similar to a malloc pool but with a dynamic
 * list of pools that can grow unbounded.
 * 
 * When pools are exhausted they are moved to
 * the back of the list. When a pool is freed
 * is moved back to the front. This ensure that
 * we can always fetch the head of the list.
 * 
 * If the head of the list is exhausted a new
 * pool is created
 * 
 * If a allocation request is made with a size
 * larger than the pool size a backup allocator
 * is used.
 * 
 * Pools cannot have specific alignments.
 * 
 * A tree is used to quickly find the pool that
 * allocated a chunk of memory
 * 
 * The initial number of pools can be choose
 */
class MallocPooled : public MallocBase
{
protected:
	/**
	 * Compare class used to find memory pool
	 */
	struct FindPool
	{
		FORCE_INLINE int32 operator()(const MemoryPool * a, const MemoryPool * b) const
		{
			return int32(a->buffer > b->buffer) - int32(a->buffer < b->buffer);
		}
	};

	/// Link type
	using Link = ::Link<MemoryPool*>;

	/// Node type
	using Node = ::BinaryNode<MemoryPool*, FindPool>;

	/// Head of memory pool list
	Link * head;

	/// Root of memory pool tree
	Node * root;

	/// Toatal number of pool
	uint32 numPools;

	/**
	 * Static pool description
	 */
	struct
	{
		/// Number of blocks
		uint32 numBlocks;

		/// Size of a pool block (in Bytes)
		sizet blockSize;

		/// Block alignment (in Bytes)
		sizet blockAlignment;

		/// Buffer size
		sizet bufferSize;
	} const poolInfo;

	/// Pool allocation size
	sizet poolAllocSize;

protected:
	/**
	 * Create and return a new pool in buffer
	 * Also updates list and tree
	 * 
	 * @param [in] buffer pool buffer
	 */
	FORCE_INLINE MemoryPool * createPool(void * buffer)
	{
		CHECK(buffer != nullptr)

		++numPools;

		// Create pool structure after buffer
		MemoryPool * pool = new (reinterpret_cast<MemoryPool*>(reinterpret_cast<uintp>(buffer) + poolInfo.bufferSize)) MemoryPool{poolInfo.numBlocks, poolInfo.blockSize, poolInfo.blockAlignment, buffer};

		// Add to tree
		Node * node = new (reinterpret_cast<Node*>(pool + 1)) Node{pool};
		if (root)
		{
			root->insert(node);
			root = root->getRoot();
		}
		else
		{
			root = node;
			root->color = BinaryNodeColor::BLACK;
		}

		// Add to list
		Link * link = new (reinterpret_cast<Link*>(node + 1)) Link{pool};
		if (head)
		{
			link->next = head;
			link->prev = head->prev;
			link->next->prev = link;
			link->prev->next = link;
			head = link;
		}
		else
		{
			head = link;
			head->prev = head->next = head;
		}

		return pool;
	}

	/**
	 * Destroy pool
	 * 
	 * @param [in] pool pointer to the pool structure
	 */
	FORCE_INLINE void destroyPool(MemoryPool * pool)
	{
		void * buffer = pool->buffer;
		Node * node = reinterpret_cast<Node*>(pool + 1);
		Link * link = reinterpret_cast<Link*>(node + 1);

		// Remove from list
		// TODO
		link->~Link();

		// Remove from tree
		if (node->remove() == root)
			root = root->right;
		
		node->~Node();
		
		// Destroy pool
		pool->~MemoryPool();

		// Dealloc buffer
		::free(buffer);
	}

public:
	/**
	 * Pool constructor
	 * 
	 * @see MemoryPool::MemoryPool
	 */
	MallocPooled(uint32 inNumBlocks, sizet inBlockSize, sizet inBlockAlignment = DEFAULT_ALIGNMENT, uint32 initialNumPools = 1);

	/**
	 * Returns number of pools created
	 */
	FORCE_INLINE uint32 getNumPools() const
	{
		return numPools;
	}

	//////////////////////////////////////////////////
	// MallocBase interface
	//////////////////////////////////////////////////
	
	virtual void * alloc(sizet size, sizet alignment = DEFAULT_ALIGNMENT) override;
	virtual void * realloc(void * orig, sizet size, sizet alignment = DEFAULT_ALIGNMENT) override;
	virtual void free(void * orig) override;
};