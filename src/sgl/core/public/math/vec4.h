#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "hal/platform_math.h"
#include "math_fwd.h"

/**
 * A templated vector with 4 components
 */
template<typename T>
struct Vec4
{
	union
	{
		/// Array layout
		T array[4];

		struct
		{
			/// Position coordinates @{
			T x, y, z, w;
			/// @}
		};
	};

	/**
	 * Returns i-th component
	 * @{
	 */
	FORCE_INLINE const T & operator[](int32 i) const
	{
		return array[i];
	}

	FORCE_INLINE T & operator[](int32 i)
	{
		return array[i];
	}
	/// @}

	/**
	 * Returns vector squared length
	 */
	FORCE_INLINE T getSquaredSize() const
	{
		return x * x + y * y + z * z + w * w;
	}

	/**
	 * Returns vector size
	 */
	FORCE_INLINE T getSize() const
	{
		CHECKF(false, "getSize() implemented only for floating-point types")
	}

	/**
	 * Normalize vector in-place
	 * 
	 * @return self
	 */
	FORCE_INLINE Vec4 & normalize()
	{
		return *this /= getSize();
	}

	/**
	 * Returns normalized vector
	 * 
	 * @return new vector
	 */
	FORCE_INLINE Vec4 getNormal() const
	{
		return *this / getSize();
	}

	/**
	 * Returns inverted vector
	 * 
	 * @return new vector
	 */
	FORCE_INLINE Vec4 operator-() const
	{
		return Vec4{-x, -y, -z, -w};
	}

	/**
	 * Compound vector-vector element-wise arithmetic operations
	 * 
	 * @param [in] other vector operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec4 & operator+=(const Vec4 & other)
	{
		x += other.x, y += other.y, z += other.z, w += other.w;
		return *this;
	}

	FORCE_INLINE Vec4 & operator-=(const Vec4 & other)
	{
		x -= other.x, y -= other.y, z -= other.z, w -= other.w;
		return *this;
	}

	FORCE_INLINE Vec4 & operator*=(const Vec4 & other)
	{
		x *= other.x, y *= other.y, z *= other.z, w *= other.w;
		return *this;
	}

	FORCE_INLINE Vec4 & operator/=(const Vec4 & other)
	{
		x /= other.x, y /= other.y, z /= other.z, w /= other.w;
		return *this;
	}
	/// @}

	/**
	 * Compound vector-vector element-wise arithmetic operations
	 * 
	 * @param [in] other vector operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec4 & operator+=(T s)
	{
		x += s, y += s, z += s, w += s;
		return *this;
	}

	FORCE_INLINE Vec4 & operator-=(T s)
	{
		x -= s, y -= s, z -= s, w -= s;
		return *this;
	}

	FORCE_INLINE Vec4 & operator*=(T s)
	{
		x *= s, y *= s, z *= s, w *= s;
		return *this;
	}

	FORCE_INLINE Vec4 & operator/=(T s)
	{
		x /= s, y /= s, z /= s, w /= s;
		return *this;
	}
	/// @}

	/**
	 * Vector-vector element-wise arithmetic operations
	 * 
	 * @param [in] other vector operand
	 * @return new vector
	 * @{
	 */
	FORCE_INLINE Vec4 operator+(const Vec4 & other) const
	{
		return Vec4{*this} += other;
	}

	FORCE_INLINE Vec4 operator-(const Vec4 & other) const
	{
		return Vec4{*this} -= other;
	}

	FORCE_INLINE Vec4 operator*(const Vec4 & other) const
	{
		return Vec4{*this} *= other;
	}

	FORCE_INLINE Vec4 operator/(const Vec4 & other) const
	{
		return Vec4{*this} /= other;
	}
	/// @}

	/**
	 * Vector-scalar arithmetic operations
	 * 
	 * @param [in] s scalar operand
	 * @return new vector
	 * @{
	 */
	FORCE_INLINE Vec4 operator+(T s) const
	{
		return Vec4{*this} += s;
	}

	FORCE_INLINE Vec4 operator-(T s) const
	{
		return Vec4{*this} -= s;
	}

	FORCE_INLINE Vec4 operator*(T s) const
	{
		return Vec4{*this} *= s;
	}

	FORCE_INLINE Vec4 operator/(T s) const
	{
		return Vec4{*this} /= s;
	}
	/// @}

	/**
	 * Returns dot product
	 * 
	 * @param [in] other vector operand
	 * @return dot product
	 */
	FORCE_INLINE T operator&(const Vec4 & other) const
	{
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	/**
	 * Cast to subtype
	 * 
	 * @return new vector
	 */
	template<typename U>
	FORCE_INLINE operator Vec4<U>() const
	{
		return Vec4<U>{static_cast<U>(x), static_cast<U>(y), static_cast<U>(z), static_cast<U>(w)};
	}
};

//////////////////////////////////////////////////
// Floating-point specialization
//////////////////////////////////////////////////

template<>
FORCE_INLINE float32 Vec4<float32>::getSize() const
{
	return PlatformMath::sqrt(getSquaredSize());
}