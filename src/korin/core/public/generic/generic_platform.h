#pragma once

/**
 * Sets Type to appropriate type according
 * to pointer type size
 * @{
 */
template<typename T32, typename T64, unsigned long long N>
struct SelectIntPointerType {};

template<typename T32, typename T64> struct SelectIntPointerType<T32, T64, 4> { using Type = T32; };
template<typename T32, typename T64> struct SelectIntPointerType<T32, T64, 8> { using Type = T64; };
/// @}

/**
 * Generic platform types, can be derived to
 * set platform dependent types
 */
struct GenericPlatformTypes
{
	/// Integer types @{
	using int8	= signed char;
	using int16 = signed short;
	using int32 = signed int;
	using int64	= signed long long;

	using uint8		= unsigned char;
	using uint16	= unsigned short;
	using uint32	= unsigned int;
	using uint64	= unsigned long long;

	using ubyte	= uint8;
	using byte	= int8;

	using intp	= typename SelectIntPointerType<int32, int64, sizeof(void*)>::Type;
	using uintp	= typename SelectIntPointerType<uint32, uint64, sizeof(void*)>::Type;
	using sizet	= uintp;
	/// @}

	/// Floating-point types @{
	using float32	= float;
	using float64	= double;
	using float128	= long double;
	/// @}

	/// Character types @{
	using ansichar	= char;
	using widechar	= wchar_t;
	using char8		= uint8;
	using char16	= uint16;
	using char32	= uint32;
	/// @}
};