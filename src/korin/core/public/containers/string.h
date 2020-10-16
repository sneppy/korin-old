#pragma once

#include "core_types.h"
#include "hal/platform_memory.h"
#include "hal/platform_strings.h"
#include "array.h"

/**
 * 
 */
class String
{
protected:
	/// String content
	Array<ansichar, MallocAnsi> array;

protected:
	/**
	 * Makes sure that string is terminated
	 */
	FORCE_INLINE void terminateString()
	{
		array[array.count] = '\0';
	}

public:
	/**
	 * Default constructor, initializes an
	 * empty string with length n
	 * 
	 * @param [in] n string length
	 */
	FORCE_INLINE String()
		: array{0, 0, nullptr}
	{
		//
	}

	/**
	 * Initializes an empty string of capacity n
	 * (excluding terminating character)
	 * 
	 * @param [in] n string capacity
	 */
	explicit FORCE_INLINE String(sizet n)
		: array{n + 1, n, nullptr}
	{
		terminateString();
	}

	/**
	 * Buffer constructor
	 * 
	 * @param [in] src stream of characters
	 * @param [in] len source length (in Bytes)
	 */
	FORCE_INLINE String(const ansichar * src, sizet n)
		: array{n + 1, n, nullptr}
	{
		// Copy source and terminate string
		Memory::memcpy(array.buffer, src, n);
		terminateString();
	}

	/**
	 * C string constructor
	 * 
	 * @param [in] src a null-terminated C string
	 */
	FORCE_INLINE String(const ansichar * src)
		: array{PlatformStrings::getLength(src) + 1, 0, nullptr}
	{
		// Manually set count
		array.count = array.capacity - 1;

		// Copy source string (including null character)
		Memory::memcpy(array.buffer, src, array.capacity);
	}

	/**
	 * Copy constructor, copy string content.
	 * We override it because otherwise string
	 * would not be terminated
	 */
	FORCE_INLINE String(const String & other)
		: array{other.array.buffer, other.array.count, 1, nullptr}
	{
		terminateString();
	}

protected:
	/**
	 * Copy constructor with extra slack
	 * 
	 * @param [in] other source string
	 * @param [in] slack extra slack to add
	 */
	FORCE_INLINE String(const String & other, sizet slack)
		: array{other.array.buffer, other.array.count, slack + 1, nullptr}
	{
		terminateString();
	}

public:
	/**
	 * Move constructor, no copy involved
	 */
	FORCE_INLINE String(String && other)
		: array{move(other.array)}
	{
		//
	}

	/**
	 * Copy assignment
	 */
	FORCE_INLINE String & operator=(const String & other)
	{
		// Copy array
		array.resizeIfNecessary(other.array.count + 1);
		array.count = other.array.count;
		PlatformMemory::memcpy(array.buffer, other.array.buffer, array.count + 1);

		return *this;
	}

	/**
	 * Move assignment
	 */
	FORCE_INLINE String & operator=(String && other)
	{
		// Move array
		array = move(other.array);
		return *this;
	}

	/**
	 * Returns string length
	 * @{
	 */
	FORCE_INLINE sizet getLength() const
	{
		return array.count;
	}
	METHOD_ALIAS_CONST(getSize, getLength)
	METHOD_ALIAS_CONST(getLen, getLength)
	/// @}

	/**
	 * Returns string raw content
	 * @{
	 */
	FORCE_INLINE ansichar * operator*()
	{
		return array.buffer;
	}
	METHOD_ALIAS(getData, operator*);

	FORCE_INLINE const ansichar * operator*() const
	{
		return array.buffer;
	}
	METHOD_ALIAS_CONST(getData, operator*);
	/// @}

	/**
	 * Returns reference to underlying array
	 * resource
	 * @{
	 */
	FORCE_INLINE Array<ansichar> & getArray()
	{
		return array;
	}

	FORCE_INLINE const Array<ansichar> & getArray() const
	{
		return array;
	}
	/// @}

