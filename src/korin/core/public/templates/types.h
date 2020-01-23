#pragma once

#include "../core_types.h"

/**
 * Sets value to true if types are the same
 * @{
 */
template<typename A, typename B>
struct IsSameType
{
	enum {value = false};
};

template<typename T> struct IsSameType<T, T> { enum {value = true}; };
/// @}

/**
 * Sets value to true if type is void
 */
template<typename T>
using IsVoid = IsSameType<T, void>;

/**
 * Sets value to true if type is a pointer
 * @{
 */
template<typename T>
struct IsPointer
{
	enum {value = false};
};

template<typename T> struct IsPointer<T*> { enum {value = true}; };
/// @}

/**
 * Sets value to true if type is a reference
 * @{
 */
template<typename T>
struct IsReference
{
	enum {value = false};
};

template<typename T> struct IsReference<T&> { enum {value = true}; };
/// @}

/**
 * Sets value to true if type is integral
 * (i.e. int, char, short, etc.)
 * @{
 */
template<typename T>
struct IsIntegral
{
	enum {value = false};
};

template<> struct IsIntegral<int8>	{ enum {value = true}; };
template<> struct IsIntegral<int16>	{ enum {value = true}; };
template<> struct IsIntegral<int32>	{ enum {value = true}; };
template<> struct IsIntegral<int64>	{ enum {value = true}; };

template<> struct IsIntegral<uint8>		{ enum {value = true}; };
template<> struct IsIntegral<uint16>	{ enum {value = true}; };
template<> struct IsIntegral<uint32>	{ enum {value = true}; };
template<> struct IsIntegral<uint64>	{ enum {value = true}; };

template<> struct IsIntegral<bool> { enum {value = true}; };

template<> struct IsIntegral<ansichar>	{ enum {value = true}; };
template<> struct IsIntegral<widechar>	{ enum {value = true}; };
/// @}

/**
 * Sets value to true if T is a trivial type
 */
template<typename T>
struct IsTrivial
{
	enum {value = __is_trivial(T)};
};

/**
 * Sets value to true if T is a POD type
 */
template<typename T>
struct IsPOD
{
	enum {value = __is_pod(T)};
};

/**
 * Sets value to true if T has trivial
 * constructor
 */
template<typename T>
struct IsTriviallyConstructible
{
	enum {value = __has_trivial_constructor(T)};
};

/**
 * Sets value to true if T has trivial
 * destructor
 */
template<typename T>
struct IsTriviallyDestructible
{
	enum {value = __has_trivial_destructor(T)};
};

/**
 * Sets value to true if T has trivial
 * copy constructor
 */
template<typename T>
struct IsTriviallyCopyable
{
	enum {value = __has_trivial_copy(T)};
};

/**
 * Sets value to true if T has trivial
 * copy assignment
 */
template<typename T>
struct IsTriviallyAssignable
{
	enum {value = __has_trivial_assign(T)};
};

/**
 * Remove a single pointer from T
 * If T is not a pointer sets Type to T
 * @{
 */
template<typename T>
struct RemovePointer
{
	using Type = T;
};

template<typename T> struct RemovePointer<T*> { using Type = T; };
/// @}

/**
 * Remove all pointers from T
 * @{
 */
template<typename T>
struct RemoveAllPointers
{
	using Type = T;
};

template<typename T> struct RemoveAllPointers<T*> { using Type = typename RemoveAllPointers<T>::Type; };
/// @}

/**
 * Remove a single reference from T
 * If T is not a reference sets Type to T
 * @{
 */
template<typename T>
struct RemoveReference
{
	using Type = T;
};

template<typename T> struct RemoveReference<T&> { using Type = T; };
/// @}

/**
 * Remove const qualifier from T
 * @{
 */
template<typename T>
struct RemoveConst
{
	using Type = T;
};

template<typename T> struct RemoveConst<const T>	{ using Type = T; };
template<typename T> struct RemoveConst<const T*>	{ using Type = T*; };
template<typename T> struct RemoveConst<const T&>	{ using Type = T&; };
/// @}

/**
 * Get base type, which can include
 * pointers as well.
 */
template<typename T>
struct NakedType
{
	using Type = typename RemoveConst<typename RemoveReference<T>::Type>::Type;
};