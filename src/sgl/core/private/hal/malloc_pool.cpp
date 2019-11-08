#include <hal/malloc_pool.h>
#include <misc/assert.h>
#include <hal/platform_math.h>

//////////////////////////////////////////////////
// MemoryPool
//////////////////////////////////////////////////

MemoryPool::MemoryPool(uint32 inNumBlocks, sizet inBlockSize, sizet inBlockAlignment, void * inBuffer)
	: buffer{inBuffer}
	, bHasOwnBuffer{inBuffer == nullptr}
	, numBlocks{inNumBlocks}
	, blockSize{inBlockSize}
	, blockAlignment{PlatformMath::max(inBlockAlignment, MIN_ALIGNMENT)}
	, numFreeBlocks{numBlocks}
	, logicalBlockSize{PlatformMath::align2Up(blockLinkSize + blockSize - 1, blockAlignment)}
	, bufferSize{logicalBlockSize * numBlocks}
{
	// Create buffer if necessary
	if (bHasOwnBuffer)
	{
	#if PLATFORM_UNIX
		if (UNLIKELY(::posix_memalign(&buffer, blockAlignment, bufferSize) != 0))	
			// TODO: handle error
			;
	#else
		// TODO: Windows code
	#endif
	}
	
	CHECKF((reinterpret_cast<uintp>(buffer) & (blockAlignment - 1)) == 0, "buffer is not correctly aligned (required alignment %llx)", blockAlignment)

	head = buffer;
	
	// Init linked list
	void ** link = nullptr;
	while ((uintp)head < (uintp)buffer + bufferSize)
	{
		link = getBlockLink(head);
		head = (void*)((uintp)head + logicalBlockSize);
		*link = head;
	}
	
	// Reset head and last link
	head = buffer;
	*link = nullptr;
}

void * MemoryPool::acquireBlock()
{
	// Pool exhausted
	if (UNLIKELY(head == nullptr)) return nullptr;

	// Unlink head
	void * out = head;
	head = getNextBlock(head);

	--numFreeBlocks;

	return out;
}

void MemoryPool::releaseBlock(void * block)
{
	CHECKF(hasBlock(block), "trying to free a block allocated from another pool or allocator")

	if (LIKELY(head))
	{
		*getBlockLink(block) = head;
		head = block;
	}
	else
		*getBlockLink(head = block) = nullptr;

	++numFreeBlocks;
}

//////////////////////////////////////////////////
// MallocPool
//////////////////////////////////////////////////

void * MallocPool::alloc(sizet size, sizet alignment)
{
	CHECKF(size <= pool.blockSize, "requested size exceed pool block max size, backup allocator will be used (pool block size is 0x%llx, requested size is 0x%llx)", pool.blockSize, size)
	CHECKF(alignment <= pool.blockAlignment, "requested alignment exceeds pool block alignment, backup allocator will be used (pool block alignment is 0x%llx, requested alignment is 0x%llx)", pool.blockAlignment, alignment)

	void * out = nullptr;

	if (LIKELY(pool.getNumFreeBlocks() > 0 && size <= pool.blockSize && alignment <= pool.blockAlignment))
		out = pool.acquireBlock();
	else
		; // TODO: use backup allocator
	
	return out;
}

void * MallocPool::realloc(void * orig, sizet size, sizet alignment)
{
	CHECKF(size <= pool.blockSize, "requested size exceed pool block max size, backup allocator will be used (pool block size is 0x%llx, requested size is 0x%llx)", pool.blockSize, size)
	CHECKF(alignment <= pool.blockAlignment, "requested alignment exceeds pool block alignment, backup allocator will be used (pool block alignment is 0x%llx, requested alignment is 0x%llx)", pool.blockAlignment, alignment)

	void * out = orig;

	if (LIKELY(pool.hasBlock(orig)))
	{
		if (size > pool.blockSize)
		{
			pool.releaseBlock(orig);
		}
		else
			; // Return original block
	}
	else
		; // TODO: use backup allocator

	return out;
}