	/**
	 * Provides access to string characters
	 * 
	 * @param [in] idx character index
	 * @return lvalue reference
	 * @{
	 */
	FORCE_INLINE ansichar & operator[](uint64 idx)
	{
		return array[idx];
	}
	METHOD_ALIAS(getAt, operator[]);

	FORCE_INLINE const ansichar & operator[](uint64 idx) const
	{
		return array[idx];
	}
	METHOD_ALIAS_CONST(getAt, operator[]);
	/// @}

	/**
	 * Compare string with another string, case sensitive
	 * 
	 * @param [in] other string operand or c string
	 * @return 0 if strings are equal, the difference of the first non-equal characters otherwise
	 */
	FORCE_INLINE int32 cmp(const String & other) const
	{
		return PlatformStrings::cmp(array.buffer, other.array.buffer);
	}

	FORCE_INLINE int32 cmp(const char * other) const
	{
		return PlatformStrings::cmp(array.buffer, other);
	}
	/// @}

	/**
	 * Compare string with another string, case insensitive
	 * 
	 * @param [in] other string operand or c string
	 * @return 0 if strings are equal, the difference of the first non-equal characters otherwise
	 */
	FORCE_INLINE int32 icmp(const String & other) const
	{
		return PlatformStrings::icmp(array.buffer, other.array.buffer);
	}

	FORCE_INLINE int32 icmp(const char * other) const
	{
		return PlatformStrings::icmp(array.buffer, other);
	}
	/// @}

	/**
	 * Returns true[false] if two
	 * strings are equal, case
	 * sensitive
	 * 
	 * @param [in] other other string
	 * @return true[false] if strings
	 * 	are equal
	 * @{
	 */
	FORCE_INLINE bool operator==(const String & other) const
	{
		return cmp(other) == 0;
	}

	FORCE_INLINE bool operator==(const char * other) const
	{
		return cmp(other) == 0;
	}

	FORCE_INLINE bool operator!=(const String & other) const
	{
		return cmp(other) != 0;
	}

	FORCE_INLINE bool operator!=(const char * other) const
	{
		return cmp(other) != 0;
	}
	/// @}

	/**
	 * Return true if string is
	 * first[last] in lexicographic
	 * order
	 * 
	 * @param other other string
	 * @return true[false]
	 * @{
	 */
	FORCE_INLINE bool operator<(const String & other) const
	{
		return cmp(other) < 0;
	}

	FORCE_INLINE bool operator<(const char * other) const
	{
		return cmp(other) < 0;
	}

	FORCE_INLINE bool operator>(const String & other) const
	{
		return cmp(other) > 0;
	}

	FORCE_INLINE bool operator>(const char * other) const
	{
		return cmp(other) > 0;
	}

	FORCE_INLINE bool operator<=(const String & other) const
	{
		return cmp(other) <= 0;
	}

	FORCE_INLINE bool operator<=(const char * other) const
	{
		return cmp(other) <= 0;
	}

	FORCE_INLINE bool operator>=(const String & other) const
	{
		return cmp(other) >= 0;
	}

	FORCE_INLINE bool operator>=(const char * other) const
	{
		return cmp(other) >= 0;
	}
	/// @}

	/**
	 * Append formatted text to string
	 * 
	 * @param [in] format format string
	 * @param [in] args format arguments
	 * @return reference to self
	 */
	template<typename... Args>
	String & appendFormat(const char * format, Args &&... args)
	{
		// We have an overhead due to the fact that we
		// don't know the size of the formatted string
		// a priori
		constexpr size_t maxlen = 1024;
		char buff[maxlen] = {};

		// Print in buffer
		int32 len = snprintf(buff, maxlen, format, forward<Args>(args)...);

		// Resize buffer and copy content
		array.resizeIfNecessary(array.count + len + 1);
		PlatformMemory::memcpy(array.buffer + array.count, buff, len + 1);
		array.count += len;

		return *this;
	}

