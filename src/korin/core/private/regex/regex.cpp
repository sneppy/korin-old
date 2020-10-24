#include "regex/regex.h"

namespace Re
{
	/**
	 * @defgroup RegexSpecialFunctions
	 */
	namespace
	{
		/**
		 * ...
		 * @{
		 */
		constexpr FORCE_INLINE bool isWhiteSpace(ansichar cc)
		{
			return cc == ' ' || cc == '\t' || cc == '\r' || cc == '\n' || cc == '\v' || cc == '\f';
		}

		constexpr FORCE_INLINE bool isDigit(ansichar cc)
		{
			return cc >= '0' && cc <= '9';
		}

		constexpr FORCE_INLINE bool isAlphaLower(ansichar cc)
		{
			return cc >= 'a' && cc <= 'z';
		}

		constexpr FORCE_INLINE bool isAlphaUpper(ansichar cc)
		{
			return cc >= 'A' && cc <= 'Z';
		}

		constexpr FORCE_INLINE bool isAlpha(ansichar cc)
		{
			return isAlphaLower(cc) || isAlphaLower(cc);
		}

		constexpr FORCE_INLINE bool isWord(ansichar cc)
		{
			return isDigit(cc) || isAlpha(cc) || cc == '_';
		}
		/** @} */
	} // namespace

	/**
	 * Parse an escape sequence and
	 * updates builder accordingly.
	 * Returns number of read
	 * characters.
	 * 
	 * @param builder ref to
	 * 	automaton builder
	 * @param sequence sequence string
	 * 	(which includes '\')
	 * @return number of read
	 * 	characters (excluding '\')
	 */
	sizet parseEscapeSequence(Regex::BuilderT & builder, const ansichar * sequence)
	{
		using SymbolT = Regex::SymbolT;
		using LambdaT = Regex::LambdaT;

		sizet idx = 0;

		CHECK(sequence[idx] == '\\');

		// Switch on second symbol
		switch (sequence[++idx])
		{
			case 'b':
			{
				// Matches, without consuming, a word
				// boundary (e.g. `Hello>|<, world!`)
				builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 numRead) -> bool {

					outNumRead = 0;
					return isWord(*input) && (numRead == 0 || !isWord(*(input - 1))) || !isWord(*input) && (numRead > 0 && isWord(*(input - 1)));
				}, "WordBoundaries");
				break;
			}

