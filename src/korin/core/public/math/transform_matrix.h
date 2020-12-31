#pragma once

#include "core_types.h"
#include "./math_types.h"
#include "./vec3.h"
#include "./quat.h"
#include "./mat4.h"

/**
 * @brief A matrix that represents a
 * 3D transform (translation, rotation,
 * scale).
 */
struct TransformMatrix : public Mat4<float32>
{
	using Vec3T = Vec3<float32>;
	using Vec4T = Vec4<float32>;
	using Mat4::operator&=;
	using Mat4::operator&;

private:
	using Mat4::Mat4;

public:
	/**
	 * @brief Set zero transform (zero translation,
	 * zero rotation, unit scale)
	 */
	constexpr TransformMatrix()
		: Mat4{Mat4::eye}
	{
		//
	}

	/**
	 * @brief Initialize transform with given
	 * translation, rotation and scale.
	 * 
	 * @param inTranslation input translation
	 * 	vector
	 * @param inRotation input rotation quaternion
	 * @param inScale input scale vector
	 */
	constexpr explicit TransformMatrix(const Vec3T & inTranslation, const Quat & inRotation, const Vec3T inScale)
		: Mat4{
			inScale.x * (1.f - 2.f * (inRotation.y * inRotation.y + inRotation.z * inRotation.z)),
			inScale.y * 2.f * (inRotation.x * inRotation.y - inRotation.z * inRotation.w),
			inScale.z * 2.f * (inRotation.x * inRotation.z + inRotation.y * inRotation.w),
			inTranslation.x,

			inScale.x * 2.f * (inRotation.x * inRotation.y + inRotation.z * inRotation.w),
			inScale.y * (1.f - 2.f * (inRotation.x * inRotation.x + inRotation.z * inRotation.z)),
			inScale.z * 2.f * (inRotation.y * inRotation.z - inRotation.x * inRotation.w),
			inTranslation.y,

			inScale.x * 2.f * (inRotation.x * inRotation.z - inRotation.y * inRotation.w),
			inScale.z * 2.f * (inRotation.y * inRotation.z + inRotation.x * inRotation.w),
			inScale.z * (1.f - 2.f * (inRotation.x * inRotation.x + inRotation.y * inRotation.y)),
			inTranslation.z,
			
			0.f, 0.f, 0.f, 1.f
		}
	{
		//
	}

	/**
	 * @brief Initialize transform with given
	 * translation, rotation and unit scale.
	 * 
	 * @param inTranslation input translation
	 * 	vector
	 * @param inRotation input rotation quaternion
	 */
	constexpr explicit TransformMatrix(const Vec3T & inTranslation, const Quat & inRotation)
		: Mat4{
			(1.f - 2.f * (inRotation.y * inRotation.y + inRotation.z * inRotation.z)),
			2.f * (inRotation.x * inRotation.y - inRotation.z * inRotation.w),
			2.f * (inRotation.x * inRotation.z + inRotation.y * inRotation.w),
			inTranslation.x,

			2.f * (inRotation.x * inRotation.y + inRotation.z * inRotation.w),
			(1.f - 2.f * (inRotation.x * inRotation.x + inRotation.z * inRotation.z)),
			2.f * (inRotation.y * inRotation.z - inRotation.x * inRotation.w),
			inTranslation.y,

			2.f * (inRotation.x * inRotation.z - inRotation.y * inRotation.w),
			2.f * (inRotation.y * inRotation.z + inRotation.x * inRotation.w),
			(1.f - 2.f * (inRotation.x * inRotation.x + inRotation.y * inRotation.y)),
			inTranslation.z,
			
			0.f, 0.f, 0.f, 1.f
		}
	{
		//
	}

