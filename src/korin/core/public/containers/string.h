#pragma once

#include "core_types.h"
#include "hal/platform_memory.h"
#include "hal/platform_strings.h"
#include "array.h"

/**
 * 
 */
template<typename CharT>
class StringBase
{
	using ArrayT = Array<CharT>;

protected:
	/**
	 * Makes sure that string is terminated
	 */
	FORCE_INLINE void terminateString()
	{
		array[array.count - 1] = '\0';
	}

public:
	/**
	 * Default constructor, initializes an
	 * empty string.
	 */
	FORCE_INLINE StringBase()
		: array{}
	{
		//
	}

	/**
	 * Initializes an empty string of capacity n
	 * (excluding terminating character).
	 * 
	 * @param n string capacity
	 */
	explicit FORCE_INLINE StringBase(sizet n)
		: array{nullptr, n + 1}
	{
		terminateString();
	}

	/**
	 * Buffer constructor.
	 * 
	 * @param src stream of characters
	 * @param len source length (in Bytes)
	 */
	FORCE_INLINE StringBase(const CharT * src, sizet n)
		: StringBase{n}
	{
		// Copy source and terminate string
		Memory::memcpy(*array, src, n);
		array[n] = '\0';
	}

	/**
	 * C string constructor
	 * 
	 * @param src a null-terminated C string
	 */
	FORCE_INLINE StringBase(const CharT * src)
		: StringBase{src, PlatformStrings::getLength(src)}
	{
		//
	}

	/**
	 * 
	 */
	FORCE_INLINE StringBase(const Name & inName)
		: StringBase{*inName, inName.getLength()}
	{
		//
	}

	/**
	 * Copy constructor, copy string content.
	 * We override it because otherwise string
	 * would not be terminated
	 */
	FORCE_INLINE StringBase(const StringBase & other)
		: array{other.array.buffer, other.array.count}
	{
		//
	}

protected:
	/**
	 * Copy constructor with extra slack
	 * 
	 * @param other source string
	 * @param slack extra slack to add
	 */
	FORCE_INLINE StringBase(const StringBase & other, sizet slack)
		: array{other.array.buffer, other.array.count, slack}
	{
		//
	}

public:
	/**
	 * Returns string length.
	 * @{
	 */
	FORCE_INLINE sizet getLength() const
	{
		return array.count - 1;
	}

	METHOD_ALIAS_CONST(getSize, getLength)
	METHOD_ALIAS_CONST(getLen, getLength)
	/** @} */

	/**
	 * Returns string raw content.
	 * @{
	 */
	FORCE_INLINE CharT * operator*()
	{
		return *array;
	}

	METHOD_ALIAS(getData, operator*)
	METHOD_ALIAS(getBuffer, operator*)

	FORCE_INLINE const CharT * operator*() const
	{
		return *array;
	}

	METHOD_ALIAS_CONST(getData, operator*)
	METHOD_ALIAS_CONST(getBuffer, operator*)
	/** @} */

	/**
	 * Returns reference to underlying array
	 * resource.
	 * @{
	 */
	FORCE_INLINE ArrayT & getArray()
	{
		return array;
	}

	FORCE_INLINE const ArrayT & getArray() const
	{
		return array;
	}
	/** @} */

	/**
	 * Provides access to string characters.
	 * 
	 * @param idx character index
	 * @return ref to idx-th character
	 * @{
	 */
	FORCE_INLINE CharT & operator[](uint64 idx)
	{
		return array[idx];
	}

	METHOD_ALIAS(getAt, operator[])

	FORCE_INLINE const CharT & operator[](uint64 idx) const
	{
		return array[idx];
	}

	METHOD_ALIAS_CONST(getAt, operator[])
	/** @} */

	/**
	 * Compare string with another string,
	 * case sensitive
	 * 
	 * @param other another string-like object
	 * @return 0 if strings are equal, the
	 * 	difference of the first non-equal
	 * characters otherwise
	 */
	FORCE_INLINE int32 cmp(const StringBase & other) const
	{
		return PlatformStrings::cmp(array.buffer, other.array.buffer);
	}

	FORCE_INLINE int32 cmp(const CharT * other) const
	{
		return PlatformStrings::cmp(array.buffer, other);
	}
	/** @} */

	/**
	 * Compare string with another string,
	 * case insensitive
	 * 
	 * @param other another string-like object
	 * @return 0 if strings are equal, the
	 * 	difference of the first non-equal characters otherwise
	 */
	FORCE_INLINE int32 icmp(const StringBase & other) const
	{
		return PlatformStrings::icmp(array.buffer, other.array.buffer);
	}

