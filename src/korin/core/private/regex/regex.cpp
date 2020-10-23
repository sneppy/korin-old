#include "regex/regex.h"

namespace Re
{
	void Regex::compile(const ansichar * pattern, sizet patternLen)
	{
		// Create new builder
		BuilderT builder = automaton.createBuilder();

		for (sizet idx = 0; idx < patternLen; ++idx)
		{
			switch (pattern[idx])
			{
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

				case '+':
				{
					builder.pushJump();
					break;
				}

				case '.':
				{
					builder.pushState<AnySymbolT>();
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