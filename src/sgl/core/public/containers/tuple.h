#pragma once

#include "../core_types.h"
#include "../templates/types.h"
#include "../templates/utility.h"
#include "containers_types.h"

/**
 * A fixed size array.
 * 
 * @param T element type
 * @param n tuple length
 */
template<typename T, sizet n>
class Tuple
{
	template<typename, sizet>		friend class Tuple;
	template<typename, typename>	friend class Array;

public:
	/// Tuple length
	constexpr static sizet len{n};

protected:
	/// Tuple values
	T values[n];

public:
	/**
	 * Default constructor, empty tuple.
	 */
	Tuple() = default;

	/**
	 * Copy constructor.
	 * @{
	 */
	Tuple(const Tuple & other) = default;
	Tuple(Tuple & other) = default;
	/// @}

	/**
	 * Move constructor
	 */
	Tuple(Tuple && other) = default;

	/**
	 * Buffer constructor
	 */
	FORCE_INLINE Tuple(const T * src)
	{
		// Copy elements
		Memory::constructCopyElements(values, src, n);
	}

	/**
	 * List constructor.
	 */
	template<typename ... Values> 
	constexpr FORCE_INLINE Tuple(Values && ... inValues)
		: values{forward<Values>(inValues) ...}
	{
		//
	}

	/**
	 * Create new tuple with other
	 * tuple's elements
	 */
	template<sizet m>
	constexpr FORCE_INLINE Tuple(const Tuple<T, m> & other)
		: Tuple{other.values}
	{
		//
	}

	/**
	 * Indexing operator.
	 * Negative indices return elements
	 * starting from the last one (i.e. 
	 * -1 returns last element)
	 * 
	 * @param i index
	 * @returns ref to i-th element
	 * @{
	 */
	FORCE_INLINE const T & operator[](int64 i) const
	{
		return i < 0 ? values[len + i] : values[i];
	}

	FORCE_INLINE T & operator[](int64 i)
	{
		return i < 0 ? values[len + i] : values[i];
	}
	/// @}

	/**
	 * Append tuple to tuple.
	 * 
	 * @param other tuple to append
	 * @returns new tuple
	 */
	template<sizet m>
	FORCE_INLINE Tuple<T, n + m> operator+(const Tuple<T, m> & other) const
	{
		Tuple<T, n + m> out;
		Memory::constructCopyElements(out.values, values, n);
		Memory::constructCopyElements(out.values + n, other.values, m);

		return out;
	}

	/**
	 * Append element to tuple.
	 * 
	 * @param t element to append
	 * @returns new tuple
	 */
	template<typename TT>
	FORCE_INLINE typename EnableIf<IsSameType<TT, T>::value, Tuple<T, n + 1>>::Type operator+(TT && t) const
	{
		Tuple<T, n + 1> out{*this};
		out[n] = forward<TT>(t);

		return out;
	}

	/**
	 * Slice tuple
	 */
	template<int64 begin = 0, int64 end = n>
	FORCE_INLINE Tuple<T, ((n + end) % n) - ((n + begin) % n)> slice() const
	{
		constexpr int64 x = (n + begin) % n;
		constexpr int64 y = (n + end) % n;
		constexpr sizet z = y - x;
		return Tuple<T, z>{values + x};
	}

	/**
	 * Cast to Array
	 */
	template<typename MallocT>
	FORCE_INLINE operator Array<T, MallocT>() const
	{
		return Array<T, MallocT>{values, n};
	}
};

template<typename T, sizet n>
constexpr sizet Tuple<T, n>::len;

/**
 * Helper function to deduce tuple
 * type and size
 * 
 * @param a first element
 * @param bc arbitrary number of elements
 * @return new tuple with element type
 * 	of first element
 */
template<typename T, typename ... Args>
constexpr FORCE_INLINE Tuple<typename RemoveReference<T>::Type, sizeof ... (Args) + 1> __make_tuple(T && a, Args && ... args)
{
	return Tuple<typename RemoveReference<T>::Type, sizeof ... (Args) + 1>{forward<T>(a), forward<Args>(args) ...};
}

/**
 * Shorthand for @ref __make_tuple
 */
#define T(...) __make_tuple(__VA_ARGS__)