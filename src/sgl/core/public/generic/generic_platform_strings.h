#pragma once

#include "core_types.h"

#define CHAR_COMPAREI(c1, c2) \
	( \
		c1 == c2 || \
		(c1 >= 'a' && c1 <= 'z' && (c1 - c2) == ('a' - 'A')) || \
		(c1 >= 'A' && c1 <= 'Z' && (c1 - c2) == ('A' - 'a')) \
	)

/**
 * Platform-agnostic string operations
 */
struct GenericPlatformStrings
{
	/**
	 * Returns the length of the string
	 * String must be terminated
	 * 
	 * @param [in] str string
	 * @return string length
	 */
	static FORCE_INLINE sizet getLength(const char * str)
	{
		sizet len; for (len = 0; *str != '\0'; ++str, ++len);
		return len;
	}

	/**
	 * Compare two strings, case sensitive
	 * 
	 * @param [in] s1,s2 string operands
	 * @return 0 if strings are equal, otherwise the difference between first non-equal characters
	 */
	static FORCE_INLINE int32 cmp(const char * s1, const char * s2)
	{
		for (; *s1 == *s2; ++s1, ++s2)
			// All characters before the termination are equal
			if (*s1 == '\0') return 0;
		
		return *s1 - *s2;
	}

	/**
	 * Compare two strings, case insensitive
	 * 
	 * @param [in] s1,s2 string operands
	 * @return 0 if strings are equal, the difference of the first non-equal characters otherwise
	 */
	static FORCE_INLINE int32 icmp(const char * s1, const char * s2)
	{
		for (; CHAR_COMPAREI(*s1, *s2); ++s1, ++s2)
			// All characters before the termination are equal
			if (*s1 == '\0') return 0;
		
		// TODO: what should we return
		// ?
		return *s1 - *s2;
	}

	/**
	 * Compare two substrings, case sensitive
	 * 
	 * @param [in] s1,s2 string operands
	 * @param [in] len substring length
	 * @param [in] begin substring offset
	 * @return 0 if strings are equal, the difference of the first non-equal characters otherwise
	 */
	static FORCE_INLINE int32 cmpn(const char * s1, const char * s2, sizet len, sizet start = 0)
	{
		const sizet end = start + len;
		for (sizet n = start; n < end; ++n)
			if (s1[n] != s2[n]) return s1[n] - s2[n];
		
		return 0;
	}

	/**
	 * Compare two substrings, case insensitive
	 * 
	 * @param [in] s1,s2 string operands
	 * @param [in] len substring length
	 * @param [in] begin substring offset
	 * @return 0 if strings are equal, the difference of the first non-equal characters otherwise
	 */
	static FORCE_INLINE int32 icmpn(const char * s1, const char * s2, sizet len, sizet start = 0)
	{
		const sizet end = start + len;
		for (sizet n = start; n < end; ++n)
			// TODO: what should we return
			// ?
			if (!CHAR_COMPAREI(s1[n], s2[n])) return s1[n] - s2[n];
		
		return 0;
	}
};