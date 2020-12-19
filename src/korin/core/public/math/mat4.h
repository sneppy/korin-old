#pragma once

#include "core_types.h"
#include "templates/utility.h"
#include "hal/platform_memory.h"
#include "./vec4.h"
#include "./vec3.h"

/**
 * @brief A templated 4x4 matrix.
 * 
 * @param T element type
 */
template<typename T>
struct Mat4
{
	/// @brief Matrix static values
	/// @{
	static constexpr Mat4 zero = 0;
	static constexpr Mat4 ones = 1;
	static constexpr Mat4 eye = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
	/// @}

	union
	{
		/// Flatten data
		T vec[16];

		/// Matrix data
		T mat[4][4];
	};

	/**
	 * @brief Default constructor.
	 */
	constexpr FORCE_INLINE Mat4()
		: vec{}
	{
		//
	}

	/**
	 * @brief Scalar constructor.
	 */
	constexpr FORCE_INLINE Mat4(const T & s)
		: vec{
			s, s, s, s,
			s, s, s, s,
			s, s, s, s,
			s, s, s, s
		}
	{
		//
	}

	/**
	 * @brief Initialize matrix rows.
	 * 
	 * @param e0,... matrix values
	 */
	constexpr FORCE_INLINE Mat4(
		const T & s0, const T & s1, const T & s2, const T & s3,
		const T & s4, const T & s5, const T & s6, const T & s7,
		const T & s8, const T & s9, const T & s10, const T & s11,
		const T & s12, const T & s13, const T & s14, const T & s15
	)
		: vec{
			s0, s1, s2, s3,
			s4, s5, s6, s7,
			s8, s9, s10, s11,
			s12, s13, s14, s15
		}
	{
		//
	}

	/**
	 * @brief Create a matrix from a dynamic
	 * buffer.
	 * 
	 * @param src pointer to a non-null
	 * 	buffer, which contains exactly
	 * 	16 scalar values
	 */
	FORCE_INLINE Mat4(const T * src)
	{
		Memory::memcpy(vec, src, sizeof(vec));
	}

	/**
	 * @brief Returns pointer to idx-th row.
	 * 
	 * @param idx row index
	 * @return pointer to row
	 * @{
	 */
	FORCE_INLINE const T * operator[](uint32 idx) const
	{
		return mat[idx];
	}

	FORCE_INLINE T * operator[](uint32 idx)
	{
		return mat[idx];
	}
	/// @}

	/**
	 * @brief Returns a pointer to matrix
	 * flat data.
	 * 
	 * @return pointer to data
	 * @{
	 */
	FORCE_INLINE const T * operator*() const
	{
		return vec;
	}

	FORCE_INLINE T * operator*()
	{
		return vec;
	}
	/// @}

	/**
	 * @brief Compares the values of two
	 * matrices.
	 * 
	 * Returns a bit vector where each
	 * bit is 1 if the corresponding
	 * elements are equals, 0 otherwise.
	 * 
	 * @param other comparison operand
	 * @return bit vector
	 */
	uint16 cmpeq(const Mat4 & other) const
	{
		uint16 out = 0; for (uint32 i = 0; i < 16; ++i)
			out = (out << 1u) | (uint16)(vec[i] == other.vec[i]);
		
		return out;
	}

	/**
	 * @brief Returns true if all elements
	 * are equals.
	 * 
	 * @param other matrix to compare
	 * @return true if all elements are
	 * 	equals
	 * @{
	 */
	FORCE_INLINE bool operator==(const Mat4 & other) const
	{
		return cmp(other) == (uint16)0xffffu;
	}

	FORCE_INLINE bool operator!=(const Mat4 & other) const
	{
		return !operator==(other);
	}
	/// @}

	/**
	 * @brief Tranposes matrix in place and returns
	 * ref to self.
	 */
	FORCE_INLINE Mat4 & transpose()
	{
		swap(vec[1], vec[4]);
		swap(vec[2], vec[8]);
		swap(vec[3], vec[12]);
		swap(vec[6], vec[9]);
		swap(vec[7], vec[13]);
		swap(vec[11], vec[14]);

		return *this;
	}

