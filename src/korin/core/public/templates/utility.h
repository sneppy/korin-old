#pragma once

#include "core_types.h"
#include "./enable_if.h"
#include "./types.h"

/**
 * Forces move semantics by casting to rvalue reference
 * 
 * @param [in] t reference variable
 */
template<typename T>
constexpr FORCE_INLINE typename RemoveReference<T>::Type && move(T && t)
{
	return static_cast<typename RemoveReference<T>::Type&&>(t);
}

/**
 * Passes lvalues as lvalues and rvalues as lavalues
 * 
 * @param [in] t reference variable
 * @{
 */
template<typename T>
constexpr FORCE_INLINE T && forward(typename RemoveReference<T>::Type & t)
{
	return static_cast<T&&>(t);
}

template<typename T>
constexpr FORCE_INLINE T && forward(typename RemoveReference<T>::Type && t)
{
	return static_cast<T&&>(t);
}
/// @}

/**
 * Swap two values
 * 
 * @param [in] a,b values to swap
 */
template<typename T>
constexpr FORCE_INLINE void swap(T & a, T & b)
{
	T t{move(a)}; a = move(b), b = move(t);
}
