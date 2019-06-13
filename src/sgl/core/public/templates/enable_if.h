#pragma once

#include "core_types.h"

/**
 * Sets the specified Type if Condition
 * evaluates to true
 * @{
 */
template<bool C, typename T = void>
struct EnableIf {};

template<typename T> struct EnableIf<true, T> { using Type = T; };
/// @}