	/**
	 * @brief Returns a copy of the matrix
	 * transposed.
	 */
	FORCE_INLINE Mat4 getTransposed() const
	{
		return Mat4{*this}.transpose();
	}

protected:
	/**
	 * @brief Returns matrix of algebraic 
	 * complements. 
	 */
	constexpr Mat4 getInverseComplements() const
	{
		const T * lower = vec + 0;
		const T * upper = vec + 8;

		const T afbe = lower[0] * lower[5] - lower[1] * lower[4];
		const T agce = lower[0] * lower[6] - lower[2] * lower[4];
		const T ahde = lower[0] * lower[7] - lower[3] * lower[4];
		const T bgcf = lower[1] * lower[6] - lower[2] * lower[5];
		const T bhdf = lower[1] * lower[7] - lower[3] * lower[5];
		const T chdg = lower[2] * lower[7] - lower[3] * lower[6];
		
		const T injm = upper[0] * upper[5] - upper[1] * upper[4];
		const T iokm = upper[0] * upper[6] - upper[2] * upper[4];
		const T iplm = upper[0] * upper[7] - upper[3] * upper[4];
		const T jokn = upper[1] * upper[6] - upper[2] * upper[5];
		const T jpln = upper[1] * upper[7] - upper[3] * upper[5];
		const T kplo = upper[2] * upper[7] - upper[3] * upper[6];

		return {
			+(lower[5] * kplo - lower[6] * jpln + lower[7] * jokn),
			-(lower[1] * kplo - lower[2] * jpln + lower[3] * jokn),
			+(upper[5] * chdg - upper[6] * bhdf + upper[7] * bgcf),
			-(upper[1] * chdg - upper[2] * bhdf + upper[3] * bgcf),

			-(lower[4] * kplo - lower[6] * iplm + lower[7] * iokm),
			+(lower[0] * kplo - lower[2] * iplm + lower[3] * iokm),
			-(upper[4] * chdg - upper[6] * ahde + upper[7] * agce),
			+(upper[0] * chdg - upper[2] * ahde + upper[3] * agce),

			+(lower[4] * jpln - lower[5] * iplm + lower[7] * injm),
			-(lower[0] * jpln - lower[1] * iplm + lower[3] * injm),
			+(upper[4] * bhdf - upper[5] * ahde + upper[7] * afbe),
			-(upper[0] * bhdf - upper[1] * ahde + upper[3] * afbe),

			-(lower[4] * jokn - lower[5] * iokm + lower[6] * injm),
			+(lower[0] * jokn - lower[1] * iokm + lower[2] * injm),
			-(upper[4] * bgcf - upper[5] * agce + upper[6] * afbe),
			+(upper[0] * bgcf - upper[1] * agce + upper[2] * afbe)
		};
	}

	/**
	 * @brief Returns matrix of algebraic
	 * complements.
	 */
	constexpr FORCE_INLINE Mat4 getComplements() const
	{
		return getInverseComplements().invert();
	}
	
public:
	/**
	 * @brief Invert matrix in place.
	 */
	FORCE_INLINE Mat4 & invert()
	{
		// Get inverse and copy values
		PlatformMemory::memcpy(vec, getInverse().vec, sizeof(vec));
		return *this;
	}

	/**
	 * @brief Returns the inverse of the
	 * matrix as a copy.
	 */
	constexpr Mat4 getInverse() const
	{
		// Get complements matrix and
		Mat4 invcomp = getInverseComplements();

		// compute determinant
		const T * row = invcomp[0];
		const T invdet = 1.f / (vec[0] * row[0] + vec[1] * row[1] + vec[2] * row[2] + vec[3] * row[3]);

		// Return transposed multiplied by inverse determinant
		return invcomp * invdet;
	}

	/**
	 * @brief Multiply by a scalar.
	 * 
	 * Element-wise multiplication by
	 * a scalar value.
	 * 
	 * @param s scalar value
	 * @return ref to self
	 */
	constexpr Mat4 & operator*=(const T & s)
	{
		for (uint8 idx = 0; idx < 16; ++idx)
		{
			vec[idx] *= s;
		}

		return *this;
	}

	/**
	 * @brief Returns matrix multiplied by
	 * a scalar value.
	 * 
	 * @param s scalar value
	 * @return new scaled matrix
	 */
	constexpr FORCE_INLINE Mat4 operator*(const T & s) const
	{
		return (Mat4{*this} *= s);
	}

	/**
	 * @brief Returns matrix multiplied by
	 * a scalar value.
	 * 
	 * @param s scalar value
	 * @param m matrix operand
	 * @return scaled matrix
	 */
	friend constexpr FORCE_INLINE Mat4 operator*(const T & s, const Mat4 & m)
	{
		return m * s;
	}

	/**
	 * @brief Broadcast element-wise multiplication
	 * of matrix columns with column vector.
	 * 
	 * Modifies this matrix.
	 * 
	 * @param v column vector
	 * @return ref to self
	 */
	Mat4 & operator*=(const Vec4<T> & v)
	{
		for (uint8 j = 0; j < 16; ++j)
		{
			// Broadcast onto rows
			vec[j] *= v[j / 4];
		}

		return *this;
	}

