#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "hal/platform_math.h"
#include "math_types.h"
#include "vec3.h"
#include "vec4.h"
#include "../containers/tuple.h"

/**
 * A 4 component vector that describes a rotation
 */
struct Quat : public Vec4<float32>
{
	/**
	 * Default constructor
	 */
	FORCE_INLINE Quat()
		: Vec4{0.f, 0.f, 0.f, 1.f}
	{
		//
	}

private:
	/// Privately inherit vec4 constructors
	using Vec4::Vec4;

public:
	/**
	 * Angle and axis constructor
	 * 
	 * @param [in] inAngle rotation angle
	 * @param [in] axis normalized rotation axis
	 */
	FORCE_INLINE Quat(float32 inAngle, Vec3<float32> inAxis)
	{
		inAngle *= 0.5f;
		inAxis *= PlatformMath::sin(inAngle);

		this->x = inAxis.x;
		this->y = inAxis.y;
		this->z = inAxis.z;
		this->w = PlatformMath::cos(inAngle);
	}

	/**
	 * Constructs from physics rotation vector
	 */
	FORCE_INLINE Quat(Vec3<float32> v)
	{
		float32 alpha = v.getSize();
		float32 a = alpha * 0.5f;

		v *= PlatformMath::sin(a) / alpha;

		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = PlatformMath::cos(a);
	}

	/**
	 * Extract angle and axis from quaternion
	 * 
	 * @param [out] outAngle rotation angle
	 * @param [out] outAxis rotation axis
	 */
	FORCE_INLINE void getAngleAndAxis(float32 & outAngle, Vec3<float32> & outAxis) const
	{
		float32 a = PlatformMath::acos(this->w);
		float32 s = 1.f / PlatformMath::sin(a);

		outAngle = a * 2.f;
		outAxis = Vec3<float32>(this->x * s, this->y * s, this->w * s);
	}

	/**
	 * Returns angle of rotation
	 */
	FORCE_INLINE float32 getAngle() const
	{
		return PlatformMath::acos(this->w) * 2.f;
	}

	/**
	 * Invert quaternion rotation
	 * 
	 * @return new quaternion
	 */
	FORCE_INLINE Quat operator-() const
	{
		return Quat{this->x, this->y, this->z, -this->w};
	}

	/**
	 * Quaternion hamilton product
	 * Produce a quaternion that describe
	 * the combined rotation of q1 and q2
	 * in inverse order (apply q2 then q1)
	 * 
	 * @param [in] other quaternion operand (q2)
	 * @return new quaternion
	 */
	FORCE_INLINE Quat operator*(const Quat & other) const
	{
		// @ref https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
		// Wikipedia uses (angle, axis<i, j, k>) notation
		// Here we use (axis<x, y, z>, angle) notation,
		// thus everything is inverted
		
		return Quat{
			this->w * other.x + this->x * other.w - this->y * other.z + this->z * other.y,
			this->w * other.y + this->x * other.z + this->y * other.w - this->z * other.x,
			this->w * other.z - this->x * other.y + this->y * other.x + this->z * other.w,
			this->w * other.w - this->x * other.x - this->y * other.y - this->z * other.z
		};
	}

	/**
	 * Multiply a vector by this quaternion
	 * Applies rotation to vector
	 * 
	 * @param [in] v vector operand
	 */
	FORCE_INLINE Vec3<float32> operator*(const Vec3<float32> & v) const
	{
		/** @see http://people.csail.mit.edu/bkph/articles/Quaternions.pdf */
		const Vec3<float32> q{this->x, this->y, this->z};
		const Vec3<float32> t = 2.f * (q ^ v);
		return v + (this->w * t) + (q ^ t);
	}

	/**
	 * Returns rotated axes
	 * 
	 * @return tuple with x, y, z axes
	 */
	FORCE_INLINE Tuple<Vec3<float32>, 3> getAxes() const
	{
		const vec3 x = *this * Vec3<float32>::right;
		const vec3 y = *this * Vec3<float32>::up;
		const vec3 z = x ^ y;

		return T(x, y, z);
	}

	/**
	 * Returns direction vector
	 * @{
	 */
	FORCE_INLINE Vec3<float32> getRight() const
	{
		return *this * Vec3<float32>::right;
	}

	FORCE_INLINE Vec3<float32> getLeft() const
	{
		return *this * Vec3<float32>::left;
	}

	FORCE_INLINE Vec3<float32> getUp() const
	{
		return *this * Vec3<float32>::up;
	}

	FORCE_INLINE Vec3<float32> getDown() const
	{
		return *this * Vec3<float32>::down;
	}

	FORCE_INLINE Vec3<float32> getForward() const
	{
		return *this * Vec3<float32>::forward;
	}

	FORCE_INLINE Vec3<float32> getBackward() const
	{
		return *this * Vec3<float32>::backward;
	}
	/// @}
};