	/**
	 * @brief Initialize transform with given
	 * translation, identity rotation and unit
	 * scale.
	 * 
	 * @param inTranslation input translation
	 * 	vector
	 */
	constexpr explicit TransformMatrix(const Vec3T & inTranslation)
		: Mat4{
			1.0, 0.f, 0.f, inTranslation.x,
			0.f, 1.f, 0.f, inTranslation.y,
			0.f, 0.f, 1.f, inTranslation.z,
			0.f, 0.f, 0.f, 1.f
		}
	{
		//
	}

	/**
	 * @brief Returns the translation component
	 * of this transform.
	 */
	constexpr FORCE_INLINE Vec3T getTranslation() const
	{
		return {mat[0][3], mat[1][3], mat[2][3]};
	}

	/**
	 * @brief Returns the rotation component
	 * of this transform.
	 */
	constexpr Quat getRotation() const
	{
		// Get transform scale
		const Vec3T scale = getScale();

		// https://www.euclideanspace.com/maths/geometry/rotations/conversions/matrixToQuaternion/
		const float32 quatW = PlatformMath::sqrt(1.f + mat[0][0] / scale[0] + mat[1][1] / scale[1] + mat[2][2] / scale[2]) / 2.f;
		const float32 quatX = (mat[2][1] / scale[1] - mat[1][2] / scale[2]) / (4.f * quatW);
		const float32 quatY = (mat[0][2] / scale[2] - mat[2][0] / scale[0]) / (4.f * quatW);
		const float32 quatZ = (mat[1][0] / scale[0] - mat[0][1] / scale[1]) / (4.f * quatW);

		return {quatX, quatY, quatZ, quatW};
	}

	/**
	 * @brief Returns the scale component of
	 * this transform.
	 */
	constexpr Vec3T getScale() const
	{
		float32 scaleX = PlatformMath::sqrt(mat[0][0] * mat[0][0] + mat[1][0] * mat[1][0] + mat[2][0] * mat[2][0]);
		float32 scaleY = PlatformMath::sqrt(mat[0][1] * mat[0][1] + mat[1][1] * mat[1][1] + mat[2][1] * mat[2][1]);
		float32 scaleZ = PlatformMath::sqrt(mat[0][2] * mat[0][2] + mat[1][2] * mat[1][2] + mat[2][2] * mat[2][2]);

		return {scaleX, scaleY, scaleZ};
	}

	/**
	 * @brief Sets the translation component
	 * of this transform.
	 * 
	 * @param inTranslation new translation
	 * 	vector
	 */
	constexpr FORCE_INLINE TransformMatrix & setTranslation(const Vec3T & inTranslation)
	{
		mat[0][3] = inTranslation.x;
		mat[1][3] = inTranslation.y;
		mat[2][3] = inTranslation.z;

		return *this;
	}

	/**
	 * @brief Sets the rotation component of
	 * this transform.
	 * 
	 * @param inRotation new rotation
	 * 	quaternion
	 */
	constexpr TransformMatrix & setRotation(const Quat & inRotation)
	{
		const Vec3T scale = getScale();

		const float32 x2 = inRotation.x * inRotation.x;
		const float32 xy = inRotation.x * inRotation.y;
		const float32 xz = inRotation.x * inRotation.z;
		const float32 xw = inRotation.z * inRotation.w;
		const float32 y2 = inRotation.y * inRotation.y;
		const float32 yz = inRotation.y * inRotation.z;
		const float32 yw = inRotation.y * inRotation.w;
		const float32 z2 = inRotation.z * inRotation.z;
		const float32 zw = inRotation.z * inRotation.w;

		mat[0][0] = scale.x * (1.f - 2.f * (y2 + z2));
		mat[0][1] = scale.y * (2.f * (xy - zw));
		mat[0][2] = scale.z * (2.f * (xz + yw));

		mat[1][0] = scale.x * (2.f * (xy + zw));
		mat[1][1] = scale.y * (1.f - 2.f * (x2 + z2));
		mat[1][2] = scale.z * (2.f * (yz - xw));

		mat[2][0] = scale.x * (2.f * (xz - yw));
		mat[2][1] = scale.y * (2.f * (yz + xw));
		mat[2][2] = scale.z * (1.f - 2.f * (x2 + y2));

		return *this;
	}

