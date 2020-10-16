#pragma once

#include "core_types.h"

/**
 * Null compare type, that always returns 0.
 */
struct NullCompare
{
	template<typename A, typename B>
	FORCE_INLINE int32 operator()(const A&&, const B&&) const
	{
		return 0;
	}

	template<typename A, typename B>
	FORCE_INLINE int32 operator()(const A*, const B*) const
	{
		return 0;
	}
};

/**
 * Default three way compare. Overloaded apply
 * operator returns 1 if first argument is greater
 * than second argument, -1 if first argument is
 * smaller than second argument, and 0 otherwise.
 * 
 * It uses class-defined LT and GT operators to
 * compare the objects.
 */
struct ThreeWayCompare
{
	template<typename A, typename B>
	FORCE_INLINE int32 operator()(A && a, B && b) const
	{
		return static_cast<int32>(a > b) - static_cast<int32>(a < b);
	}

	template<typename A, typename B>
	FORCE_INLINE int32 operator()(const A * a, const B * b) const
	{
		return *this(forward<A>(*a), forward<B>(*b));
	}
};