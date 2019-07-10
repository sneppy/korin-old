#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "hal/platform_math.h"
#include "math_types.h"

/**
 * Templated 2D vector
 */
template<typename T>
struct alignas(2 * sizeof(T)) Vec2
{
	union
	{
		struct
		{
			/// Position components @{
			T x, y;
			/// @}
		};

		struct
		{
			/// Texture components @{
			T u, v;
			/// @}
		};

		/// Array layout
		T array[2];		
	};

	/**
	 * Zero initialize vector
	 */
	FORCE_INLINE Vec2()
		: array{}
	{
		//
	}

	/**
	 * Initializes vector components
	 * 
	 * @param [in] inX,inY,inZ vector components
	 */
	FORCE_INLINE Vec2(T inX, T inY)
		: array{inX, inY}
	{
		//
	}

	/**
	 * Scalar constructor
	 * 
	 * @param [in] s scalar value
	 */
	FORCE_INLINE Vec2(T s)
		: array{s, s}
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
	 * Returns vector squared size
	 */
	FORCE_INLINE T getSquaredSize() const
	{
		return x * x + y * y;
	}

	/**
	 * Returns vector size (only floating-point)
	 */
	FORCE_INLINE T getSize() const
	{
		CHECKF(false, "getSize() is implemented only for floating-point types")
	}

	/**
	 * Normalize vector in-place
	 * 
	 * @return self
	 */
	FORCE_INLINE Vec2 & normalize()
	{
		return *this /= getSize();
	}

	/**
	 * Returns normalized vector
	 * 
	 * @return new vector
	 */
	FORCE_INLINE Vec2 getNormal() const
	{
		return *this / getSize();
	}

	/**
	 * Returns inverted vector
	 */
	FORCE_INLINE Vec2 operator-() const
	{
		return Vec2{-x, -y};
	}

	/**
	 * Compound vector-vector element-wise arithmetic operations
	 * 
	 * @param [in] other vector operand
	 * @return self
	 * @{
	 */
	FORCE_INLINE Vec2 & operator+=(Vec2 other)
	{
		x += other.x, y += other.y;
		return *this;
	}

	FORCE_INLINE Vec2 & operator-=(Vec2 other)
	{
		x -= other.x, y -= other.y;
		return *this;
	}

	FORCE_INLINE Vec2 & operator*=(Vec2 other)
	{
		x *= other.x, y *= other.y;
		return *this;
	}

	FORCE_INLINE Vec2 & operator/=(Vec2 other)
	{
		x /= other.x, y /= other.y;
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
	FORCE_INLINE Vec2 & operator+=(T s)
	{
		x += s, y += s;
		return *this;
	}

	FORCE_INLINE Vec2 & operator-=(T s)
	{
		x -= s, y -= s;
		return *this;
	}

	FORCE_INLINE Vec2 & operator*=(T s)
	{
		x *= s, y *= s;
		return *this;
	}

	FORCE_INLINE Vec2 & operator/=(T s)
	{
		x /= s, y /= s;
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
	FORCE_INLINE Vec2 operator+(Vec2 other) const
	{
		return Vec2{*this} += other;
	}

	FORCE_INLINE Vec2 operator-(Vec2 other) const
	{
		return Vec2{*this} -= other;
	}

	FORCE_INLINE Vec2 operator*(Vec2 other) const
	{
		return Vec2{*this} *= other;
	}

	FORCE_INLINE Vec2 operator/(Vec2 other) const
	{
		return Vec2{*this} /= other;
	}
	/// @}

	/**
	 * Vector-scalar arithmetic operations
	 * 
	 * @param [in] s scalar operand
	 * @return new vector
	 * @{
	 */
	FORCE_INLINE Vec2 operator+(T s) const
	{
		return Vec2{*this} += s;
	}

	FORCE_INLINE Vec2 operator-(T s) const
	{
		return Vec2{*this} -= s;
	}

	FORCE_INLINE Vec2 operator*(T s) const
	{
		return Vec2{*this} *= s;
	}

	FORCE_INLINE Vec2 operator/(T s) const
	{
		return Vec2{*this} /= s;
	}

	friend FORCE_INLINE Vec2 operator+(T s, Vec2 v)
	{
		return v += s;
	}

	friend FORCE_INLINE Vec2 operator-(T s, Vec2 v)
	{
		return v -= s;
	}

	friend FORCE_INLINE Vec2 operator*(T s, Vec2 v)
	{
		return v *= s;
	}

	friend FORCE_INLINE Vec2 operator/(T s, Vec2 v)
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
	FORCE_INLINE T operator&(Vec2 other) const
	{
		return x * other.x + y * other.y;
	}

	/**
	 * Returns cross product as a 3D vector
	 * 
	 * @param [in] other vector operand
	 * @return scaled z-axis vector
	 */
	FORCE_INLINE Vec3<T> operator^(Vec2 other) const
	{
		return Vec3<T>{0.f, 0.f, x * other.y - y * other.x};
	}

	/**
	 * Compare two vectors
	 * 
	 * @param [in] other vector operand
	 * @return comparison mask
	 * @{
	 */
	FORCE_INLINE int32 cmpeq(Vec2 other) const
	{
		return (x == other.x) | (y == other.y) << 1;
	}

	FORCE_INLINE int32 cmpne(Vec2 other) const
	{
		return (x != other.x) | (y != other.y) << 1;
	}

	FORCE_INLINE int32 cmplt(Vec2 other) const
	{
		return (x < other.x) | (y < other.y) << 1;
	}

	FORCE_INLINE int32 cmpgt(Vec2 other) const
	{
		return (x > other.x) | (y > other.y) << 1;
	}

	FORCE_INLINE int32 cmple(Vec2 other) const
	{
		return (x <= other.x) | (y <= other.y) << 1;
	}

	FORCE_INLINE int32 cmpge(Vec2 other) const
	{
		return (x >= other.x) | (y >= other.y) << 1;
	}
	/// @}

	/**
	 * Strict comparison operators
	 * 
	 * @param [in] other second operand
	 * @return true if true for all components
	 * @{
	 */
	FORCE_INLINE bool operator==(Vec2 other) const
	{
		return cmpeq(other) == 0x3;
	}

	FORCE_INLINE bool operator!=(Vec2 other) const
	{
		return cmpne(other) == 0x3;
	}

	FORCE_INLINE bool operator<(Vec2 other) const
	{
		return cmplt(other) == 0x3;
	}

	FORCE_INLINE bool operator>(Vec2 other) const
	{
		return cmpgt(other) == 0x3;
	}

	FORCE_INLINE bool operator<=(Vec2 other) const
	{
		return cmple(other) == 0x3;
	}

	FORCE_INLINE bool operator>=(Vec2 other) const
	{
		return cmpge(other) == 0x3;
	}
	/// @}

	/**
	 * Cast to subtype
	 * 
	 * @return new vector
	 */
	template<typename U>
	FORCE_INLINE operator Vec2<U>() const
	{
		return Vec2<U>{static_cast<U>(x), static_cast<U>(y)};
	}
};

//////////////////////////////////////////////////
// Floating-point specialization
//////////////////////////////////////////////////

template<>
FORCE_INLINE float32 Vec2<float32>::getSize() const
{
	return PlatformMath::sqrt(getSquaredSize());
}