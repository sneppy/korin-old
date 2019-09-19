#pragma once

#include "benchmark/benchmark.h"

#include <core_types.h>
#include <containers/array.h>
#include <vector>

#ifndef DO_NOT_OPTIMIZE_AWAY_IMPL
#define DO_NOT_OPTIMIZE_AWAY_IMPL

	static FORCE_INLINE void doNotOptimizeAway(void * p)
	{
		asm volatile("" : : "g" (p) : "memory");
	}

#endif

/**
 * SGL dynamic array implementation
 */
void sglArray(benchmark::State & state)
{
	constexpr uint32 dim = 512;

	for (auto _ : state)
	{
		Array<Array<uint32>> a{dim, 0};
		Array<Array<uint32>> b{};

		for (uint32 i = 0; i < dim; ++i)
		{
			a.add(Array<uint32>{dim, 0});
			b.add(Array<uint32>{});

			for (uint32 j = 0; j < dim; ++j)
			{
				a[i].add(i * dim + j);
				b[i].add(i * dim + j);
			}
		}

		doNotOptimizeAway(&a);
		doNotOptimizeAway(&b);
	}
}

/**
 * SGL dynamic array implementation
 */
void stdArray(benchmark::State & state)
{
	constexpr uint32 dim = 512;

	for (auto _ : state)
	{
		std::vector<std::vector<uint32>> a;
		std::vector<std::vector<uint32>> b;
		a.reserve(dim);

		for (uint32 i = 0; i < dim; ++i)
		{
			a.push_back(std::vector<uint32>{});
			b.push_back(std::vector<uint32>{});
			a[i].reserve(dim);

			for (uint32 j = 0; j < dim; ++j)
			{
				a[i].push_back(i * dim + j);
				b[i].push_back(i * dim + j);
			}
		}

		doNotOptimizeAway(&a);
		doNotOptimizeAway(&b);
	}
}

BENCHMARK(sglArray);
BENCHMARK(stdArray);