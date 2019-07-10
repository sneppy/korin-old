#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "hal/platform_math.h"

/**
 * A vector with 3 components
 */
template<typename T>
struct Vec3
{
	union
	{
		struct
		{
			/// Coordinates components
			T x, y, z;
		};

		struct
		{
			/// Color components
			T r, g, b;
		};

		/// Array layout
		T array[3];
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
		return x * x + y * y + z * z;
	}

	/**
	 * Returns vector size
	 */
	FORCE_INLINE T getSize() const
	{
		CHECKF(false, "getSize() implemented only for floating-point types")
	}
	
	/**
	 * Returns inverted vector
	 * 
	 * @return new vector
	 */
	FORCE_INLINE Vec3 operator-() const
	{
		return Vec3{-x, -y, -z};
	}

	/**
	 * Compound vector-vector element-wise arithmetic operations
	 * 
	 * @param [in] other vector operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec3 & operator+=(const Vec3 & other)
	{
		x += other.x, y += other.y, z += other.z;
		return *this;
	}

	FORCE_INLINE Vec3 & operator-=(const Vec3 & other)
	{
		x -= other.x, y -= other.y, z -= other.z;
		return *this;
	}

	FORCE_INLINE Vec3 & operator*=(const Vec3 & other)
	{
		x *= other.x, y *= other.y, z *= other.z;
		return *this;
	}

	FORCE_INLINE Vec3 & operator/=(const Vec3 & other)
	{
		x /= other.x, y /= other.y, z /= other.z;
		return *this;
	}
	/// @}

	/**
	 * Compound vector-scalar arithmetic operations
	 * 
	 * @param [in] s scalar operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec3 & operator+=(T s)
	{
		x += s, y += s, z += s;
		return *this;
	}

	FORCE_INLINE Vec3 & operator-=(T s)
	{
		x -= s, y -= s, z -= s;
		return *this;
	}

	FORCE_INLINE Vec3 & operator*=(T s)
	{
		x *= s, y *= s, z *= s;
		return *this;
	}

	FORCE_INLINE Vec3 & operator/=(T s)
	{
		x /= s, y /= s, z /= s;
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
	FORCE_INLINE Vec3 operator+(const Vec3 & other) const
	{
		return Vec3{*this} += other;
	}

	FORCE_INLINE Vec3 operator-(const Vec3 & other) const
	{
		return Vec3{*this} -= other;
	}

	FORCE_INLINE Vec3 operator*(const Vec3 & other) const
	{
		return Vec3{*this} *= other;
	}

	FORCE_INLINE Vec3 operator/(const Vec3 & other) const
	{
		return Vec3{*this} /= other;
	}
	/// @}

	/**
	 * Vector-scalar arithmetic operations
	 * 
	 * @param [in] s scalar operand
	 * @return new vector
	 * @{
	 */
	FORCE_INLINE Vec3 operator+(T s) const
	{
		return Vec3{*this} += s;
	}

	FORCE_INLINE Vec3 operator-(T s) const
	{
		return Vec3{*this} -= s;
	}

	FORCE_INLINE Vec3 operator*(T s) const
	{
		return Vec3{*this} *= s;
	}

	FORCE_INLINE Vec3 operator/(T s) const
	{
		return Vec3{*this} /= s;
	}
	/// @}

	/**
	 * Returns dot product
	 * 
	 * @param [in] other vector operand
	 * @return dot product
	 */
	FORCE_INLINE T operator&(const Vec3 & other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	/**
	 * Returns cross product as a scalar value
	 * 
	 * @param [in] other vector operand
	 * @return y component of cross product
	 */
	FORCE_INLINE Vec3 operator^(const Vec3 & other) const
	{
		return Vec3{
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}

	/**
	 * Cast to subtype
	 * 
	 * @return new vector
	 */
	template<typename U>
	FORCE_INLINE operator Vec3<U>() const
	{
		return Vec3<U>{static_cast<U>(x), static_cast<U>(y), static_cast<U>(z)};
	}
};

//////////////////////////////////////////////////
// Floating-point specialization
//////////////////////////////////////////////////

template<>
FORCE_INLINE float32 Vec3<float32>::getSize() const
{
	return PlatformMath::sqrt(getSquaredSize());
}