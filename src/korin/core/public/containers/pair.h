#pragma once

#include "core_types.h"
#include "templates/utility.h"

/**
 * A struct with two elements (conceptually
 * key and value or simply first and second).
 * 
 * @param A key type
 * @param B value type
 * @param CompareT compare class, overloads
 * 	operator()(const A&, const B&)
 */
template<typename A, typename B, typename CompareT>
struct Pair
{
	using KeyT = A;
	using ValT = B;

	/**
	 * Encapsulate compare class
	 */
	struct FindPair
	{
		/**
		 * Compare two pairs
		 * 
		 * @param a, b two pairs to compare
		 * @return a GT b (1), a EQ b (0), a LT b (-1)
		 */
		FORCE_INLINE int32 operator()(const Pair & a, const Pair & b) const
		{
			return CompareT()(a.first, b.first);
		}

		/**
		 * Compare pair and key
		 * 
		 * @param a pair
		 * @param b key-only
		 * @return a GT b (1), a EQ b (0), a LT b (-1)
		 */
		FORCE_INLINE int32 operator()(const Pair & a, const A & b) const
		{
			return CompareT()(a.first, b);
		}

		/**
		 * Compare key and pair
		 * 
		 * @param a key-only
		 * @param b pair
		 * @return a GT b (1), a EQ b (0), a LT b (-1)
		 */
		FORCE_INLINE int32 operator()(const A & a, const Pair & b) const
		{
			return CompareT()(a, b.first);
		}
	};

	/// Key
	A first;

	/// Value
	B second;

	/**
	 * Pair constructor
	 * 
	 * @param [in] key pair key
	 * @param [in] val pair value
	 */
	template<typename KeyT, typename ValT>
	FORCE_INLINE Pair(KeyT && key, ValT && val)
		: first{forward<KeyT>(key)}
		, second{forward<ValT>(val)}
	{
		//
	}

	/**
	 * Returns pair key
	 */
	FORCE_INLINE const A & getKey() const
	{
		return first;
	}

	/**
	 * Returns pair value
	 * @{
	 */
	FORCE_INLINE B & getVal()
	{
		return second;
	}

	FORCE_INLINE const B & getVal() const
	{
		return second;
	}
	/** @} */

	/**
	 * Compare two pairs based on
	 * the provided compare type.
	 * 
	 * @param other another pair
	 * 	to compare with
	 * @return comapare result
	 * @{
	 */
	FORCE_INLINE bool operator==(const Pair & other) const
	{
		return CompareT{}(*this, other) == 0;
	}

	FORCE_INLINE bool operator!=(const Pair & other) const
	{
		return !(*this == other);
	}

	FORCE_INLINE bool operator<(const Pair & other) const
	{
		return CompareT{}(*this, other) < 0;
	}

	FORCE_INLINE bool operator>(const Pair & other) const
	{
		return CompareT{}(*this, other) > 0;
	}

	FORCE_INLINE bool operator<=(const Pair & other) const
	{
		return !(*this > other);
	}

	FORCE_INLINE bool operator>=(const Pair & other) const
	{
		return !(*this < other);
	}
	/** @} */
};