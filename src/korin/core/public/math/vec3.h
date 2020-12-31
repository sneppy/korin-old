#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "hal/platform_math.h"
#include "math_types.h"

/**
 * A vector with 3 components
 */
template<typename T>
struct Vec3
{
	// TODO: Make these constexpr
	/// Static values @{
	static constexpr Vec3 zero = 0;
	static constexpr Vec3 unit = 1;
	static constexpr Vec3 right = {1, 0, 0};
	static constexpr Vec3 left = {-1, 0, 0};
	static constexpr Vec3 up = {0, 1, 0};
	static constexpr Vec3 down = {0, -1, 0};
	static constexpr Vec3 forward = {0, 0, 1};
	static constexpr Vec3 backward = {0, 0, -1};
	/// @}

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
	 * Zero initialize vector
	 */
	constexpr FORCE_INLINE Vec3()
		: array{}
	{
		//
	}

	/**
	 * Initializes vector components
	 * 
	 * @param [in] inX,inY,inZ vector components
	 */
	constexpr FORCE_INLINE Vec3(T inX, T inY, T inZ)
		: array{inX, inY, inZ}
	{
		//
	}

	/**
	 * Scalar constructor
	 * 
	 * @param [in] s scalar value
	 */
	constexpr FORCE_INLINE Vec3(T s)
		: array{s, s, s}
	{
		//
	}

	/**
	 * Vec2 constructor
	 * 
	 * @param [in] other Vec2 vector
	 * @param [in] inZ z component
	 */
	constexpr FORCE_INLINE Vec3(const Vec2<T> & other, T inZ)
		: array{other.x, other.y, inZ}
	{
		//
	}

	/**
	 * Returns i-th component
	 * @{
	 */
	constexpr FORCE_INLINE const T & operator[](int32 i) const
	{
		return array[i];
	}

	constexpr FORCE_INLINE T & operator[](int32 i)
	{
		return array[i];
	}
	/// @}

	/**
	 * Returns vector squared length
	 */
	constexpr FORCE_INLINE T getSquaredSize() const
	{
		return x * x + y * y + z * z;
	}

	/**
	 * Returns vector size
	 */
	constexpr FORCE_INLINE T getSize() const
	{
		CHECKF(false, "getSize() implemented only for floating-point types")
	}

	/**
	 * Normalize vector in-place
	 * 
	 * @return self
	 */
	constexpr FORCE_INLINE Vec3 & normalize()
	{
		return *this /= getSize();
	}

	/**
	 * Returns normalized vector
	 * 
	 * @return new vector
	 */
	constexpr FORCE_INLINE Vec3 getNormal() const
	{
		return *this / getSize();
	}
	
	/**
	 * Returns inverted vector
	 * 
	 * @return new vector
	 */
	constexpr FORCE_INLINE Vec3 operator-() const
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
	constexpr FORCE_INLINE Vec3 & operator+=(const Vec3 & other)
	{
		x += other.x, y += other.y, z += other.z;
		return *this;
	}

	constexpr FORCE_INLINE Vec3 & operator-=(const Vec3 & other)
	{
		x -= other.x, y -= other.y, z -= other.z;
		return *this;
	}

	constexpr FORCE_INLINE Vec3 & operator*=(const Vec3 & other)
	{
		x *= other.x, y *= other.y, z *= other.z;
		return *this;
	}

	constexpr FORCE_INLINE Vec3 & operator/=(const Vec3 & other)
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
	constexpr FORCE_INLINE Vec3 & operator+=(T s)
	{
		x += s, y += s, z += s;
		return *this;
	}

	constexpr FORCE_INLINE Vec3 & operator-=(T s)
	{
		x -= s, y -= s, z -= s;
		return *this;
	}

	constexpr FORCE_INLINE Vec3 & operator*=(T s)
	{
		x *= s, y *= s, z *= s;
		return *this;
	}

	constexpr FORCE_INLINE Vec3 & operator/=(T s)
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
	constexpr FORCE_INLINE Vec3 operator+(const Vec3 & other) const
	{
		return Vec3{*this} += other;
	}

	constexpr FORCE_INLINE Vec3 operator-(const Vec3 & other) const
	{
		return Vec3{*this} -= other;
	}

	constexpr FORCE_INLINE Vec3 operator*(const Vec3 & other) const
	{
		return Vec3{*this} *= other;
	}

