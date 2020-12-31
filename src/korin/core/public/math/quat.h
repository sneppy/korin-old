#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "hal/platform_math.h"
#include "templates/array.h"
#include "math_types.h"
#include "vec3.h"
#include "vec4.h"

/**
 * A 4 component vector that describes a rotation
 */
struct Quat : public Vec4<float32>
{
	/**
	 * @brief Default constructor.
	 */
	constexpr FORCE_INLINE Quat()
		: Vec4{0.f, 0.f, 0.f, 1.f}
	{
		//
	}

	/**
	 * @brief Initialize quaternion components
	 * 
	 * @param inX,inY,inZ,inW quaternion components
	 */
	constexpr FORCE_INLINE Quat(float32 inX, float32 inY, float32 inZ, float32 inW)
		: Vec4{inX, inY, inZ, inW}
	{
		//
	}

	/**
	 * Angle and axis constructor
	 * 
	 * @param inAngle rotation angle
	 * @param axis normalized rotation axis
	 */
	constexpr FORCE_INLINE Quat(float32 inAngle, Vec3<float32> inAxis)
	{
		inAngle *= 0.5f;
		inAxis *= PlatformMath::sin(inAngle);

		x = inAxis.x;
		y = inAxis.y;
		z = inAxis.z;
		w = PlatformMath::cos(inAngle);
	}

	/**
	 * @brief Constructs from physics rotation vector.
	 * 
	 * @param inRotvec physics angular rotation vector
	 */
	constexpr FORCE_INLINE Quat(Vec3<float32> inRotvec)
	{
		float32 alpha = inRotvec.getSize();
		float32 a = alpha * 0.5f;

		inRotvec *= (PlatformMath::sin(a) / alpha);

		x = inRotvec.x;
		y = inRotvec.y;
		z = inRotvec.z;
		w = PlatformMath::cos(a);
	}

	/**
	 * Extract angle and axis from quaternion
	 * 
	 * @param [out] outAngle rotation angle
	 * @param [out] outAxis rotation axis
	 */
	constexpr FORCE_INLINE void getAngleAndAxis(float32 & outAngle, Vec3<float32> & outAxis) const
	{
		const float32 a = PlatformMath::acos(w);
		const float32 s = 1.f / PlatformMath::sin(a);

		outAngle = a * 2.f;
		outAxis = {x * s, y * s, w * s};
	}

	/**
	 * Returns angle of rotation
	 */
	constexpr FORCE_INLINE float32 getAngle() const
	{
		return PlatformMath::acos(w) * 2.f;
	}

	/**
	 * @brief Invert quaternion rotation.
	 */
	constexpr FORCE_INLINE Quat & invert()
	{
		return (w = -w, *this);
	}

	/**
	 * Invert quaternion rotation
	 * 
	 * @return new quaternion
	 */
	constexpr FORCE_INLINE Quat operator-() const
	{
		return Quat{*this}.invert();
	}

	/**
	 * @brief Normalizes quaternion, a rotation
	 * quaternion must be a unit quaternion. 
	 */
	constexpr FORCE_INLINE Quat & normalize()
	{
		return static_cast<Quat&>(static_cast<Vec4&>(*this).normalize());
	}

	/**
	 * @brief Returns a normalized copy of
	 * the quaternion. 
	 */
	constexpr FORCE_INLINE Quat getNormal() const
	{
		return Quat{*this}.normalize();
	}

	/**
	 * Quaternion hamilton product
	 * Produce a quaternion that describe
	 * the combined rotation of q1 and q2
	 * in inverse order (apply q2 then q1)
	 * 
	 * @param other quaternion operand (q2)
	 * @return new quaternion
	 */
	constexpr FORCE_INLINE Quat operator&(const Quat & other) const
	{
		// @ref https://en.wikipedia.org/wiki/Quaternion#Hamilton_product
		// Wikipedia uses (angle, axis<i, j, k>) notation
		// Here we use (axis<x, y, z>, angle) notation,
		// everything is inverted
		
		return Quat{
			w * other.x + x * other.w - y * other.z + z * other.y,
			w * other.y + x * other.z + y * other.w - z * other.x,
			w * other.z - x * other.y + y * other.x + z * other.w,
			w * other.w - x * other.x - y * other.y - z * other.z
		};
	}

	/**
	 * Multiply a vector by this quaternion
	 * Applies rotation to vector
	 * 
	 * @param [in] v vector operand
	 */
	constexpr FORCE_INLINE Vec3<float32> operator&(const Vec3<float32> & v) const
	{
		/** @see http://people.csail.mit.edu/bkph/articles/Quaternions.pdf */
		const Vec3<float32> q{x, y, z};
		const Vec3<float32> t = (q ^ v) * 2.f;
		return v + (w * t) + (q ^ t);
	}

	/**
	 * Returns rotated axes
	 * 
	 * @return tuple with x, y, z axes
	 */
	constexpr FORCE_INLINE StaticArray<Vec3<float32>, 3> getAxes() const
	{
		const Vec3<float32> x = *this * Vec3<float32>::right;
		const Vec3<float32> y = *this * Vec3<float32>::up;
		const Vec3<float32> z = x ^ y;

		return {x, y, z};
	}

	/**
	 * Returns direction vector
	 * @{
	 */
	constexpr FORCE_INLINE Vec3<float32> getRight() const
	{
		return *this & Vec3<float32>::right;
	}

	constexpr FORCE_INLINE Vec3<float32> getLeft() const
	{
		return *this & Vec3<float32>::left;
	}

	constexpr FORCE_INLINE Vec3<float32> getUp() const
	{
		return *this & Vec3<float32>::up;
	}

	constexpr FORCE_INLINE Vec3<float32> getDown() const
	{
		return *this & Vec3<float32>::down;
	}

	constexpr FORCE_INLINE Vec3<float32> getForward() const
	{
		return *this & Vec3<float32>::forward;
	}

	constexpr FORCE_INLINE Vec3<float32> getBackward() const
	{
		return *this & Vec3<float32>::backward;
	}
	/// @}
};