#pragma once

#include "../core_types.h"
#include "base.h"

/**
 * Sets Type to T
 */
template<typename T>
struct TypeIdentity
{
	using Type = T;
};

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

template<typename T> struct IsReference<T&>		{ enum {value = true}; };
template<typename T> struct IsReference<T&&>	{ enum {value = true}; };
/// @}

/**
 * Sets value to true if type is volatile
 * @{
 */
template<typename T>
struct IsVolatile
{
	enum {value = false};
};

template<typename T> struct IsVolatile<volatile T> { enum {value = true}; };
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
 * Sets value to true if T has the
 * type array of U
 */
template<typename T>
struct IsArray
{
	enum {value = false};
};

template<typename T>			struct IsArray<T[]>		{ enum {value = true}; };
template<typename T, sizet n>	struct IsArray<T[n]>	{ enum {value = true}; };

/**
 * Sets value to true if T is
 * a function type
 */
template<typename T>
struct IsCFunction
{
	enum {value = false};
};

template<typename RetT, typename ... Args> struct IsCFunction<RetT(Args...)> { enum {value = true}; };

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

template<typename T> struct RemovePointer<T*>					{ using Type = T; };
template<typename T> struct RemovePointer<T* const>				{ using Type = T; };
template<typename T> struct RemovePointer<T* volatile>			{ using Type = T; };
template<typename T> struct RemovePointer<T* const volatile>	{ using Type = T; };
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

template<typename T> struct RemoveReference<T&>		{ using Type = T; };
template<typename T> struct RemoveReference<T&&>	{ using Type = T; };
/// @}

/**
 * Remove volatile qualifier from T
 * @{
 */
template<typename T>
struct RemoveVolatile
{
	using Type = T;
};

template<typename T> struct RemoveVolatile<volatile T> { using Type = T; };
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

template<typename T> struct RemoveConst<const T> { using Type = T; };
/// @}

/**
 * Remove const and volatile qualifiers
 * @{
 */
template<typename T>
struct RemoveConstVolatile
{
	using Type = T;
};

template<typename T> struct RemoveConstVolatile<const T>			{ using Type = T; };
template<typename T> struct RemoveConstVolatile<volatile T>			{ using Type = T; };
template<typename T> struct RemoveConstVolatile<const volatile T>	{ using Type = T; };
/// @}

/**
 * If T is array, remove array extent
 */
template<typename T>
struct RemoveArray
{
	using Type = T;
};

template<typename T>			struct RemoveArray<T[]>		{ using Type = T; };
template<typename T, sizet n>	struct RemoveArray<T[n]>	{ using Type = T; };

/**
 * Adds pointer to the type
 * @{
 */
struct Private_AddPointer
{
	template<typename T>
	static auto addPtr(int) -> TypeIdentity<typename RemoveReference<T>::Type*>;

	template<typename T>
	static auto addPtr(...) -> TypeIdentity<T>;
};

template<typename T>
struct AddPointer : public decltype(Private_AddPointer::addPtr<T>(0))
{
	//
};
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

/**
 * Sets type to the decayed type of T
 */
template<typename T>
class DecayType
{
	/// Unreferenced type
	using U = typename RemoveReference<T>::Type;

public:
	using Type = typename ChooseType<
		IsArray<U>::value,
		typename RemoveArray<U>::Type, // If is array
		typename ChooseType<
			IsCFunction<U>::value,
			typename AddPointer<U>::Type,
			typename RemoveConstVolatile<U>::Type
		>::Type
	>::Type;
};