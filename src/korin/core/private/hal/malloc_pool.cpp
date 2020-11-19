#include "hal/malloc_pool.h"

namespace
{
	//
} // namespace

//////////////////////////////////////////////////
// MemoryPool
//////////////////////////////////////////////////

#define BLOCK_LOGIC(block) (reinterpret_cast<void**>(reinterpret_cast<ubyte*>(block) + (blockSize - blockLogicSize)))

void MemoryPool::destroy()
{
	if (hasOwnBuffer)
	{
		// Destroy managed buffer
#if PLATFORM_UNIX
		::free(buffer);
#else
		// TODO: Windows implementation
#endif
		hasOwnBuffer = false;
	}

	buffer = nullptr;
	head = nullptr;
	numFreeBlocks = 0;
}

MemoryPool::MemoryPool(const SetupInfo & inSetupInfo, void * inBuffer)
	: buffer{inBuffer}
	, hasOwnBuffer{inBuffer == nullptr}
	, head{nullptr}
	, numFreeBlocks{inSetupInfo.numBlocks}
	, numBlocks{inSetupInfo.numBlocks}
	, blockDataSize{inSetupInfo.dataSize}
	, blockAlignment{inSetupInfo.dataAlignment}
	, blockSize{PlatformMath::align2Up((inSetupInfo.dataSize + blockLogicSize) - 1, inSetupInfo.dataAlignment)}
{
	if (hasOwnBuffer)
	{
		// Create buffer managed by the pool
#if PLATFORM_UNIX
		if (UNLIKELY(::posix_memalign(&buffer, blockAlignment, numFreeBlocks * blockSize) != 0))
			// TODO: handle error
			;
#else
		// TODO: Windows code
#endif
	}

	// Initialize list of blocks
	head = buffer; for (uint32 idx = 0; idx < numBlocks - 1; ++idx)
	{
		head = *BLOCK_LOGIC(head) = reinterpret_cast<ubyte*>(head) + blockSize;
	}

	*BLOCK_LOGIC(head) = nullptr;
	head = buffer;
}

void * MemoryPool::acquire()
{
	if (LIKELY(head != nullptr))
	{
		// Return first free block and set
		// next free block
		void * block = head;
		head = *BLOCK_LOGIC(head);

		numFreeBlocks--;

		return block;
	}
	
	// Pool exhausted
	return nullptr;
}

void MemoryPool::release(void * block)
{
	CHECKF(isBlockInRange(block), "Block %p was not acquired from this pool", block)
	
	// Prepend to list of free blocks
	*BLOCK_LOGIC(block) = head;
	head = block;

	numFreeBlocks++;
}