	/**
	 * @brief Sets the scale component of this
	 * transform.
	 * 
	 * @param inScale new scale vector
	 */
	constexpr TransformMatrix & setScale(const Vec3T & inScale)
	{
		const Vec3<float32> factor = inScale / getScale();
		for (uint8 i = 0; i < 3; ++i)
		{
			mat[i][0] *= factor.x;
			mat[i][1] *= factor.y;
			mat[i][2] *= factor.z;
		}

		return *this;
	}

	/**
	 * @brief Specialized invert method for
	 * transform matrix.
	 */
	constexpr TransformMatrix & invert()
	{
		// Compute scale component, squared
		Vec3T scale = 1.f;
		scale.x = mat[0][0] * mat[0][0] + mat[1][0] * mat[1][0] + mat[2][0] * mat[2][0];
		scale.y = mat[0][1] * mat[0][1] + mat[1][1] * mat[1][1] + mat[2][1] * mat[2][1];
		scale.z = mat[0][2] * mat[0][2] + mat[1][2] * mat[1][2] + mat[2][2] * mat[2][2];
		
		if (scale != vec3::unit)
		{
			Vec3T invscale = 1.f / scale;

			mat[0][0] *= invscale.x;
			mat[0][1] *= invscale.x;
			mat[0][2] *= invscale.x;

			mat[1][0] *= invscale.y;
			mat[1][1] *= invscale.y;
			mat[1][2] *= invscale.y;

			mat[2][0] *= invscale.z;
			mat[2][1] *= invscale.z;
			mat[2][2] *= invscale.z;
		}
		
		// Transpose rotation 3x3 matrix
		swap(mat[0][1], mat[1][0]);
		swap(mat[0][2], mat[2][0]);
		swap(mat[1][2], mat[2][1]);

		// Compute inverted translation
		const Vec3<float32> translation = getTranslation();
		mat[0][3] = mat[0][0] * -translation.x + mat[0][1] * -translation.y + mat[0][2] * -translation.z;
		mat[1][3] = mat[1][0] * -translation.x + mat[1][1] * -translation.y + mat[1][2] * -translation.z;
		mat[2][3] = mat[2][0] * -translation.x + mat[2][1] * -translation.y + mat[2][2] * -translation.z;

		return *this;
	}

	/**
	 * @brief Returns inverted transform. 
	 */
	constexpr FORCE_INLINE TransformMatrix getInverse() const
	{
		return TransformMatrix{*this}.invert();
	}

	/**
	 * @brief Combines this with another
	 * transform and returns ref to self.
	 * 
	 * @param other other transform
	 * @return ref to self
	 * @{
	 */
	constexpr FORCE_INLINE TransformMatrix & operator&=(const TransformMatrix & other)
	{
		return static_cast<TransformMatrix&>(static_cast<Mat4&>(*this) &= other);
	}

	constexpr FORCE_INLINE TransformMatrix & operator&=(TransformMatrix && other)
	{
		return static_cast<TransformMatrix&>(static_cast<Mat4&>(*this) &= move(other));
	}
	/** @} */

	/**
	 * @brief Combines two transforms.
	 * 
	 * @param other other tranform
	 * @return resulting transform matrix
	 * @{
	 */
	constexpr FORCE_INLINE TransformMatrix operator&(const TransformMatrix & other) const
	{
		return (TransformMatrix{*this} &= other);
	}

	constexpr FORCE_INLINE TransformMatrix operator&(TransformMatrix && other)
	{
		return (TransformMatrix{*this} &= move(other));
	}
	/** @} */
};

/**
 * @brief A transform matrix with unit scale.
 */
struct TranslationAndRotationMatrix : public TransformMatrix
{
	using Vec3T = Vec3<float32>;
	using Vec4T = Vec4<float32>;
	using TransformMatrix::operator&=;
	using TransformMatrix::operator&;

