#pragma once

#include "core_types.h"
#include "containers/string.h"

/**
 * @defgroup Re
 */
namespace Re
{
	class Regex;

	template<typename> class Automaton;
	template<typename> class Executor;

	template<typename> struct StateBase;
	template<typename> struct StateNull;
	template<typename> struct StateEpsilon;
	template<typename> struct StateAny;
	template<typename> struct StateSymbol;
	template<typename> struct StateString;
	template<typename> struct StateRange;
	template<typename> struct StateLambda;
	template<typename> struct StateMacro;
	template<typename> struct StatePositiveLookahed;
	template<typename> struct StateNegativeLookahed;

	/**
	 * Print a graph of the states
	 * spawning from the start state.
	 * 
	 * @param startState starting state
	 * @param acceptedState optional
	 * 	accepted state
	 * @return string that represent
	 * 	state graph
	 */
	template<typename AlphaT>
	String printStateGraph(const StateBase<AlphaT> * startState, const StateBase<AlphaT> * acceptedState = nullptr);

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

		/// Terminal symbol
		static constexpr AlphaT terminalSymbol{};

		/// Empty string
		static constexpr const AlphaT emptyString[1] = {};

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
	struct AlphabetTraits<ansichar>
	{
		/// Alphabet symbol type
		using SymbolT = ansichar;

		// Alphabet string type
		using StringT = const ansichar*;

		/// Terminal symbol
		static constexpr ansichar terminalSymbol = '\0';

		/// Empty string
		static constexpr const ansichar * emptyString = "";

		/**
		 * 
		 */
		static FORCE_INLINE const ansichar * consumeInput(const ansichar * input, int32 numRead)
		{
			return input + numRead;
		}

		/**
		 * 
		 */
		static FORCE_INLINE bool isEOF(const ansichar * input)
		{
			return *input == '\0';
		}
	};
} // namespace Re
