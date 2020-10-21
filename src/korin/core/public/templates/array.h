#pragma once

#include "core_types.h"
#include "./utility.h"

/**
 * This class represent a static
 * array, i.e. a contigous list
 * of items of the same type,
 * allocated on the stack rather
 * than on the heap. As such, the
 * size of a static array is must
 * be known at compile time and
 * cannot change thereafter.
 * 
 * @param T the type of the items
 * @param inCount number of items
 * 	in array
 */
template<typename T, uint64 inCount>
struct StaticArray
{
	/**
	 * Default construct array.
	 */
	constexpr FORCE_INLINE StaticArray()
		: buffer{}
		, count{inCount}
	{
		//
	}

	/**
	 * 
	 */
	constexpr FORCE_INLINE StaticArray(const T * inBuffer)
		: buffer{}
		, count{inCount}
	{
		for (uint64 idx = 0; idx < count; ++idx)
		{
			buffer[idx] = inBuffer[idx];
		}
	}

	/**
	 * Constructor that accepts one or
	 * more item values.
	 * 
	 * @param items item to insert in
	 * 	array
	 */
	template<typename ...ItemPackT>
	constexpr FORCE_INLINE StaticArray(ItemPackT && ...items)
		: buffer{forward<ItemPackT>(items)...}
		, count{inCount}
	{
		//
	}

	/**
	 * Returns number of items in array.
	 */
	constexpr FORCE_INLINE uint64 getCount() const
	{
		return count;
	}

	/**
	 * Returns pointer to the buffer.
	 */
	constexpr FORCE_INLINE const T * operator*() const
	{
		return static_cast<const T*>(buffer);
	}

	constexpr FORCE_INLINE T * operator*()
	{
		return static_cast<T*>(buffer);
	}
	/** @} */

	/**
	 * Returns ref to the idx-th item.
	 */
	constexpr FORCE_INLINE const T & operator[](uint64 idx) const
	{
		return buffer[idx];
	}

	constexpr FORCE_INLINE T & operator[](uint64 idx)
	{
		return buffer[idx];
	}
	/** @} */

protected:
	/// The stack allocated buffer
	T buffer[inCount];

	/// The size of the buffer, expressed
	/// as number of items
	uint64 count;
};

template<typename T, uint64 inSize>
constexpr FORCE_INLINE StaticArray<T, inSize> makeStaticArray(const T (&inList)[inSize])
{
	return StaticArray<T, inSize>(static_cast<const T*>(inList));
}

/**
 * StaticArray alias.
 * @see StaticArray
 */
template<typename T, uint64 size>
using Tuple = StaticArray<T, size>;
