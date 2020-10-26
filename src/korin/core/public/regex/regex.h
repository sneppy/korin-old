#pragma once

#include "core_types.h"
#include "./regex_types.h"
#include "./automaton.h"



/**
 * @defgroup Re
 * Regex namespace. Contains the
 * Regex class and free fucntions
 * to create, manipulate and user
 * regular expressions.
 */
namespace Re
{
	/**
	 * Implementation of a regular expression
	 * class, uing a non-determinstic finite
	 * automaton.
	 * 
	 * Example:
	 * ```cpp
	 * Regex regex{"https?://\\w+(\\.\\w+)+[/\\w+]*"};
	 * regex.match("https://www.google.com"); // True
	 * ```
	 * 
	 * The reference flavor at the moment is
	 * ECMAScript (JavaScript) regular
	 * expressions. In the future, it will
	 * support multiple flavors.
	 */
	class Regex
	{
		using AutomatonT = Automaton<ansichar>;
		using StateT = typename AutomatonT::StateT;
		using State = typename AutomatonT::State;
		using BuilderT = typename AutomatonT::BuilderT;
		using OptimizerT = typename AutomatonT::OptimizerT;

		friend sizet parseEscapeSequence(BuilderT&, const ansichar*);

		/**
		 * Compile this regex using the
		 * provided pattern string.
		 * 
		 * @param pattern regex pattern
		 * 	string
		 * @param patternLen length in
		 * 	Bytes of the pattern
		 */
		void compile(const char * pattern, sizet patternLen);

		/**
		 * Default constructor, creates an
		 * empty regex that matches nothing.
		 */
		FORCE_INLINE Regex()
			: automaton{}
		{
			//
		}

	public:
		/**
		 * Creates a new regex with the
		 * provided pattern.
		 * 
		 * @param inPattern pattern string
		 */
		FORCE_INLINE Regex(const String & inPattern)
			: Regex{}
		{
			// Compile regex into automaton
			compile(*inPattern, inPattern.getLength());
		}

		/**
		 * Returns ref to automaton.
		 * @{
		 */
		FORCE_INLINE const AutomatonT & getAutomaton() const
		{
			return automaton;
		}

		FORCE_INLINE AutomatonT & getAutomaton()
		{
			return automaton;
		}
		/** @} */

		/**
		 * Returns true if the regex
		 * matches the entire string.
		 * 
		 * Example:
		 * ```
		 * /Hello, world!/ accepts "Hello, world!" = true
		 * /world/ accepts "Hello, world!" = false
		 * /Hello/ accepts "Hello, world!" = false
		 * ```
		 * @{
		 */
		FORCE_INLINE bool accept(const ansichar * input) const
		{
			return automaton.acceptString(input);
		}

		FORCE_INLINE bool accept(const String & input) const
		{
			return accept(*input);
		}
		/** @} */

	protected:
		/// NFA instance
		AutomatonT automaton;	
	};

	/**
	 * Set of static methods that
	 * construct a regex from the
	 * pattern in place and return
	 * the result of the related
	 * method.
	 * 
	 * @param pattern pattern used
	 * 	to build the regex
	 */
	/**
	 * @see Regex::accept
	 * @param input input string
	 * @return true if the entire
	 * 	input string is recognized
	 * 	by the automaton
	 */
	FORCE_INLINE bool accept(const String & pattern, const String & input)
	{
		return Regex{pattern}.accept(input);
	}
	/** @} */
} // namespace Re
