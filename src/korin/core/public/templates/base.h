#pragma once

#include "core_types.h"

/**
 * Sets Type to first type if value is true,
 * to second type otherwise
 */
template<bool cond, typename A, typename B>
struct ChooseType
{
	using Type = A;
};

template<typename A, typename B> struct ChooseType<false, A, B> { using Type = B; };