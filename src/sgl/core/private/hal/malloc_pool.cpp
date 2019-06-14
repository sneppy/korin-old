#include "hal/malloc_pool.h"

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
		if (UNLIKELY(posix_memalign(&buffer, blockAlignment, bufferSize) != 0))	
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

void * MemoryPool::acquire()
{
	// Pool exhausted
	if (UNLIKELY(head == nullptr)) return nullptr;

	// Unlink head
	void * out = head;
	head = getNextBlock(head);

	--numFreeBlocks;

	return out;
}

void MemoryPool::release(void * block)
{
#if BUILD_DEBUG
	if (!hasBlock(block))
		printf("ERROR: block @ %p is not owned by pool @ %p\n", block, this);
#endif

	if (LIKELY(head))
	{
		*getBlockLink(block) = head;
		head = block;
	}
	else
		*getBlockLink(head = block) = nullptr;

	++numFreeBlocks;
}