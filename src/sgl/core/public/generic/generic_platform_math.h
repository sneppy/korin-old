#pragma once

#include "core_types.h"
#include "hal/platform_crt.h"
#include "templates/enable_if.h"
#include "templates/types.h"

/**
 * Platform independent math utilities
 */
struct GenericPlatformMath
{
	/// Trigonometric functions @{
	static FORCE_INLINE float32 sin(float32 v)	{ return ::sinf(v); }
	static FORCE_INLINE float32 cos(float32 v)	{ return ::cosf(v); }
	static FORCE_INLINE float32 tan(float32 v)	{ return ::tanf(v); }

	static FORCE_INLINE float32 asin(float32 v)	{ return ::asinf(v); }
	static FORCE_INLINE float32 acos(float32 v)	{ return ::acosf(v); }
	static FORCE_INLINE float32 atan(float32 v)	{ return ::atanf(v); }
	/// @}

	/// Pow functions @{
	static FORCE_INLINE float32 pow(float32 b, float32 e) { return ::powf(b, e); }

	static FORCE_INLINE float32 sqrt(float32 v) { return ::sqrtf(v); }
	/// @}

	/**
	 * Remove sign
	 * 
	 * @param [in] v math value
	 * @return value with stripped sign
	 */
	template<typename T>
	static constexpr FORCE_INLINE T abs(T v)
	{
		return v < T(0) ? -v : v;
	}

	/**
	 * Returns min[max] of two values
	 * 
	 * @param [in] a,b, values to compare
	 * @return min[max] of a and b
	 * @{
	 */
	template<typename A, typename B = A>
	static constexpr FORCE_INLINE A min(A a, B b)
	{
		return a < static_cast<A>(b) ? a : static_cast<A>(b);
	}

	template<typename A, typename B = A>
	static constexpr FORCE_INLINE A max(A a, B b)
	{
		return a > static_cast<A>(b) ? a : static_cast<A>(b);
	}
	/// @}

	/**
	 * Perform logarithm in base 2.
	 * If argument is integer, return
	 * floor(log2(x)).
	 * 
	 * @param n numeric value
	 * @{
	 */
	template<typename T>
	static constexpr FORCE_INLINE typename EnableIf<IsIntegral<T>::value && sizeof(T) < sizeof(uint64), T>::Type log2(T n)
	{
		return n == 0u ? 0u : 31u - __builtin_clz((uint32)n);
	}

	template<typename T>
	static constexpr FORCE_INLINE typename EnableIf<IsIntegral<T>::value && sizeof(T) == sizeof(uint64), T>::Type log2(T n)
	{
		return n == 0ull ? 0ull : 63ull - __builtin_clzll((uint64)n);
	}
	/// @}

	/**
	 * Align integer to the closest aligned
	 * value smaller or equal to it
	 * 
	 * ! Alignment must be a power of 2
	 * 
	 * @param [in] n integer
	 * @param [in] alignment required alignment
	 * @return aligned integer
	 */
	template<typename T>
	static constexpr FORCE_INLINE T align2Down(T n, sizet alignment)
	{
		static_assert(IsIntegral<T>::value, "T must be an integral type");

		return n & (alignment - 1) ^ n;
	}

	/**
	 * Align integer to the closest aligned
	 * value greater than it
	 * 
	 * ! Alignment must be a power of 2
	 * 
	 * @param [in] n integral value
	 * @param [in] alignment required alignment
	 */
	template<typename T>
	static constexpr FORCE_INLINE T align2Up(T n, sizet alignment)
	{
		static_assert(IsIntegral<T>::value, "T must be an integral type");

		return (n | (alignment - 1)) + 1;
	}
};

template<>
FORCE_INLINE float32 GenericPlatformMath::abs(float32 v)
{
	return fabs(v);
}