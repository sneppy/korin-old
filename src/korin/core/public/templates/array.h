#pragma once

#include "core_types.h"

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
	constexpr StaticArray()
		: buffer{}
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