	FORCE_INLINE int32 icmp(const CharT * other) const
	{
		return PlatformStrings::icmp(array.buffer, other);
	}
	/** @} */

	/**
	 * Returns true[false] if two
	 * strings are equal, case
	 * sensitive.
	 * 
	 * @param other other string
	 * @return true[false] if strings
	 * 	are equal
	 * @{
	 */
	FORCE_INLINE bool operator==(const StringBase & other) const
	{
		return cmp(other) == 0;
	}

	FORCE_INLINE bool operator==(const CharT * other) const
	{
		return cmp(other) == 0;
	}

	FORCE_INLINE bool operator!=(const StringBase & other) const
	{
		return cmp(other) != 0;
	}

	FORCE_INLINE bool operator!=(const CharT * other) const
	{
		return cmp(other) != 0;
	}
	/** @} */

	/**
	 * Return true if string is
	 * first[last] in lexicographic
	 * order
	 * 
	 * @param other other string
	 * @return true[false]
	 * @{
	 */
	template<typename StringT>
	FORCE_INLINE bool operator<(StringT && other) const
	{
		return cmp(other) < 0;
	}

	template<typename StringT>
	FORCE_INLINE bool operator>(StringT && other) const
	{
		return cmp(other) > 0;
	}

	template<typename StringT>
	FORCE_INLINE bool operator<=(StringT && other) const
	{
		return cmp(other) <= 0;
	}

	template<typename StringT>
	FORCE_INLINE bool operator>=(StringT && other) const
	{
		return cmp(other) >= 0;
	}
	/** @} */

private:
	/**
	 * Append a string at the end of this
	 * string.
	 * 
	 * @param src appended string
	 * @param len string length
	 */
	StringBase & appendString(const CharT * src, sizet len)
	{
		const sizet currLen = getLength();
		
		// Resize buffer and copy string content
		array(currLen + len) = '\0';
		Memory::memcpy(*array + currLen, src, len);

		return *this;
	}

	/**
	 * Append formatted text to string
	 * 
	 * @param format format string
	 * @param args format arguments
	 * @return ref to self
	 */
	template<typename... Args>
	StringBase & appendFormat(const CharT * format, Args &&... args)
	{
		// We have an overhead due to the fact that we
		// don't know the size of the formatted string
		// a priori
		constexpr size_t maxlen = 1024;
		CharT buff[maxlen] = {};

		// Print in buffer and append
		int32 len = snprintf(buff, maxlen, format, forward<Args>(args)...);
		return appendString(buff, len);
	}
	
public:
	/**
	 * Generic object formatter, uses
	 * toString() method.
	 * 
	 * @param arg format argument
	 * @return reference to self
	 */
	template<typename ArgT>
	FORCE_INLINE StringBase & operator+=(const ArgT & arg)
	{
		// Replace with formatted string
		return (*this += move(arg.toString()));
	}

	/**
	 * Append a single character to this
	 * string.
	 * 
	 * @param c character to append
	 * @return self
	 */
	StringBase & operator+=(CharT c)
	{
		array.add('\0');
		array[getLength()] = c;

		return *this;
	}

	/**
	 * Append a c string at the end of this
	 * string.
	 * 
	 * @param cStr c string to append
	 * @return self
	 */
	StringBase & operator+=(const CharT * cStr)
	{
		return appendString(cStr, PlatformStrings::getLength(cStr));
	}

	/**
	 * Append another string at the end of
	 * this string.
	 * 
	 * @param other another string to append
	 * @return reference to self
	 */
	StringBase & operator+=(const StringBase & other)
	{
		return appendString(*(other.array), other.getLength());
	}

	/**
	 * Append number as string.
	 * 
	 * @param num number to append
	 * @return reference to self
	 * @{
	 */
	FORCE_INLINE StringBase & operator+=(int32 num)
	{
		return appendFormat("%d", num);
	}

	FORCE_INLINE StringBase & operator+=(int64 num)
	{
		return appendFormat("%lld", num);
	}

	FORCE_INLINE StringBase & operator+=(uint32 num)
	{
		return appendFormat("%u", num);
	}

	FORCE_INLINE StringBase & operator+=(uint64 num)
	{
		return appendFormat("%llu", num);
	}