void MallocPool::free(void * orig)
{
	if (LIKELY(pool.hasBlock(orig)))
		pool.releaseBlock(orig);
	else
		; // TODO: use backup allocator
}

//////////////////////////////////////////////////
// MallocPooled
//////////////////////////////////////////////////

MallocPooled::MallocPooled(uint32 inNumBlocks, sizet inBlockSize, sizet inBlockAlignment, uint32 initialNumPools)
	: head{nullptr}
	, root{nullptr}
	, numPools{0}
	, poolInfo{
		inNumBlocks,
		inBlockSize,
		inBlockAlignment,
		inNumBlocks * MemoryPool::computeLogicalBlockSize(inBlockSize, inBlockAlignment)
	}
	, poolAllocSize{0}
{
	// Determine pool allocation size
	poolAllocSize += poolInfo.bufferSize;
	poolAllocSize += sizeof(MemoryPool);
	poolAllocSize += sizeof(Node);
	poolAllocSize += sizeof(Link);

	// Allocate initial buffer
	for (uint32 poolIdx = 0; poolIdx < initialNumPools; ++poolIdx)
	{
		void * buffer = nullptr;
		if (posix_memalign(&buffer, PlatformMath::max(poolInfo.blockAlignment, MIN_ALIGNMENT), poolAllocSize) == 0)
			createPool(buffer);
	}

	CHECK(head != nullptr && root != nullptr)
}

bool MallocPooled::hasBlock(void * orig)
{
	CHECK(orig != nullptr)

	// Traverse tree to find memory pool
	// that could own this block
	Node * it = root;
	while (it)
	{
		if (it->data->hasBlock(orig))
			return true;
		else if (orig < it->data->buffer)
			it = it->left;
		else
			it = it->right;
	}

	return false;
}

void * MallocPooled::alloc(sizet size, sizet alignment)
{
	CHECKF(size <= poolInfo.blockSize, "requested size exceed pool block max size, backup allocator will be used (pool block size is 0x%llx, requested size is 0x%llx)", poolInfo.blockSize, size)
	CHECKF(alignment <= poolInfo.blockAlignment, "requested alignment exceeds pool block alignment, backup allocator will be used (pool block alignment is 0x%llx, requested alignment is 0x%llx)", poolInfo.blockAlignment, alignment)

	void * out = nullptr;

	if (size <= poolInfo.blockSize && alignment <= poolInfo.blockAlignment)
	{
		MemoryPool * pool = head->data;

		// Create a new pool if necessary
		if (pool->isExhausted())
		{
			void * buffer = nullptr;
			if (posix_memalign(&buffer, poolInfo.blockAlignment, poolAllocSize) == 0)
				pool = createPool(buffer);
		}

		// Acquire block from pool
		out = pool->acquireBlock();

		if (pool->isExhausted())
		{
			// Move to back
			head = head->next;
		}
	}
	else
		; // TODO: backup allocator
	
	return out;
}

void * MallocPooled::realloc(void * orig, sizet size, sizet alignment)
{
	CHECKF(size <= poolInfo.blockSize, "requested size exceed pool block max size, backup allocator will be used (pool block size is 0x%llx, requested size is 0x%llx)", poolInfo.blockSize, size)
	CHECKF(alignment <= poolInfo.blockAlignment, "requested alignment exceeds pool block alignment, backup allocator will be used (pool block alignment is 0x%llx, requested alignment is 0x%llx)", poolInfo.blockAlignment, alignment)

	return orig;
}

void MallocPooled::free(void * orig)
{
	CHECK(orig != nullptr)

	// Traverse tree to find memory pool
	// that could own this block
	Node * it = root;
	while (it)
	{
		if (it->data->hasBlock(orig))
			break;
		else if (orig < it->data->buffer)
			it = it->left;
		else
			it = it->right;
	}

	// Release block, return to pool
	if (it) it->data->releaseBlock(orig);
}