	/**
	 * @brief Construct with zero translation and
	 * identity rotation.
	 */
	constexpr FORCE_INLINE TranslationAndRotationMatrix()
		: TransformMatrix{}
	{
		//
	}

	/**
	 * @brief Construct with given translation
	 * and rotation.
	 * 
	 * @param inTranslation translation vector
	 * @param inRotation rotation quaternion
	 */
	constexpr FORCE_INLINE explicit TranslationAndRotationMatrix(const Vec3T & inTranslation, const Quat & inRotation)
		: TransformMatrix{inTranslation, inRotation}
	{
		//
	}

	/**
	 * @brief Construct with given translation
	 * and identity rotation.
	 * 
	 * @param inTranslation translation vector
	 */
	constexpr FORCE_INLINE explicit TranslationAndRotationMatrix(const Vec3T & inTranslation)
		: TransformMatrix{inTranslation}
	{
		//
	}

	/**
	 * @brief Sets the rotation component of
	 * this transform.
	 * 
	 * @param inRotation new rotation
	 * 	quaternion
	 */
	constexpr TranslationAndRotationMatrix & setRotation(const Quat & inRotation)
	{
		const float32 x2 = inRotation.x * inRotation.x;
		const float32 xy = inRotation.x * inRotation.y;
		const float32 xz = inRotation.x * inRotation.z;
		const float32 xw = inRotation.z * inRotation.w;
		const float32 y2 = inRotation.y * inRotation.y;
		const float32 yz = inRotation.y * inRotation.z;
		const float32 yw = inRotation.y * inRotation.w;
		const float32 z2 = inRotation.z * inRotation.z;
		const float32 zw = inRotation.z * inRotation.w;

		mat[0][0] = (1.f - 2.f * (y2 + z2));
		mat[0][1] = (2.f * (xy - zw));
		mat[0][2] = (2.f * (xz + yw));

		mat[1][0] = (2.f * (xy + zw));
		mat[1][1] = (1.f - 2.f * (x2 + z2));
		mat[1][2] = (2.f * (yz - xw));

		mat[2][0] = (2.f * (xz - yw));
		mat[2][1] = (2.f * (yz + xw));
		mat[2][2] = (1.f - 2.f * (x2 + y2));

		return *this;
	}

	/**
	 * @brief Inverts the translation and rotation
	 * matrix and returns ref to self.
	 */
	constexpr TranslationAndRotationMatrix & invert()
	{		
		// Transpose rotation 3x3 matrix
		swap(mat[0][1], mat[1][0]);
		swap(mat[0][2], mat[2][0]);
		swap(mat[1][2], mat[2][1]);

		// Compute inverted translation
		const Vec3<float32> translation = getTranslation();
		mat[0][3] = mat[0][0] * -translation.x + mat[0][1] * -translation.y + mat[0][2] * -translation.z;
		mat[1][3] = mat[1][0] * -translation.x + mat[1][1] * -translation.y + mat[1][2] * -translation.z;
		mat[2][3] = mat[2][0] * -translation.x + mat[2][1] * -translation.y + mat[2][2] * -translation.z;

		return *this;
	}

	/**
	 * @brief Returns inverted transform. 
	 */
	constexpr FORCE_INLINE TranslationAndRotationMatrix getInverse() const
	{
		return TranslationAndRotationMatrix{*this}.invert();
	}

	/**
	 * @brief Combines this with another
	 * transform and returns ref to self.
	 * 
	 * @param other other transform
	 * @return ref to self
	 * @{
	 */
	constexpr FORCE_INLINE TranslationAndRotationMatrix & operator&=(const TranslationAndRotationMatrix & other)
	{
		return static_cast<TranslationAndRotationMatrix&>(static_cast<Mat4&>(*this) &= other);
	}

	constexpr FORCE_INLINE TranslationAndRotationMatrix & operator&=(TranslationAndRotationMatrix && other)
	{
		return static_cast<TranslationAndRotationMatrix&>(static_cast<Mat4&>(*this) &= move(other));
	}
	/** @} */

