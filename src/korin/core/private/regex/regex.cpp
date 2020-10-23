#include "regex/regex.h"

namespace Re
{
	namespace
	{
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
	} // namespace
	

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
				// TODO: A single character of: a, b or c, `[abc]`
				// TODO: A character except: a, b or c, `[^abc]`
				// TODO: A character in the range: a-z, `[a-z]`
				// TODO: A character not in the range: a-z, `[^a-z]`
				// TODO: A character in the range: a-z or A-Z, `[a-zA-Z]`
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
					builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

						outNumRead = 0;
						return false; // FIXME: We need num of read characters
					});
					break;
				}

				case '$':
				{
					// Matches end of the string
					builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

						outNumRead = 0;
						return false; // FIXME: We need num of read characters
					});
					break;
				}

				case '\\':
				{
					// Switch on second symbol
					switch (pattern[++idx])
					{
						case 'b':
						{
							// Matches, without consuming, a word
							// boundary (e.g. `Hello>|<, world!`)
							builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

								outNumRead = 0;
								return false; // FIXME: We need the number of characters read
							});
							break;
						}

						case 'B':
						{
							// Matches, without consuming, a position
							// between two word characters
							builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

								outNumRead = 0;
								return false; // FIXME: We need the number of characters read
							});
							break;
						}

						case 'd':
						{
							// Matches any single digit character
							builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

								return outNumRead = isDigit(*input);
							});
							break;
						}

						case 'D':
						{
							// Matches any single non-digit character
							builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

								return outNumRead = !isDigit(*input);
							});
							break;
						}

						case 's':
						{
							// Matches any single whitespace character
							builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

								return outNumRead = isWhiteSpace(*input);
							});
							break;
						}

						case 'S':
						{
							// Matches any single non-whitespace character
							builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

								return outNumRead = !isWhiteSpace(*input);
							});
							break;
						}

						case 'w':
						{
							// Matches any single word character (i.e. [a-zA-Z0-9_])
							builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

								return outNumRead = isWord(*input);
							});
							break;
						}

						case 'W':
						{
							// Matches any single non-word character
							builder.pushState<LambdaT>([](const ansichar * input, int32 & outNumRead) -> bool {

								return outNumRead = !isWord(*input);
							});
							break;
						}

						default:
						{
							// Escaped special character
							builder.pushState<SymbolT>(pattern[idx]);
							break;
						}
					}
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