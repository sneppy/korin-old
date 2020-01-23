#pragma once

#include "../core_types.h"
#include "../templates/utility.h"
#include "../hal/platform_memory.h"

/**
 * A templated 4x4 matrix
 * 
 * @param T element type
 */
template<typename T>
struct Mat4
{
public:
	union
	{
		/// Flatten data
		T vec[16];

		/// Matrix data
		T mat[4][4];

		/// Individual elements
		struct { T a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p; };
	};

public:
	/**
	 * Default constructor
	 */
	constexpr FORCE_INLINE Mat4() = default;

	/**
	 * Scalar constructor
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
	 * List initializer.
	 * 
	 * @param values at least two values of type T
	 */
	template<typename ... Values>
	constexpr FORCE_INLINE Mat4(const T & a, const T & b, Values && ... c)
		: vec{a, b, forward<Values>(c) ...}
	{
		//
	}

	/**
	 * Buffer constructor
	 */
	FORCE_INLINE Mat4(const T * src)
	{
		Memory::memcpy(vec, src, sizeof(vec));
	}

	/**
	 * Indexing operator.
	 * Returns pointer to first element
	 * of i-th row.
	 * 
	 * @param i index
	 * @return pointer to i-th row
	 * @{
	 */
	FORCE_INLINE const T * operator[](uint32 i) const
	{
		return mat[i];
	}

	FORCE_INLINE T * operator[](uint32 i)
	{
		return mat[i];
	}
	/// @}

	/**
	 * Vector indexing operator.
	 * Returns i-th element, row-major order.
	 * 
	 * @param i index
	 * @return ref to i-th element
	 * @{
	 */
	FORCE_INLINE const T & operator()(uint32 i) const
	{
		return vec[i];
	}

	FORCE_INLINE T & operator()(uint32 i)
	{
		return vec[i];
	}
	/// @}

	/**
	 * Quick matrix indexing operator.
	 * Return (i, j)-th element.
	 * 
	 * @param i, j row and column indices
	 * @return ref to (i, j)-th element
	 * @{
	 */
	FORCE_INLINE const T & operator()(uint32 i, uint32 j) const
	{
		return mat[i][j];
	}

	FORCE_INLINE T & operator()(uint32 i, uint32 j)
	{
		return mat[i][j];
	}
	/// @}

	/**
	 * Returns a pointer to matrix flat
	 * data.
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
	 * Compare values, returns a bit
	 * vector where each bit is 1 if
	 * the corresponding elements are
	 * equals, 0 otherwise.
	 * 
	 * @param other comparison operand
	 * @return bit vector
	 */
	uint16 cmp(const Mat4 & other) const
	{
		uint16 out = 0; for (uint32 i = 0; i < 16; ++i)
			out = (out << 1u) | (uint16)(vec[i] == other.vec[i]);
		
		return out;
	}

	/**
	 * Returns true if all elements
	 * are equals.
	 * 
	 * @param other compariso operand
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
	 * Tranpose matrix in place.
	 * 
	 * @return ref to self
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
};