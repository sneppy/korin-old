#pragma once

#include "core_types.h"
#include "templates/utility.h"

/**
 * A struct with two elements (conceptually
 * key and value or simply first and second)
 */
template<typename A, typename B, typename CompareT>
struct Pair
{
	/// Key and value types @{
	using KeyT = A;
	using ValT = B;
	/// @}

	/**
	 * 
	 */
	struct FindPair
	{
		FORCE_INLINE int32 operator()(const Pair & a, const Pair & b) const
		{
			return CompareT()(a.first, b.first);
		}
	};

	/// Key
	A first;

	/// Value
	B second;

public:
	/**
	 * Default constructor, zero initialize both
	 */
	FORCE_INLINE Pair()
		: first{}
		, second{}
	{
		//
	}

	/**
	 * Key constructor, zero initialize value
	 * 
	 * @param [in] key pair key
	 * @{
	 */
	FORCE_INLINE Pair(const A & key)
		: first{key}
		, second{}
	{
		//
	}

	FORCE_INLINE Pair(A && key)
		: first{move(key)}
		, second{}
	{
		//
	}
	/// @}

	/**
	 * Pair constructor
	 * 
	 * @param [in] key pair key
	 * @param [in] val pair value
	 */
	template<typename _A, typename _B>
	FORCE_INLINE Pair(_A && key, _B && val)
		: first{forward<_A>(key)}
		, second{forward<_B>(val)}
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
	/// @}

	// TODO: Comparison operators
};