	FORCE_INLINE StringBase & operator+=(float32 num)
	{
		return appendFormat("%f", num);
	}

	FORCE_INLINE StringBase & operator+=(float64 num)
	{
		return appendFormat("%f", num);
	}
	/** @} */

	/**
	 * Makes a new copy of the string and
	 * appends content to it.
	 * 
	 * @param arg content to append
	 * @return new string
	 */
	template<typename Arg>
	FORCE_INLINE StringBase operator+(const Arg & arg) const
	{
		return (StringBase{*this} += (arg));
	}

private:
	/**
	 * Replaces content with another string.
	 * 
	 * @param src replacement string buffer
	 * @param len string length
	 * @return ref to self
	 */
	StringBase & printString(const CharT * src, sizet len)
	{
		array(len) = '\0';
		Memory::memcpy(*array, src, len);
	}

	/**
	 * Print formatted text into string.
	 * 
	 * @param format format string
	 * @param args format arguments
	 * @return reference to self
	 */
	template<typename... Args>
	StringBase & printFormat(const char * format, Args &&... args)
	{
		// We have an overhead due to the fact that we
		// don't know the size of the formatted string
		// a priori
		constexpr sizet maxlen = 1024;
		CharT buff[maxlen] = {};

		// Print in buffer
		const sizet len = snprintf(buff, maxlen, format, forward<Args>(args)...);
		return printString(buff, len);
	}

public:
	/**
	 * Generic object formatter, expects
	 * toString() method
	 * 
	 * @param arg format argument
	 * @return reference to self
	 */
	template<typename T>
	FORCE_INLINE StringBase & operator<<=(const T & t)
	{
		// Replace with formatted string
		return (*this = move(t.toString()));
	}
	
	/**
	 * Reads content from file and
	 * appends to string.
	 * 
	 * @param fp source file pointer
	 * @return reference to self
	 */
	StringBase & operator<<=(FILE * fp)
	{
		// TODO
		return *this;
	}

	/**
	 * Print number as string.
	 * 
	 * @param num number to append
	 * @return reference to self
	 * @{
	 */
	FORCE_INLINE StringBase & operator<<=(int32 num)
	{
		return printFormat("%d", num);
	}

	FORCE_INLINE StringBase & operator<<=(int64 num)
	{
		return printFormat("%lld", num);
	}

	FORCE_INLINE StringBase & operator<<=(uint32 num)
	{
		return printFormat("%u", num);
	}

	FORCE_INLINE StringBase & operator<<=(uint64 num)
	{
		return printFormat("%llu", num);
	}

	FORCE_INLINE StringBase & operator<<=(float32 num)
	{
		return printFormat("%f", num);
	}

	FORCE_INLINE StringBase & operator<<=(float64 num)
	{
		return printFormat("%f", num);
	}
	/** @} */

	/**
	 * Static constructor, returns a
	 * formatted string.
	 * 
	 * @param format format string
	 * @param args format arguments
	 */
	template<typename... Args>
	static FORCE_INLINE StringBase format(const char * format, Args &&... args)
	{
		// We don't know string length a priori
		constexpr sizet maxlen = 1024;
		char buff[maxlen] = {};

		// Print and return new string
		int32 len = snprintf(buff, maxlen, format, forward<Args>(args)...);
		return StringBase{buff, (sizet)len};
	}

	/**
	 * Returns a substring of the string.
	 * 
	 * @param len substring length
	 * @param pos initial position
	 * @return new string
	 */
	StringBase substr(sizet len, sizet pos = 0) const
	{
		return StringBase{array.buffer + pos, len};
	}
	
private:
	/**
	 * Private implementation of splice algorithm.
	 * @see splice
	 */
	StringBase & splice(sizet subPos, sizet subLen, const CharT * replSrc, sizet replLen)
	{
		if (replLen <= subLen)
		{
			// Since replacement string is smaller
			// then replaced substring, we don't
			// need to grow in size
			array.removeAt(subPos, subLen - replLen);

			if (replLen > 0)
			{
				// Copy replacement string
				Memory::memcpy(*array + subPos, replSrc, replLen);
			}
		}
		else
		{
			const sizet len = getLength();

			// Replacement string is longer, buffer
			// will be resized
			array(len + (replLen - subLen)) = '\0';
			Memory::memmov(*array + subPos + replLen, *array + subPos + subLen, len - (subPos + subLen));
			Memory::memcpy(*array + subPos, replSrc, replLen);
		}

		return *this;
	}

public:
	/**
	 * Replaces substr(len, pos) with inserted
	 * string.
	 * 
	 * @param pos position of the substring
	 * @param len length of the substring
	 * @param [inserted=""] replacement string
	 * @return ref to self
	 * @{
	 */
	StringBase & splice(sizet pos, sizet len, const StringBase & inserted = "")
	{
		return splice(pos, len, *inserted, inserted.getLength());
	}