	/**
	 * @brief Broadcast element-wise multiplication
	 * of matrix rows with row vector.
	 * 
	 * Returns a new matrix.
	 * 
	 * @param v row vector
	 * @param m matrix operand
	 * @return new matrix
	 */
	Mat4 operator*(const Vec4<T> & v) const
	{
		return (Mat4{*this} *= v);
	}

	/**
	 * @brief Broadcast element-wise multiplication
	 * of matrix rows with row vector.
	 * 
	 * @param v row vector
	 * @param m matrix operand
	 * @return new matrix
	 */
	friend Mat4 operator*(const Vec4<T> & v, const Mat4 & m)
	{
		Mat4 n{m};
		
		for (uint8 i = 0; i < 16; ++i)
		{
			// Broadcast vector onto columns
			n.vec[i] *= v[i % 4];
		}

		return n;
	}

	/**
	 * @brief Element-wise matrix multiplication.
	 * 
	 * @param other other matrix
	 * @return ref to self
	 */
	Mat4 & operator*=(const Mat4 & other)
	{
		for (uint8 idx = 0; idx < 16; ++idx)
		{
			vec[idx] *= other.vec[idx];
		}

		return *this;
	}

	/**
	 * @brief Returns the result of element-wise
	 * matrix-matrix multiplication.
	 * 
	 * @param other other matrix
	 * @return new multiplied matrix
	 */
	FORCE_INLINE Mat4 operator*(const Mat4 & other)
	{
		return (Mat4{*this} *= other);
	}

	/**
	 * @brief Alias for element-wise matrix
	 * multiplication.
	 * 
	 * @param other other matrix
	 * @return ref to self
	 */
	FORCE_INLINE Mat4 & mul(const Mat4 & other)
	{
		return (*this *= other);
	}

private:
	/**
	 * @brief Multiply this matrix by a
	 * transpoed matrix.
	 * 
	 * @param other transposed matrix
	 * @return ref to self 
	 */
	Mat4 & multiplyTransposed(const Mat4 & other)
	{
		for (uint8 i = 0; i < 4; ++i)
		{
			T row[4] = {};

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
	 * @brief Multiply by another matrix
	 * and returns ref to self.
	 * 
	 * If both matrices represents a
	 * transformation, the resulting
	 * matrix represents a transformation
	 * as well, whose application is
	 * equivalent to the application
	 * of the two transformations in the
	 * inverse order:
	 * 
	 * ```
	 * M * N * v = N(M(v));
	 * ```
	 * 
	 * @param other other matrix
	 * @return ref to self
	 * @{
	 */
	FORCE_INLINE Mat4 & operator&=(const Mat4 & other)
	{
		return multiplyTransposed(other.getTransposed());
	}

	FORCE_INLINE Mat4 & operator&=(Mat4 && other)
	{
		// Avoid making a copy for trasnposed matrix
		return multiplyTransposed(other.transpose());
	}
	/// @}

	/**
	 * @brief Returns result of matrix
	 * multiplication
	 * 
	 * @param other other matrix
	 * @return resulting matrix
	 * @{
	 */
	FORCE_INLINE Mat4 operator&(const Mat4 & other) const
	{
		return (Mat4{*this} &= other);
	}

	FORCE_INLINE Mat4 operator&(Mat4 && other) const
	{
		return (Mat4{*this} &= move(other));
	}
	/// @}

	/**
	 * @brief Alias for matrix-matrix multiplication.
	 * 
	 * @param other other matrix
	 * @return ref to self
	 */
	Mat4 & dot(const Mat4 & other)
	{
		return (*this &= other);
	}

	/**
	 * @brief Multiply a vector by this
	 * matrix.
	 * 
	 * A 3D vector is first converted to
	 * a 4D vector with w = 1.f.
	 * 
	 * @param v vector operand
	 * @return multiplied vector
	 * @{
	 */
	Vec4<T> operator&(const Vec4<T> & v) const
	{
		Vec4<T> u = 0.f;

		for (uint8 i = 0; i < 4; ++i)
		{
			for (uint8 j = 0; j < 4; ++j)
			{
				u[i] += mat[i][j] * v[j];
			}
		}

		return u;
	}

	FORCE_INLINE Vec3<T> operator&(const Vec3<T> & v) const
	{
		return (*this) & Vec4<T>{v, 1.f};
	}
	/** @} */

	/**
	 * @brief Transform a vector using
	 * this matrix.
	 * 
	 * This operation is equivalent to
	 * matrix-vector multiplication.
	 * 
	 * @param v vector to transform
	 * @return transformed vector
	 */
	FORCE_INLINE Vec4<T> transformVector(const Vec4<T> & v) const
	{
		return (*this) & v;
	}

	FORCE_INLINE Vec3<T> transformVector(const Vec3<T> & v) const
	{
		return (*this) & v;
	}
	/** @} */
};