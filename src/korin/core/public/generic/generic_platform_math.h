#pragma once

#include "core_types.h"
#include "hal/platform_crt.h"
#include "templates/enable_if.h"
#include "templates/types.h"
#include "templates/utility.h"

/**
 * @brief Platform independent math utilities.
 */
struct GenericPlatformMath
{
	/// @brief Trigonometric functions.
	/// @{
	static constexpr FORCE_INLINE float32 sin(float32 v)	{ return ::sinf(v); }
	static constexpr FORCE_INLINE float32 cos(float32 v)	{ return ::cosf(v); }
	static constexpr FORCE_INLINE float32 tan(float32 v)	{ return ::tanf(v); }

	static constexpr FORCE_INLINE float32 asin(float32 v)	{ return ::asinf(v); }
	static constexpr FORCE_INLINE float32 acos(float32 v)	{ return ::acosf(v); }
	static constexpr FORCE_INLINE float32 atan(float32 v)	{ return ::atanf(v); }
	/// @}

	/// @brief Pow functions.
	/// @{
	static constexpr FORCE_INLINE float32 pow(float32 b, float32 e) { return ::powf(b, e); }

	static constexpr FORCE_INLINE float32 sqrt(float32 v) { return ::sqrtf(v); }
	/// @}

	/**
	 * @brief Integer power.
	 */
	template<typename T, typename U>
	static constexpr FORCE_INLINE typename EnableIf<IsIntegral<U>::value, T>::Type pow(const T & x, const U & y)
	{
		if (y == 0) return T{1};
		T z = pow(x, y >> 1);
		return y & 0x1 ? z * z * x : z * z;
	}
	/// @}

	/**
	 * @brief Returns the factorial of an
	 * integer number.
	 * 
	 * @param n input value
	 * @return n!
	 */
	static constexpr FORCE_INLINE uint64 fact(uint64 n)
	{
		uint64 x = 1;
		for (; n > 0; --n)
		{
			x *= n;
		}

		return x;
	}

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
	 * @param [in] a,b,c values to compare
	 * @return min[max] of a and b or list
	 * 	of values
	 * @{
	 */
	template<typename T, typename U>
	static constexpr FORCE_INLINE T min(const T & a, const U & b)
	{
		return a < b ? a : b;
	}

	template<typename T, typename U, typename ... Args>
	static constexpr FORCE_INLINE T min(const T & a, const U & b, Args && ... c)
	{
		return a < b ? min(a, forward<Args>(c)...) : min(b, forward<Args>(c)...);
	}

	template<typename T, typename U>
	static constexpr FORCE_INLINE T max(const T & a, const U & b)
	{
		return a > b ? a : b;
	}

	template<typename T, typename U, typename ... Args>
	static constexpr FORCE_INLINE T max(const T & a, const U & b, Args && ... c)
	{
		return a > b ? max(a, forward<Args>(c)...) : max(b, forward<Args>(c)...);
	}
	/// @}

	/**
	 * Perform logarithm in base 2.
	 * If argument is integer, returns
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

	/**
	 * @brief Compute parity of unsigned number,
	 * i.e. the number of 1s modulo 2.
	 * 
	 * @param n unsigned number
	 * @return parity of number
	 */
	template<typename T>
	static constexpr FORCE_INLINE T parity(const T & n);
};

template<>
FORCE_INLINE float32 GenericPlatformMath::abs(float32 v)
{
	return fabs(v);
}