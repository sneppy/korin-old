#pragma once

#include "benchmark/benchmark.h"

#include <core_types.h>
#include <containers/string.h>
#include <string>

#ifndef DO_NOT_OPTIMIZE_AWAY_IMPL
#define DO_NOT_OPTIMIZE_AWAY_IMPL

	static FORCE_INLINE void doNotOptimizeAway(void * p)
	{
		asm volatile("" : : "g" (p) : "memory");
	}

#endif

/**
 * My string implementation
 */
void sglString(benchmark::State & state)
{
	String a, b, c, d;

	for (auto _ : state)
	{
		a += "longer string. ";
		b += "name is sneppy, with an e. ";

		c = a + b;
		d = b + a;
	}

	doNotOptimizeAway(&a);
	doNotOptimizeAway(&b);
	doNotOptimizeAway(&c);
	doNotOptimizeAway(&d);
}

/**
 * C++ stdlib implementation
 */
void stdString(benchmark::State & state)
{
	std::string a, b, c, d;

	for (auto _ : state)
	{
		a += "longer string. ";
		b += "name is sneppy, with an e. ";

		c = a + b;
		d = b + a;
	}

	doNotOptimizeAway(&a);
	doNotOptimizeAway(&b);
	doNotOptimizeAway(&c);
	doNotOptimizeAway(&d);
}

BENCHMARK(sglString);
BENCHMARK(stdString);