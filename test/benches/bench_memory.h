#pragma once

#include <benchmark/benchmark.h>

#include <core_types.h>
#include <hal/malloc_ansi.h>
#include <hal/malloc_pool.h>

#ifndef DO_NOT_OPTIMIZE_AWAY_IMPL
#define DO_NOT_OPTIMIZE_AWAY_IMPL

	static FORCE_INLINE void doNotOptimizeAway(void * p)
	{
		asm volatile("" : : "g" (p) : "memory");
	}

#endif

template<typename MallocT>
MallocBase * createAllocator(uint32 n, sizet size, sizet align = sizeof(uintp));

FORCE_INLINE MallocBase * destroyAllocator(MallocBase * malloc)
{
	delete malloc;
	return nullptr;
}

template<>
FORCE_INLINE MallocBase * createAllocator<MallocAnsi>(uint32 n, sizet size, sizet align)
{
	return new MallocAnsi;
}

template<>
FORCE_INLINE MallocBase * createAllocator<MallocPooled>(uint32 n, sizet size, sizet align)
{
	return new MallocPooled{n / 8, size, align};
}

template<>
FORCE_INLINE MallocBase * createAllocator<MallocPool>(uint32 n, sizet size, sizet align)
{
	return new MallocPool{n, size, align};
}

template<typename MallocT>
void uniformAlloc(benchmark::State & state)
{
	// Get test ranges
	const uint32 numBlocks = state.range(0);
	const sizet blockSize = state.range(1);

	// Create allocator
	MallocBase * malloc = createAllocator<MallocT>(numBlocks, blockSize);
	void * blocks[numBlocks] = {};

	for (auto _ : state)
	{
		// Ansi allocation
		for (uint32 i = 0; i < numBlocks; ++i)
			blocks[i] = malloc->alloc(blockSize);
		
		for (uint32 i = 0; i < numBlocks; ++i)
			if (blocks[i]) malloc->free(blocks[i]);
	}

	// Destroy allocator
	destroyAllocator(malloc);
}

BENCHMARK_TEMPLATE(uniformAlloc, MallocAnsi)->Ranges({{1u << 3, 1u << 15}, {1u << 3, 1u << 9}});
BENCHMARK_TEMPLATE(uniformAlloc, MallocPool)->Ranges({{1u << 3, 1u << 15}, {1u << 3, 1u << 9}});
BENCHMARK_TEMPLATE(uniformAlloc, MallocPooled)->Ranges({{1u << 3, 1u << 15}, {1u << 3, 1u << 9}});