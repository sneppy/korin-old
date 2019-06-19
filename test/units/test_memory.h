#pragma once

#include <gtest/gtest.h>

#include "hal/platform_memory.h"
#include "hal/malloc_ansi.h"
#include "hal/malloc_pool.h"
#include "hal/malloc_object.h"

TEST(memory, malloc_ansi)
{
	void * dst = nullptr, * src = nullptr;
	MallocAnsi * malloc = new MallocAnsi;

	// Do some allocations
	dst = malloc->alloc(4096, 32);
	src = malloc->alloc(8192, 16);
	
	ASSERT_TRUE(dst != nullptr);
	ASSERT_TRUE(src != nullptr);

	malloc->free(dst);
	malloc->free(src);

	// Check alignment
	dst = malloc->alloc(65536, 1u << 10);
	ASSERT_EQ(reinterpret_cast<uintp>(dst) & ((1u << 10) - 1), 0x0);

	dst = malloc->realloc(dst, 65536, 1u << 16);
	ASSERT_EQ(reinterpret_cast<uintp>(dst) & ((1u << 16) - 1), 0x0);

	delete malloc;

	SUCCEED();
}

TEST(memory, memory_pool)
{
	const uint32 numBlocks = 4096;
	const uint32 blockSize = 1024;
	const uint32 blockAlignment = 64;

	MemoryPool * pool = new MemoryPool(numBlocks, blockSize, blockAlignment, nullptr);
	ASSERT_EQ(pool->getNumFreeBlocks(), numBlocks);

	void * block = pool->acquireBlock();
	ASSERT_TRUE(block != nullptr);
	ASSERT_EQ((uintp)block & (blockAlignment - 1), 0);
	ASSERT_EQ(pool->getNumFreeBlocks(), numBlocks - 1);

	pool->releaseBlock(block);
	ASSERT_EQ(pool->getNumFreeBlocks(), numBlocks);

	struct 
	{
		/**
		 * Remove all blocks
		 */
		void operator()(MemoryPool * pool, uint32 & numBlocks) const
		{
			void * block = pool->acquireBlock();

			if (LIKELY(block != nullptr))
			{
				++numBlocks;
				operator()(pool, numBlocks);
				pool->releaseBlock(block);
			}
		}
	} cycleBlocks;

	// Cycle all blocks twice
	uint32 testedNumBlocks;

	cycleBlocks(pool, testedNumBlocks = 0);
	ASSERT_EQ(testedNumBlocks, numBlocks);

	cycleBlocks(pool, testedNumBlocks = 0);
	ASSERT_EQ(testedNumBlocks, numBlocks);

	delete pool;

	SUCCEED();
}

TEST(memory, malloc_pooled)
{
	struct Foo
	{
		float32 vec[4];
		void * next;
	} * foo;
	
	MallocPooled * malloc = new MallocPooled(65536, sizeof(Foo), alignof(Foo), 8);

	foo = reinterpret_cast<Foo*>(malloc->alloc(sizeof(Foo), alignof(Foo)));
	foo->next = foo;
	foo->vec[0] = 3.14f;

	ASSERT_TRUE(foo != nullptr);

	for (uint32 i = 0; i < 65536 * 15; ++i)
	{
		foo = reinterpret_cast<Foo*>(malloc->alloc(sizeof(Foo), alignof(Foo)));
		foo->vec[0] = (float32)i;
	}
	
	ASSERT_EQ(malloc->getNumPools(), 16);

	SUCCEED();
}

TEST(memory, malloc_object)
{
	struct Foo
	{
		uint32 a;
		float32 b;
		void * something;
	} * foo;

	MallocAnsi * innerMalloc = new MallocAnsi;
	MallocObject<Foo, MallocAnsi> * malloc = new MallocObject<Foo, MallocAnsi>(innerMalloc);

	// Do some allocations
	foo = malloc->alloc(1);
	foo->a = 1;
	foo->b = 3.14f;

	foo = malloc->realloc(foo, 16);
	foo[0].a = 2;
	foo[10].something = foo;

	malloc->free(foo);

	delete malloc;
	delete innerMalloc;

	SUCCEED();
}