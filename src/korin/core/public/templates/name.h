#pragma once

#include "core_types.h"
#include "./types.h"
#include "./array.h"

/**
 * A name is a static string which
 * can be manipulated at compile
 * time.
 */
class Name
{
	using CharT = ansichar;
	template<sizet length> using LiteralT = const CharT[length];
	template<uint64 count> using StaticArrayT = StaticArray<CharT, count>;

	Name() = delete;	

public:
	/**
	 * Accepts a string and a length
	 * in Bytes.
	 * 
	 * @param inBuffer pointer to
	 * 	string buffer
	 * @param inLength length of the
	 * 	string in Bytes
	 */
	constexpr Name(const CharT * inBuffer, sizet inLength)
		: buffer{inBuffer}
		, length{inLength}
	{
		//
	}

	/**
	 * Constructor that accepts a string
	 * literal.
	 * 
	 * Example:
	 * 
	 * ```
	 * constexpr Name typeName = "Array";
	 * ```
	 * 
	 * @param inLiteralLength length of
	 * 	the literal (includes '\0')
	 * @param inLiteral literal address
	 */
	template<sizet inLength>
	constexpr Name(LiteralT<inLength> & inLiteral)
		: buffer{inLiteral}
		, length{inLength - 1}
	{
		//
	}

	/**
	 * Construct a name from a static
	 * array object.
	 * 
	 * @param inCount size of the
	 * 	array (includes '\0')
	 * @param inArray static array
	 */
	template<uint64 inCount>
	constexpr explicit Name(const StaticArrayT<inCount> & inArray)
		: buffer{*inArray}
		, length{inArray.getCount() - 1}
	{
		//
	}

	/**
	 * Get pointer to buffer.
	 */
	constexpr FORCE_INLINE const CharT * const operator*() const
	{
		return buffer;
	}

	/**
	 * 
	 */
	constexpr FORCE_INLINE sizet getLength() const
	{
		return length;
	}

	/**
	 * 
	 */
	constexpr FORCE_INLINE const CharT & operator[](sizet idx) const
	{
		return buffer[idx];
	}

	/**
	 * Utility for compile-time
	 * concatenation of names.
	 * 
	 * @param names list of names
	 * 	to concatenate
	 */
	template<const Name & ...names>
	struct Concat
	{
		static constexpr auto concat() noexcept
		{
			StaticArrayT<length + 1> buffer{};

			auto copyBuffer = [i = 0, &buffer](auto const & name) mutable {

				for (sizet idx = 0; idx < name.getLength(); ++idx)
				{
					buffer[i++] = name[idx];
				}
			};
			(copyBuffer(names), ...);

			return buffer;
		}

		/// Output name length
		static constexpr sizet length = (names.getLength() + ... + 0);
		
		/// Output static buffer
		static constexpr auto buffer = concat();

		/// Output name object
		static constexpr Name name{buffer};
	};

protected:
	/// Pointer to static buffer
	const ansichar * buffer;

	/// Text length
	const sizet length;
};

/**
 * Quick macro for String concatenation.
 */
#define JOIN_NAME(...) Name::Concat<__VA_ARGS__>::name
