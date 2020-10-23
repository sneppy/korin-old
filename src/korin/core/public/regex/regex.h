#pragma once

#include "core_types.h"
#include "./regex_types.h"
#include "./automaton.h"

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
 */
class Regex
{
	using AutomatonT = NFA::Automaton<ansichar>;
	using StateT = typename AutomatonT::StateT;
	using EpsilonT = typename AutomatonT::EpsilonT;
	using SymbolT = typename AutomatonT::SymbolT;
	using AnySymbolT = typename AutomatonT::AnyT;
	using BuilderT = typename AutomatonT::BuilderT;
	using OptimizerT = typename AutomatonT::OptimizerT;

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