#pragma once

#include "core_types.h"

#ifndef DO_NOT_OPTIMIZE_AWAY_IMPL
#define DO_NOT_OPTIMIZE_AWAY_IMPL

	static FORCE_INLINE void doNotOptimizeAway(void * p)
	{
		asm volatile("" : : "g" (p) : "memory");
	}

#endif