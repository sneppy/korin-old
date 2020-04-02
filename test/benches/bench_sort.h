#pragma once

#include "benchmark/benchmark.h"

#include "core_types.h"
#include "containers/array.h"
#include "algorithm/sort.h"

#include <vector>
#include <list>

#ifndef DO_NOT_OPTIMIZE_AWAY_IMPL
#define DO_NOT_OPTIMIZE_AWAY_IMPL

	static FORCE_INLINE void doNotOptimizeAway(void * p)
	{
		asm volatile("" : : "g" (p) : "memory");
	}

#endif

template<typename ContainerT> void initContainer(ContainerT & container, uint32 dim);

template<>
void initContainer<std::vector<int32>>(std::vector<int32> & vec, uint32 dim)
{
	for (uint32 i = 0; i < dim; ++i)
		vec.push_back(rand());
}

/**
 * Korin quicksort
 */
template<typename ContainerT>
void korinQuicksort(benchmark::State & state)
{
	const uint32 dim = state.range(0);
	ContainerT arr;
	
	initContainer(arr, dim);

	for (auto _ : state)
	{
		state.PauseTiming();
		std::random_shuffle(arr.begin(), arr.end());
		state.ResumeTiming();

		Sort::quicksort(arr.begin(), arr.end(), [](int32 a, int32 b) { return a - b; });

		doNotOptimizeAway(&arr);
	}
}

/**
 * Stdlib dynamic array implementation
 */
template<typename ContainerT>
void stdQuicksort(benchmark::State & state)
{
	const uint32 dim = state.range(0);
	ContainerT arr;
	
	initContainer(arr, dim);

	for (auto _ : state)
	{
		state.PauseTiming();
		std::random_shuffle(arr.begin(), arr.end());
		state.ResumeTiming();

		std::sort(arr.begin(), arr.end());

		doNotOptimizeAway(&arr);
	}
}

BENCHMARK_TEMPLATE(korinQuicksort, std::vector<int32>)->Range(1u << 6u, 100000000);
BENCHMARK_TEMPLATE(stdQuicksort, std::vector<int32>)->Range(1u << 6u, 100000000);