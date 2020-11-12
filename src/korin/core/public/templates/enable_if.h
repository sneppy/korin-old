#pragma once

#include "core_types.h"

/**
 * Sets the specified Type if Condition
 * evaluates to true
 * @{
 */
template<bool isEnabled, typename T = void>
struct EnableIf
{
	//
};

template<typename T> struct EnableIf<true, T> { using Type = T; };
/** @} */

/** Quick access to the enabled type */
template<bool isEnabled, typename T = void>
using EnableIfT = typename EnableIf<isEnabled, T>::Type;