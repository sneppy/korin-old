#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "hal/platform_math.h"
#include "math_types.h"

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
	 * Zero initializes vector
	 */
	FORCE_INLINE Vec4()
		: array{}
	{
		//
	}

	/**
	 * Initializes vector components
	 * 
	 * @param [in] inX,inY,inZ vector components
	 */
	FORCE_INLINE Vec4(T inX, T inY, T inZ, T inW)
		: array{inX, inY, inZ, inW}
	{
		//
	}

	/**
	 * Scalar constructor
	 * 
	 * @param [in] s scalar value
	 */
	FORCE_INLINE Vec4(T s)
		: array{s, s, s, s}
	{
		//
	}

	/**
	 * Vec3 constructor
	 * 
	 * @param [in] other Vec3 vector
	 * @param [in] inW in w component
	 */
	FORCE_INLINE Vec4(const Vec3<T> & other, T inW)
		: array{other.x, other.y, other.z, inW}
	{
		//
	}

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

	friend FORCE_INLINE Vec4 operator+(T s, const Vec4 & v)
	{
		return v += s;
	}

	friend FORCE_INLINE Vec4 operator-(T s, const Vec4 & v)
	{
		return v -= s;
	}

	friend FORCE_INLINE Vec4 operator*(T s, const Vec4 & v)
	{
		return v *= s;
	}

	friend FORCE_INLINE Vec4 operator/(T s, const Vec4 & v)
	{
		return v /= s;
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
	 * Compare two vectors
	 * 
	 * @param [in] other vector operand
	 * @return comparison mask
	 * @{
	 */
	FORCE_INLINE int32 cmpeq(const Vec4 & other) const
	{
		return (x == other.x) | (y == other.y) << 1 | (z == other.z) << 2 | (w == other.w) << 3;
	}

	FORCE_INLINE int32 cmpne(const Vec4 & other) const
	{
		return (x != other.x) | (y != other.y) << 1 | (z != other.z) << 2 | (w != other.w) << 3;
	}

	FORCE_INLINE int32 cmplt(const Vec4 & other) const
	{
		return (x < other.x) | (y < other.y) << 1 | (z < other.z) << 2 | (w < other.w) << 3;
	}

	FORCE_INLINE int32 cmpgt(const Vec4 & other) const
	{
		return (x > other.x) | (y > other.y) << 1 | (z > other.z) << 2 | (w > other.w) << 3;
	}

	FORCE_INLINE int32 cmple(const Vec4 & other) const
	{
		return (x <= other.x) | (y <= other.y) << 1 | (z <= other.z) << 2 | (w <= other.w) << 3;
	}

	FORCE_INLINE int32 cmpge(const Vec4 & other) const
	{
		return (x >= other.x) | (y >= other.y) << 1 | (z >= other.z) << 2 | (w >= other.w) << 3;
	}
	/// @}

	/**
	 * Strict comparison operators
	 * 
	 * @param [in] other second operand
	 * @return true if true for all components
	 * @{
	 */
	FORCE_INLINE bool operator==(const Vec4 & other) const
	{
		return cmpeq(other) == 0xf;
	}

	FORCE_INLINE bool operator!=(const Vec4 & other) const
	{
		return cmpne(other) == 0xf;
	}

	FORCE_INLINE bool operator<(const Vec4 & other) const
	{
		return cmplt(other) == 0xf;
	}

	FORCE_INLINE bool operator>(const Vec4 & other) const
	{
		return cmpgt(other) == 0xf;
	}

	FORCE_INLINE bool operator<=(const Vec4 & other) const
	{
		return cmple(other) == 0xf;
	}

	FORCE_INLINE bool operator>=(const Vec4 & other) const
	{
		return cmpge(other) == 0xf;
	}
	/// @}

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