	/**
	 * Generic object formatter, expects toString() method
	 * * Actually, I haven't decided yet. I don' like toString()
	 * * reminds me of fucking Java
	 * 
	 * @param [in] arg format argument
	 * @return reference to self
	 */
	template<typename T>
	FORCE_INLINE String & operator+=(const T & t)
	{
		// Replace with formatted string
		return operator+=(move(t.toString()));
	}

	/**
	 * Append a single character to this string
	 * 
	 * @param [in] c character to append
	 * @return self
	 */
	String & operator+=(ansichar c)
	{
		// Resize and write character in buffer
		array.resizeIfNecessary(array.count + 2);
		array.buffer[array.count] = c;
		array.count += 1;
		terminateString();

		return *this;
	}

	/**
	 * Append a c string at the end of this
	 * string
	 * 
	 * @param [in] string c string to append
	 * @return self
	 */
	String & operator+=(const ansichar * cstr)
	{
		const sizet len = PlatformStrings::getLength(cstr);
		
		// Resize buffer and copy string content
		array.resizeIfNecessary(array.count + len + 1);
		Memory::memcpy(array.buffer + array.count, cstr, len + 1);
		array.count += len;

		return *this;
	}

	/**
	 * Append another string at the end of
	 * this string
	 * 
	 * @param [in] other string to append
	 * @return reference to self
	 */
	String & operator+=(const String & other)
	{
		// Resize buffer and copy string content
		array.resizeIfNecessary(array.count + other.array.count + 1);
		Memory::memcpy(array.buffer + array.count, other.array.buffer, other.array.count + 1);
		array.count += other.array.count;

		return *this;
	}

	/**
	 * Append number as string
	 * 
	 * @param [in] num number to append
	 * @return reference to self
	 * @{
	 */
	FORCE_INLINE String & operator+=(int32 num)
	{
		return appendFormat("%d", num);
	}

	FORCE_INLINE String & operator+=(int64 num)
	{
		return appendFormat("%lld", num);
	}

	FORCE_INLINE String & operator+=(uint32 num)
	{
		return appendFormat("%u", num);
	}

	FORCE_INLINE String & operator+=(uint64 num)
	{
		return appendFormat("%llu", num);
	}

	FORCE_INLINE String & operator+=(float32 num)
	{
		return appendFormat("%f", num);
	}

	FORCE_INLINE String & operator+=(float64 num)
	{
		return appendFormat("%f", num);
	}
	/// @}

	/**
	 * Makes a new copy of the string and appends content to it
	 * 
	 * @param [in] arg content to append
	 * @return new string
	 */
	template<typename Arg>
	FORCE_INLINE String operator+(const Arg & arg) const
	{
		return String{*this}.operator+=(arg);
	}

	/**
	 * Print formatted text into string
	 * 
	 * @param [in] format format string
	 * @param [in] args format arguments
	 * @return reference to self
	 */
	template<typename... Args>
	String & printFormat(const char * format, Args &&... args)
	{
		// We have an overhead due to the fact that we
		// don't know the size of the formatted string
		// a priori
		constexpr sizet maxlen = 1024;
		char buff[maxlen] = {};

		// Print in buffer
		array.count = snprintf(buff, maxlen, format, forward<Args>(args)...);
		
		// Resize buffer and copy content
		array.resizeIfNecessary(array.count + 1);
		PlatformMemory::memcpy(array.buffer, buff, (sizet)(array.count + 1));

		return *this;
	}

	/**
	 * Generic object formatter, expects toString() method
	 * * Actually, I haven't decided yet. I don' like toString()
	 * * reminds me of fucking Java
	 * 
	 * @param [in] arg format argument
	 * @return reference to self
	 */
	template<typename T>
	FORCE_INLINE String & operator<<=(const T & t)
	{
		// Replace with formatted string
		return operator=(move(t.toString()));
	}
	
	/**
	 * Read content from file and append to string
	 * 
	 * @param [in] fp source file pointer
	 * @return reference to self
	 */
	String & operator<<=(FILE * fp)
	{
		return *this;
	}

