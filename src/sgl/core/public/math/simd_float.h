#pragma once

#include "core_types.h"
#include "simd.h"

/**
 * 
 */
template<>
struct Vector4<float32>
{
	/// Element type
	using T = float32;

	/// Vector type
	using Type = __m128;

	/**
	 * Aligned load
	 * 
	 * @param [in] ptr pointer to aligned memory
	 * @param [in] a,b,c,d vector components
	 * @param [in] s scalar value
	 * @return new vector
	 * @{
	 */
	static FORCE_INLINE Type load(const T * ptr)
	{
		return _mm_load_ps(ptr);
	}

	static FORCE_INLINE Type load(T a, T b, T c, T d)
	{
		return _mm_set_ps(a, b, c, d);
	}

	static FORCE_INLINE Type load(T s)
	{
		return _mm_set1_ps(s);
	}
	/// @}

	/**
	 * Unaligned load
	 * 
	 * @param [in] ptr pointer to source data
	 * @return new vector
	 */
	static FORCE_INLINE Type loadu(const T * ptr)
	{
		return _mm_loadu_ps(ptr);
	}

	/**
	 * Aligned store
	 * 
	 * @param [in] ptr pointer to aligned destination
	 * @param [in] v source operand
	 */
	template<uint8 mask = 0xf>
	static FORCE_INLINE void store(T * ptr, Type v)
	{
		_mm_mask_store_ps(ptr, mask, v);
	}

	/**
	 * Unaligned store
	 * 
	 * @param [in] ptr pointer to unaliged dest
	 * @param [in] v source operand
	 */
	static FORCE_INLINE void store(T * ptr, Type v)
	{
		_mm_storeu_ps(ptr, v);
	}

	/**
	 * Element-wise arithmetic operations
	 * 
	 * @param [in] v1,v2 vector operands
	 * @return new vector
	 * @{
	 */
	static FORCE_INLINE Type add(Type v1, Type v2)
	{
		return _mm_add_ps(v1, v2);
	}

	static FORCE_INLINE Type sub(Type v1, Type v2)
	{
		return _mm_sub_ps(v1, v2);
	}

	static FORCE_INLINE Type mul(Type v1, Type v2)
	{
		return _mm_mul_ps(v1, v2);
	}

	static FORCE_INLINE Type div(Type v1, Type v2)
	{
		return _mm_div_ps(v1, v2);
	}
	/// @}

};