	StringBase & splice(sizet pos, sizet len, const CharT * cStr = "")
	{
		return splice(pos, len, cStr, PlatformStrings::getLength(cStr));
	}
	/** @} */

	/**
	 * Find index of first occurence in string.
	 * Pattern can be a string, a character or
	 * a regular expression.
	 * 
	 * @param pattern pattern to search for
	 * 	in string
	 * @param startPos start position
	 * @param patternLen if pattern is C string,
	 * 	length of the pattern
	 * @return index of first occurence, or -1 if
	 * 	none found
	 * @{
	 */
	int64 findIndex(const char * pattern, sizet startPos = 0, sizet patternLen = 0) const
	{
		// Get length if not provided
		patternLen = patternLen ? patternLen : PlatformStrings::getLength(pattern);

		// Check pattern length doesn't exceed string length
		if (patternLen > getLength() - startPos) return -1;

		for (sizet idx = startPos, lastIdx = getLength() - patternLen; idx <= lastIdx; ++idx)
		{
			; // TODO: Maybe have some nice pattern-matching solution
			if (Memory::memcmp(&array[idx], pattern, patternLen) == 0) return idx;
		}

		return -1;
	}

	FORCE_INLINE int64 findIndex(const StringBase & pattern, sizet startPos = 0) const
	{
		return findIndex(*pattern, startPos, pattern.getLength());
	}

	int64 findIndex(char pattern, sizet startPos = 0) const
	{
		for (sizet idx = startPos, len = getLength(); idx < len; ++idx)
		{
			if (array[idx] == pattern) return idx;
		}

		// Pattern not found, return -1
		return -1;
	}
	/** @} */

	/**
	 * Utility function that return all
	 * occurences of pattern in the
	 * string.
	 * 
	 * @param pattern pattern to match.
	 * 	Either a string, a character or
	 * 	a regular expression.
	 * @return array of occurences
	 */
	template<typename PatternT>
	Array<sizet> findAll(PatternT && pattern) const
	{
		Array<sizet> idxs{};
		int64 startPos = 0;

		while ((startPos = findIndex(forward<PatternT>(pattern), startPos)) != -1)
		{
			// Add occurence and advance position
			idxs.add(static_cast<sizet>(startPos++));
		}

		return idxs;
	}

	/**
	 * Replaces occurences of text into string
	 * 
	 * @param pattern 
	 */
	StringBase & replaceAll(const StringBase & pattern, const StringBase & replacement)
	{
		sizet patternLen = pattern.getLength();
		sizet replacementLen = replacement.getLength();
		int64 startPos = 0;

		while ((startPos = findIndex(pattern, startPos)) != -1)
		{
			// Occurence found, needs replacing
			splice(startPos, patternLen, replacement);

			// Skip replacement string
			startPos += replacementLen;
		}

		return *this;
	}

	/**
	 * Returns a lowercase copy of the string.
	 */
	StringBase toLower() const
	{
		StringBase lower{*this};

		// Transform each character
		for (sizet idx = 0, len = getLength(); idx < len; ++idx)
		{
			// Sum the difference
			if (lower[idx] >= 'A' && lower[idx] <= 'Z') lower[idx] += ('a' - 'A');
		}

		return lower;
	}

	/**
	 * Returns an uppercase copy of the string.
	 */
	StringBase toUpper() const
	{
		StringBase upper{*this};

		// Transform each character
		for (sizet idx = 0, len = getLength(); idx < len; ++idx)
		{
			// Sum the difference
			if (upper[idx] >= 'a' && upper[idx] <= 'z') upper[idx] -= ('a' - 'A');
		}

		return upper;
	}

protected:
	/// Underlying array object
	ArrayT array;
};

/**
 * Array toString implementation.
 */
template<typename T>
String Array<T>::toString() const
{
	// Default if no items
	if (count == 0) return "[]";

	String out = "[";

	for (uint64 idx = 0; idx < count - 1; ++idx)
	{
		out += buffer[idx];
		out += ", ";
	}

	return out + buffer[count - 1] + "]";
}