	/**
	 * @brief Combines two transforms.
	 * 
	 * @param other other tranform
	 * @return resulting transform matrix
	 * @{
	 */
	constexpr FORCE_INLINE TranslationAndRotationMatrix operator&(const TranslationAndRotationMatrix & other) const
	{
		return (TranslationAndRotationMatrix{*this} &= other);
	}

	constexpr FORCE_INLINE TranslationAndRotationMatrix operator&(TranslationAndRotationMatrix && other)
	{
		return (TranslationAndRotationMatrix{*this} &= move(other));
	}
	/** @} */
};

/**
 * @brief A transform matrix with zero translation
 * and unit scale. 
 */
struct RotationMatrix : public TranslationAndRotationMatrix
{
	using TranslationAndRotationMatrix::operator&=;
	using TranslationAndRotationMatrix::operator&;

	/**
	 * @brief Construct a rotation matrix with
	 * the identity rotation.
	 */
	constexpr FORCE_INLINE RotationMatrix()
		: TranslationAndRotationMatrix{}
	{
		//
	}

	/**
	 * @brief Construct a new rotation matrix
	 * with the give rotation.
	 * 
	 * @param inRotation rotation quaternion
	 */
	constexpr FORCE_INLINE RotationMatrix(const Quat & inRotation)
		: TranslationAndRotationMatrix{0.f, inRotation}
	{
		//
	}

	/**
	 * @brief 
	 * 
	 */
	constexpr FORCE_INLINE RotationMatrix & transpose()
	{
		swap(vec[1], vec[4]);
		swap(vec[2], vec[8]);
		swap(vec[6], vec[9]);

		return *this;
	}

	/**
	 * @brief Returns a copy of the matrix
	 * transposed.
	 */
	constexpr FORCE_INLINE RotationMatrix getTransposed() const
	{
		return RotationMatrix{*this}.transpose();
	}

	/**
	 * @brief Inverts the translation and rotation
	 * matrix and return ref to self.
	 */
	constexpr RotationMatrix & invert()
	{
		return transpose();
	}

	/**
	 * @brief Returns inverted transform. 
	 */
	constexpr FORCE_INLINE RotationMatrix getInverse() const
	{
		return RotationMatrix{*this}.invert();
	}

private:
	/**
	 * @brief Multiply this matrix by a
	 * transposed matrix.
	 * 
	 * @param other tranposed rotation matrix
	 * @return ref to self
	 */
	constexpr RotationMatrix & multiplyTransposed(const RotationMatrix & other)
	{
		for (uint8 i = 0; i < 3; ++i)
		{
			float32 row[4] = {};

			for (uint8 j = 0; j < 16; ++j)
			{
				row[j / 4] += mat[i][j % 4] * other.vec[j];
			}

			PlatformMemory::memcpy(mat[i], row, sizeof(mat[i]));
		}

		return *this;
	}

public:
	/**
	 * @brief Combine with another rotation
	 * matrix.
	 * 
	 * @param other other rotation matrix
	 * @return ref to self
	 * @{
	 */
	constexpr FORCE_INLINE RotationMatrix & operator&=(const RotationMatrix & other)
	{
		return multiplyTransposed(other.getTransposed());
	}

	constexpr FORCE_INLINE RotationMatrix & operator&=(RotationMatrix && other)
	{
		return multiplyTransposed(other.transpose());
	}
	/** @} */

	/**
	 * @brief Returns combined rotations.
	 * 
	 * @param other other rotation matrix
	 * @return resulting rotation matrix
	 * @{
	 */
	constexpr FORCE_INLINE RotationMatrix operator&(const RotationMatrix & other) const
	{
		return (RotationMatrix{*this} &= other);
	}

	constexpr FORCE_INLINE RotationMatrix operator&(RotationMatrix && other)
	{
		return (RotationMatrix{*this} &= move(other));
	}
	/** @} */
};
