#pragma once

#include "core_types.h"
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
	explicit FORCE_INLINE String(uint64 n = 0)
		: array(n + 1, n, nullptr)
	{
		terminateString();
	}

	/**
	 * Buffer constructor
	 * 
	 * @param [in] src stream of characters
	 * @param [in] len source length (in Bytes)
	 */
	FORCE_INLINE String(const ansichar * src, uint64 n)
		: array(n + 1, n, nullptr)
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
		// TODO: Replace with PlatformString methods
		: array(strlen(src) + 1, 0, nullptr)
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
		: array(other.array)
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
		: array(other.array.count + slack + 1, 0, nullptr)
	{
		array.count = other.array.count;
		Memory::memcpy(array.buffer, other.array.buffer, array.count + 1); // Copy null character
	}

public:
	/**
	 * Move constructor, no copy involved
	 */
	FORCE_INLINE String(String && other)
		: array(move(other.array))
	{
		//
	}

	/**
	 * Returns string length
	 */
	FORCE_INLINE sizet getLength() const
	{
		return array.count;
	}

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
	 * Append a single character to this string
	 * 
	 * @param [in] c character to append
	 * @return self
	 */
	String & operator+=(ansichar c)
	{
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
		// TODO: Replace with PlatformString methods
		const sizet len = strlen(cstr);
		
		array.resizeIfNecessary(array.count + len + 1);
		Memory::memcpy(array.buffer + array.count, cstr, len);
		array.count += len;
		terminateString();

		return *this;
	}

	/**
	 * Append another string at the end of
	 * this string
	 * 
	 * @param [in] other string to append
	 * @return self
	 */
	String & operator+=(const String & other)
	{
		array.resizeIfNecessary(array.count + other.array.count + 1);
		Memory::memcpy(array.buffer + array.count, other.array.buffer, other.array.count);
		array.count += other.array.count;
		terminateString();

		return *this;
	}
	
	METHOD_ALIAS(append, operator+=)

	/**
	 * Concatenates this string with a single
	 * character, generates a new string
	 * 
	 * @param [in] c character to append
	 * @return new string
	 */
	FORCE_INLINE String operator+(ansichar c) const
	{
		String out(*this, 1);
		return move(out += c);
	}

	/**
	 * Concatenates this string with a c string,
	 * generates a new string
	 * 
	 * @param [in] cstr c string to append
	 * @return new string
	 */
	String operator+(const ansichar * cstr) const
	{
		// TODO: Replace with PlatformString methods
		const sizet len = strlen(cstr);
		String out(*this, len);
		Memory::memcpy(out.array.buffer + out.array.count, cstr, len + 1); // Copy null character

		return move(out);
	}

	/**
	 * Concatenates this string with another
	 * string, generates a new string
	 * 
	 * @param [in] other string to append
	 * @return new string
	 */
	FORCE_INLINE String operator+(const String & other) const
	{
		String out(*this, other.array.count);
		return move(out += other);
	}

	METHOD_ALIAS(concat, operator+)
};