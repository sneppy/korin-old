#pragma once

#include "benchmark/benchmark.h"

#include "core_types.h"
#include "containers/tree.h"
#include "containers/pair.h"
#include "containers/map.h"

#include <map>

#ifndef DO_NOT_OPTIMIZE_AWAY_IMPL
#define DO_NOT_OPTIMIZE_AWAY_IMPL

	static FORCE_INLINE void doNotOptimizeAway(void * p)
	{
		asm volatile("" : : "g" (p) : "memory");
	}

#endif

struct LessThan
{
	FORCE_INLINE int32 operator()(uint32 a, uint32 b) const
	{
		return int32(a > b) - int32(a < b);
	}
};

/**
 * SGL ordered map implementation
 */
void sglMap(benchmark::State & state)
{
	const uint32 numNodes = state.range(0);
	Map<uint32, uint32, LessThan> map;

	for (auto _ : state)
	{
		for (uint32 i = 0; i < numNodes; ++i)
			map[i] = i << 1;
		
		for (uint32 i = 0; i < numNodes; ++i)
			map[i] = map[(numNodes - i) - 1];
		
		for (uint32 i = 0; i < numNodes; ++i)
			map.remove(i);
	}

	doNotOptimizeAway(&map);
}

/**
 * Stdlib ordered map implementation
 */
void stdMap(benchmark::State & state)
{
	const uint32 numNodes = state.range(0);
	std::map<uint32, uint32> map;

	for (auto _ : state)
	{
		for (uint32 i = 0; i < numNodes; ++i)
			map[i] = i << 1;
		
		for (uint32 i = 0; i < numNodes; ++i)
			map[i] = map[(numNodes - i) - 1];
		
		for (uint32 i = 0; i < numNodes; ++i)
			map.erase(i);
	}

	doNotOptimizeAway(&map);
}

BENCHMARK(sglMap)->RangeMultiplier(0x2)->Ranges({{0x1, 0x10000}});
BENCHMARK(stdMap)->RangeMultiplier(0x2)->Ranges({{0x1, 0x10000}});