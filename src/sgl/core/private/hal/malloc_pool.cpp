#include "hal/malloc_pool.h"
#include "misc/assert.h"

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
	CHECKF(size <= pool.blockSize, "requested size exceed pool block max size, backup allocator will be used (pool block size is 0x%x, requested size is 0x%x)", pool.blockSize, size)
	CHECKF(alignment <= pool.blockAlignment, "requested alignment exceeds pool block alignment, backup allocator will be used (pool block alignment is 0x%x, requested alignment is 0x%x)", pool.blockAlignment, alignment)

	void * out = nullptr;

	if (LIKELY(pool.getNumFreeBlocks() > 0 && size <= pool.blockSize && alignment <= pool.blockAlignment))
		out = pool.acquireBlock();
	else
		; // TODO: use backup allocator
	
	return out;
}

void * MallocPool::realloc(void * orig, sizet size, sizet alignment)
{
	CHECKF(size <= pool.blockSize, "requested size exceed pool block max size, backup allocator will be used (pool block size is 0x%x, requested size is 0x%x)", pool.blockSize, size)
	CHECKF(alignment <= pool.blockAlignment, "requested alignment exceeds pool block alignment, backup allocator will be used (pool block alignment is 0x%x, requested alignment is 0x%x)", pool.blockAlignment, alignment)

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