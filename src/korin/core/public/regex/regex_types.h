#pragma once

namespace NFA
{
	template<typename> struct StateBase;
	template<typename> struct StateNull;
	template<typename> struct StateEpsilon;
	template<typename> struct StateAny;
	template<typename> struct StateSymbol;
	template<typename> struct StateString;

	template<typename> class Automaton;
} // namespace NFA

class Regex;

/**
 * A struct that contains informations
 * about a certain alphabet. There
 * must be a template specialization
 * for each alphabet type that needs
 * to be used.
 * 
 * @param AlphaT the type of the basic
 * 	symbol of the alphabet
 */
template<typename AlphaT>
struct AlphabetTraits
{
	/// Alphabet symbol type
	using SymbolT = AlphaT;

	/// Alphabet string type
	using StringT = const AlphaT*;

	/**
	 * 
	 */
	static StringT consumeInput(const StringT & input, int32 numRead);

	/**
	 * 
	 */
	static bool isEOF(const StringT & input);
};

/**
 * Alphabet specialization for char
 * type.
 */
template<>
struct AlphabetTraits<char>
{
	/// Alphabet symbol type
	using SymbolT = char;

	// Alphabet string type
	using StringT = const char*;

	/// Terminal symbol
	static constexpr char terminalSymbol = '\0';

	/// Empty string
	static constexpr const char * emptyString = "";

	/**
	 * 
	 */
	static FORCE_INLINE const char * consumeInput(const char * input, int32 numRead)
	{
		return input + numRead;
	}

	/**
	 * 
	 */
	static FORCE_INLINE bool isEOF(const char * input)
	{
		return *input == '\0';
	}
};