	/**
	 * Print number as string
	 * 
	 * @param [in] num number to append
	 * @return reference to self
	 * @{
	 */
	FORCE_INLINE String & operator<<=(int32 num)
	{
		return printFormat("%d", num);
	}

	FORCE_INLINE String & operator<<=(int64 num)
	{
		return printFormat("%lld", num);
	}

	FORCE_INLINE String & operator<<=(uint32 num)
	{
		return printFormat("%u", num);
	}

	FORCE_INLINE String & operator<<=(uint64 num)
	{
		return printFormat("%llu", num);
	}

	FORCE_INLINE String & operator<<=(float32 num)
	{
		return printFormat("%f", num);
	}

	FORCE_INLINE String & operator<<=(float64 num)
	{
		return printFormat("%f", num);
	}
	/// @}

	/**
	 * Makes a new copy of the string with the new content
	 * 
	 * @param [in] arg argument to format
	 * @return new string
	 */
	template<typename Arg>
	FORCE_INLINE String operator<<(const Arg & arg) const
	{
		return String{}.operator<<=(arg);
	}

	/**
	 * Static constructor, returns a formatted string
	 * 
	 * @param [in] format format string
	 * @param [in] args format arguments
	 */
	template<typename... Args>
	static FORCE_INLINE String format(const char * format, Args &&... args)
	{
		// We don't know string length a priori
		constexpr sizet maxlen = 1024;
		char buff[maxlen] = {};

		// Print and return new string
		int32 len = snprintf(buff, maxlen, format, forward<Args>(args)...);
		return String{buff, (sizet)len};
	}

	/**
	 * Returns a substring of the string
	 * 
	 * @param [in] len substring length
	 * @param [in] pos initial position
	 * @return new string
	 */
	String substr(sizet len, sizet pos = 0) const
	{
		return String{array.buffer + pos, len};
	}

	/**
	 * Replaces substr(len, pos) with inserted string.
	 * 
	 * @param pos position of the substring
	 * @param len length of the substring
	 * @param [inserted=""] replacement string
	 * @return ref to self
	 */
	String & splice(sizet pos, sizet len, const String & inserted = "")
	{
		// Get insertion size
		const sizet insertionLen = inserted.getLength();

		if (insertionLen <= len)
		{
			// We have to remove a bunch of characters anyway
			// First remove unused characters
			array.removeAt(pos, len - insertionLen);

			if (inserted.getLength())
			{
				// Copy inserted string in place
				PlatformMemory::memcpy(*array + pos, inserted.getData(), insertionLen);
			}
		}
		else
		{
			// We only have to add characters
			// First, replace array
			array.resizeIfNecessary(array.count += (insertionLen - len));

			// Move to accomodate string
			PlatformMemory::memmov(array.buffer + pos + insertionLen, array.buffer + pos + len, array.count - (len + pos));

			// Copy inserted string in place
			PlatformMemory::memcpy(*array + pos, inserted.getData(), insertionLen);
		}

		// Re-terminate string
		terminateString();

		return *this;
	}

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
			if (PlatformMemory::memcmp(&array[idx], pattern, patternLen) == 0) return idx;
		}

		return -1;
	}

	FORCE_INLINE int64 findIndex(const String & pattern, sizet startPos = 0) const
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
	/// @}

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
	String & replaceAll(const String & pattern, const String & replacement)
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
	String toLower() const
	{
		String lower{*this};

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
	String toUpper() const
	{
		String upper{*this};

		// Transform each character
		for (sizet idx = 0, len = getLength(); idx < len; ++idx)
		{
			// Sum the difference
			if (upper[idx] >= 'a' && upper[idx] <= 'z') upper[idx] -= ('a' - 'A');
		}

		return upper;
	}
};

/**
 * Special case, because we could not define
 * this function before defining String class
 */
template<typename T, typename MallocT>
String Array<T, MallocT>::toString() const
{
	String out{"["};
	uint64 i; for (i = 0; i < count - 1; ++i)
	{
		out += buffer[i];
		out += ", ";
	}

	if (i != 0)
	{
		out += buffer[i];
		out += "]";
	}

	return out;
}