	constexpr FORCE_INLINE Vec3 operator/(const Vec3 & other) const
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
	constexpr FORCE_INLINE Vec3 operator+(T s) const
	{
		return Vec3{*this} += s;
	}

	constexpr FORCE_INLINE Vec3 operator-(T s) const
	{
		return Vec3{*this} -= s;
	}

	constexpr FORCE_INLINE Vec3 operator*(T s) const
	{
		return Vec3{*this} *= s;
	}

	constexpr FORCE_INLINE Vec3 operator/(T s) const
	{
		return Vec3{*this} /= s;
	}

	friend constexpr FORCE_INLINE Vec3 operator+(T s, const Vec3 & v)
	{
		return v + s;
	}

	friend constexpr FORCE_INLINE Vec3 operator-(T s, const Vec3 & v)
	{
		return v - s;
	}

	friend constexpr FORCE_INLINE Vec3 operator*(T s, const Vec3 & v)
	{
		return v * s;
	}

	friend constexpr FORCE_INLINE Vec3 operator/(T s, const Vec3 & v)
	{
		return v / s;
	}
	/// @}

	/**
	 * Returns dot product
	 * 
	 * @param [in] other vector operand
	 * @return dot product
	 */
	constexpr FORCE_INLINE T operator&(const Vec3 & other) const
	{
		return x * other.x + y * other.y + z * other.z;
	}

	/**
	 * Returns cross product as a scalar value
	 * 
	 * @param [in] other vector operand
	 * @return y component of cross product
	 */
	constexpr FORCE_INLINE Vec3 operator^(const Vec3 & other) const
	{
		return Vec3{
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
	}

	/**
	 * Returns true if vector's length is nearly zero
	 */
	FORCE_INLINE bool isNearlyZero() const
	{
		CHECKF(false, "isNearlyZero() implemented only for floating-point types")
		return false;
	}

	/**
	 * Compare two vectors
	 * 
	 * @param [in] other vector operand
	 * @return comparison mask
	 * @{
	 */
	constexpr FORCE_INLINE int32 cmpeq(const Vec3 & other) const
	{
		return (x == other.x) | (y == other.y) << 1 | (z == other.z) << 2;
	}

	constexpr FORCE_INLINE int32 cmpne(const Vec3 & other) const
	{
		return (x != other.x) | (y != other.y) << 1 | (z != other.z) << 2;
	}

	constexpr FORCE_INLINE int32 cmplt(const Vec3 & other) const
	{
		return (x < other.x) | (y < other.y) << 1 | (z < other.z) << 2;
	}

	constexpr FORCE_INLINE int32 cmpgt(const Vec3 & other) const
	{
		return (x > other.x) | (y > other.y) << 1 | (z > other.z) << 2;
	}

	constexpr FORCE_INLINE int32 cmple(const Vec3 & other) const
	{
		return (x <= other.x) | (y <= other.y) << 1 | (z <= other.z) << 2;
	}

	constexpr FORCE_INLINE int32 cmpge(const Vec3 & other) const
	{
		return (x >= other.x) | (y >= other.y) << 1 | (z >= other.z) << 2;
	}
	/// @}

	/**
	 * Strict comparison operators
	 * 
	 * @param [in] other second operand
	 * @return true if true for all components
	 * @{
	 */
	constexpr FORCE_INLINE bool operator==(const Vec3 & other) const
	{
		return cmpeq(other) == 0x7;
	}

	constexpr FORCE_INLINE bool operator!=(const Vec3 & other) const
	{
		return !(*this == other);
	}

	constexpr FORCE_INLINE bool operator<(const Vec3 & other) const
	{
		return cmplt(other) == 0x7;
	}

	constexpr FORCE_INLINE bool operator>(const Vec3 & other) const
	{
		return cmpgt(other) == 0x7;
	}

	constexpr FORCE_INLINE bool operator<=(const Vec3 & other) const
	{
		return cmple(other) == 0x7;
	}

	constexpr FORCE_INLINE bool operator>=(const Vec3 & other) const
	{
		return cmpge(other) == 0x7;
	}
	/// @}

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
constexpr FORCE_INLINE float32 Vec3<float32>::getSize() const
{
	return PlatformMath::sqrt(getSquaredSize());
}

template<>
constexpr FORCE_INLINE bool Vec3<float32>::isNearlyZero() const
{
	return getSquaredSize() < 4 * FLT_EPSILON;
}