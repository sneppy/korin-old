#include "hal/malloc_multi_pool.h"
#include "containers/inline_list.h"

//////////////////////////////////////////////////
// MultiMemoryPool
//////////////////////////////////////////////////

void MultiMemoryPool::tryCreatePool()
{
	constexpr sizet poolLogicSize = sizeof(PoolLogicT);
	sizet actualBlockSize = MemoryPool::getActualBlockSize(setupInfo);
	sizet poolDataSize = actualBlockSize * setupInfo.numBlocks;

	// Create buffer managed by the pool
	void * buffer = nullptr;
#if PLATFORM_UNIX
	if (UNLIKELY(::posix_memalign(&buffer, setupInfo.dataAlignment, poolDataSize + poolLogicSize) != 0))
		// TODO: handle error
		;
#else
	// TODO: Windows implementation
#endif

	// Create pool
	ubyte * poolLogicAddr = reinterpret_cast<ubyte*>(buffer) + poolDataSize;
	PoolLogicT * pool = new (poolLogicAddr) PoolLogicT{setupInfo, buffer};

	// Update logic
	head = !!head
		? head->pushFront(pool)
		: (tail = pool);
};

void MultiMemoryPool::destroyPool(PoolLogicT * pool)
{
	// Destroy pool first
	void * buffer = pool->buffer;
	pool->~PoolLogicT();

	// Then deallocate buffer
#if PLATFORM_UNIX
	::free(buffer);
#else
	// TODO: Windows implementation
#endif
}

void MultiMemoryPool::destroy()
{
	while (head)
	{
		// Destroy all pools
		PoolLogicT * next = head->next;
		destroyPool(head);
		head = next;
	}

	head = tail = nullptr;
}

MultiMemoryPool::PoolLogicT * MultiMemoryPool::findPool(void * block)
{
	// This is fine, since the most/last used
	// pool will be head of the stack, and thus
	// it will be queried first, which is a nice
	// feature
	for (PoolLogicT * pool = head; pool; pool = pool->next)
	{
		if (pool->isBlockInRange(block))
		{
			// Return pool that owns the block
			return pool;
		}
	}
	
	return nullptr;
}

void * MultiMemoryPool::acquire()
{
	if (!head || head->isExhausted())
	{
		tryCreatePool();
	}
	CHECK(head != nullptr && !head->isExhausted())

	PoolLogicT * pool = head;
	void * block = pool->acquire();

	if (pool->isExhausted())
	{
		if (pool != tail)
		{
			// There is at least another pool

			head = pool->popHead();
			tail = tail->pushBack(pool);
		}
	}

	return block;
}

void MultiMemoryPool::release(void * block)
{
	CHECKF(head != nullptr, "No pool allocated yet.");

	// Find pool that may own this block
	PoolLogicT * pool = findPool(block);
	CHECKF(pool != nullptr && pool->isBlockInRange(block), "Block (%p) was not acquired from this instance.", block)

	pool->release(block);

	// ? Delete if empty?

	if (pool != head)
	{
		// Bring to front, if not already head

		if (pool == tail)
		{
			tail = pool->popTail();
			head = head->pushFront(pool);
		}
		else
		{
			pool->pop();
			head = head->pushFront(pool);
		}
	}
}

bool MultiMemoryPool::isBlockInRange(void * block)
{
	// TODO
	return false;
}
