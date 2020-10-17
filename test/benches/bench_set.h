#pragma once

#include "benchmark/benchmark.h"
#include "./bench_util.h"

#include "templates/functional.h"
#include "containers/set.h"

#include <set>

/**
 * Korin ordered set implementation
 */
void korinSet(benchmark::State & state)
{
	const uint32 numNodes = state.range(0);
	Set<uint32> set;

	for (auto _ : state)
	{
		for (uint32 i = 0; i < numNodes; ++i)
			set.set(i);
		
		for (uint32 i = 0; i < numNodes; ++i)
			set.remove(i);
	}

	doNotOptimizeAway(&set);
}

/**
 * Std ordered set implementation
 */
void stdSet(benchmark::State & state)
{
	const uint32 numNodes = state.range(0);
	std::set<uint32> set;

	for (auto _ : state)
	{
		for (uint32 i = 0; i < numNodes; ++i)
			set.insert(i);
		
		for (uint32 i = 0; i < numNodes; ++i)
			set.erase(i);
	}

	doNotOptimizeAway(&set);
}

BENCHMARK(korinSet)->RangeMultiplier(0x2)->Ranges({{0x1, 0x10000}});
BENCHMARK(stdSet)->RangeMultiplier(0x2)->Ranges({{0x1, 0x10000}});