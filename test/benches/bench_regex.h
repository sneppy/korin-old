#pragma once

#include "benchmark/benchmark.h"

#include "regex/regex.h"
#include <regex>

static constexpr const char * patterns[] = {
	/* 0 */"Hello, world!",
	/* 1 */"[abc]+",
	/* 2 */"https?://\\w+(\\.\\w+)+",
	/* 3 */"\\b([0-9]|[1-9][0-9]|1[0-9][0-9]|2[0-4][0-9]|25[0-5])\\b",
	/* 4 */"\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z][A-Za-z]+\\b",
	/* 5 */"\\b(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b"
};

static constexpr const char * tests[] = {
	/* 0 */"Hello, world!",
	/* 1 */"aabbabc",
	/* 2 */"http://sneppy.ddns.net",
	/* 3 */"209",
	/* 4 */"charlie.brown@peanuts.com",
	/* 5 */"127.167.178.11"
};

static constexpr sizet numTests = sizeof(tests) / sizeof(*tests);

/**
 * 
 */
void myRegex(benchmark::State & state)
{
	const char * pattern = patterns[state.range(0)];
	const char * test = tests[state.range(0)];

	for (auto _ : state)
	{
		Re::Regex regex{pattern};
		if (!regex.accept(test))
		{
			state.SkipWithError("Regex not accepted!");
		}
	}
}

/**
 * 
 */
void stdRegex(benchmark::State & state)
{
	const char * pattern = patterns[state.range(0)];
	const char * test = tests[state.range(0)];

	for (auto _ : state)
	{
		std::regex regex{pattern, std::regex_constants::ECMAScript};
		if (!std::regex_match(test, regex))
		{
			state.SkipWithError("Regex not accepted!");
		}
	}
}

BENCHMARK(myRegex)->DenseRange(0, numTests - 1);
BENCHMARK(stdRegex)->DenseRange(0, numTests - 1);
