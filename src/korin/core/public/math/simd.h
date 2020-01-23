#pragma once

#include "core_types.h"

/**
 * 
 */
template<typename T>
struct Vector4
{
	//
};

#if PLATFORM_USE_SIMD

/// Include intrinsics
#include <immintrin.h>

#include "simd_float.h"

#endif