			case 'B':
			{
				// Matches, without consuming, a position
				// between two word characters
				builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 numRead) -> bool {

					outNumRead = 0;
					return isWord(*input) && (numRead > 0 && isWord(*(input - 1)));
				}, "NonWordBoundaries");
				break;
			}

			case 'd':
			{
				// Matches any single digit character
				builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 /* numRead */) -> bool {

					return outNumRead = isDigit(*input);
				}, "Digit");
				break;
			}

			case 'D':
			{
				// Matches any single non-digit character
				builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 /* numRead */) -> bool {

					return outNumRead = !isDigit(*input);
				}, "NonDigit");
				break;
			}

			case 's':
			{
				// Matches any single whitespace character
				builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 /* numRead */) -> bool {

					return outNumRead = isWhiteSpace(*input);
				}, "WhiteSpace");
				break;
			}

			case 'S':
			{
				// Matches any single non-whitespace character
				builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 /* numRead */) -> bool {

					return outNumRead = !isWhiteSpace(*input);
				}, "NonWhiteSpace");
				break;
			}

			case 'w':
			{
				// Matches any single word character (i.e. [a-zA-Z0-9_])
				builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 /* numRead */) -> bool {

					return outNumRead = isWord(*input);
				}, "Word");
				break;
			}

			case 'W':
			{
				// Matches any single non-word character
				builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 /* numRead */) -> bool {

					return outNumRead = !isWord(*input);
				}, "NonWord");
				break;
			}

			default:
			{
				// Escaped special character
				builder.pushState<SymbolT>(sequence[idx]);
				break;
			}
		}

		return idx;
	}
	

	void Regex::compile(const ansichar * pattern, sizet patternLen)
	{
		// Create new builder
		BuilderT builder = automaton.createBuilder();

		for (sizet idx = 0; idx < patternLen; ++idx)
		{
			switch (pattern[idx])
			{
				// TODO: Match nth subpattern, `\1`
				// TODO: Unicode property or script category `\p`
				// TODO: Negation of `\p`, `\P`
				// TODO: Match subpattern <name>, `\k<name>`
				// TODO: Hex character YYYY, `\uYYYY`
				// TODO: Hex character XX, `\u`
				// TODO: Octal character ddd, `\ddd`
				// TODO: Control character Y, `\cY`
				// TODO: Backspace character, `[\b]`
				// TODO: Exactly 3 of a, `a{3}`
				// TODO: 3 or more of a, `a{3,}`
				// TODO: Between 3 and 6 of a, `a{3, 6}`
				// TODO: Greedy quantifier, `a*`
				// TODO: Lazy quantifier, `a*?`
				// TODO: Capture everything enclosed, `(...)`
				// TODO: Match everything enclosed, `(?:...)`
				// TODO: Named Capturing Group, `(?<name>...)`
				// TODO: Positive Lookahed, `(?=...)`
				// TODO: Negative Lookahead, `(?!...)`
				// TODO: Positive Lookbehind, `(?<=...)`
				// TODO: Negative Lookbehind, `(?<!...)`
				// TODO: A character except: a, b or c, `[^abc]`
				case '(':
				{
					builder.beginGroup();
					break;
				}

				case ')':
				{
					builder.endGroup();
					break;
				}

				case '[':
				{
					// Inverted flag, false by default
					bool invertedFlag = false;

					// Get index of open and close brackets
					sizet openBracketIdx = idx;
					sizet closeBracketIdx = idx;
					for (; (pattern[closeBracketIdx] != ']' || pattern[closeBracketIdx - 1] == '\\') && closeBracketIdx < patternLen ; ++closeBracketIdx);
					
					if (closeBracketIdx == patternLen)
					{
						// If no close bracket found, interpret
						// this as square bracket symbol, and
						// throw a warning
						// TODO: Throw warning
						builder.pushState<SymbolT>('[');
						break;
					}

					if (openBracketIdx + 1 == closeBracketIdx)
					{
						// If square bracket group is empty
						// just ignore it, and throw a warning
						// TODO: Throw warning
						idx = closeBracketIdx;
						break;
					}

					// Check if NOT operation
					if (pattern[++idx] == '^')
					{
						if (idx + 1 == closeBracketIdx)
						{
							// `[^]` is equivalent to a ANY state
							builder.pushState<AnySymbolT>();
							idx = closeBracketIdx;
							break;
						}
						else
						{
							// TODO: Not implemented at the moment, just skip
							// Set inverted flag
							invertedFlag = true;
							++idx;
						}
					}

					// Start group
					builder.beginGroup();

					for (;;)
					{
						if (pattern[idx] == '\\')
						{
							// Parse escape sequence
							idx += parseEscapeSequence(builder, pattern + idx) + 1;
						}
						else if (pattern[idx + 1] == '-' && pattern[idx + 2] != ']')
						{
							// Range specifier
							CHECKF(pattern[idx] < pattern[idx + 2], "Character range out of order");
							builder.pushState<RangeT>(pattern[idx], pattern[idx + 2]);
							idx += 3;
						}
						else
						{
							// Non range, consume only one symbol
							builder.pushState<SymbolT>(pattern[idx]);
							++idx;
						}

						if (idx != closeBracketIdx)
						{
							// Push new branch
							builder.pushBranch();
						}
						else break;
					}

					// Sanity check
					CHECK(pattern[idx] == ']')

					// End group
					builder.endGroup();

					break;
				}

				case '|':
				{
					builder.pushBranch();
					break;
				}

				case '?':
				{
					builder.pushSkip();
					break;
				}

				case '+':
				{
					builder.pushJump();
					break;
				}

				case '*':
				{
					// Essentially a combination of
					// jump and skip
					builder.pushSkip();
					builder.pushJump();
					break;
				}

				case '.':
				{
					// Matches any character
					builder.pushState<AnySymbolT>();
					break;
				}

				case '^':
				{
					// Matches beginning of string
					builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 numRead) -> bool {

						outNumRead = 0;
						return numRead == 0;
					});
					break;
				}

				case '$':
				{
					// Matches end of the string
					builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead, int32 /* numRead */) -> bool {

						outNumRead = 0;
						return *input == '\0';
					});
					break;
				}

				case '\\':
				{
					idx += parseEscapeSequence(builder, pattern + idx);
					break;
				}
				
				default:
				{
					builder.pushState<SymbolT>(pattern[idx]);
					break;
				}
			}
		}

		// End main group
		builder.endGroup();

		// TODO: Move in own function
		OptimizerT optimizer{automaton};
		optimizer.removeEpsilons();
	}
} // namespace Re