#pragma once

#include "gtest/gtest.h"

#include "regex/automaton.h"
#include "regex/regex.h"

TEST(regex, basic)
{
	{
		Re::Regex regex{"abc"};

		ASSERT_TRUE(regex.accept("abc"));
		ASSERT_FALSE(regex.accept("abcd"));
		ASSERT_FALSE(regex.accept(" abc"));
	}

	{
		Re::Regex regex{".."};

		for (char cc = 32; cc < 127; ++cc)
		{
			ASSERT_FALSE(regex.accept(String{1, cc}));
			ASSERT_TRUE(regex.accept(String{2, cc}));
			ASSERT_FALSE(regex.accept(String{3, cc}));
		}
	}

	SUCCEED();
}

TEST(regex, special_characters)
{
	{
		// Any character
		Re::Regex regex{"."};

		for (char cc = 1; cc < 127; ++cc)
		{
			ASSERT_TRUE(regex.accept(String{1, cc}));
		}

		ASSERT_FALSE(regex.accept("\0"));
	}

	{
		// Digit characters
		Re::Regex regex{"\\d\\d"};

		ASSERT_TRUE(regex.accept("10"));
		ASSERT_TRUE(regex.accept("67"));
		ASSERT_FALSE(regex.accept("ab"));
		ASSERT_FALSE(regex.accept("1"));
	}

	{
		// Word characters
		Re::Regex regex{"\\w"};

		for (char cc = 0; cc < 127; ++cc)
		{
			if ((cc >= 'A' && cc <= 'Z') || (cc >= 'a' && cc <= 'z') || (cc >= '0' && cc <= '9') || cc == '_')
			{
				ASSERT_TRUE(regex.accept(String{1, cc}));
			}
			else
			{
				ASSERT_FALSE(regex.accept(String{1, cc}));
			}
		}
	}

	{
		// Whitespace characters
		Re::Regex regex{"\\s"};

		for (char cc = 0; cc < 127; ++cc)
		{
			if (cc  == ' ' || cc == '\t' || cc == '\r' || cc == '\v' || cc == '\n' || cc == '\f')
			{
				ASSERT_TRUE(regex.accept(String{1, cc}));
			}
			else
			{
				ASSERT_FALSE(regex.accept(String{1, cc}));
			}
		}
	}
}

TEST(regex, quantifiers)
{
	{
		// Match at least one
		Re::Regex regex{"a+"};

		for (sizet num = 1; num < 1000; ++num)
		{
			ASSERT_TRUE(regex.accept(String{num, 'a'}));
			ASSERT_FALSE(regex.accept(String{num, 'b'}));
		}

		ASSERT_FALSE(regex.accept(""));
	}

	{
		Re::Regex regex{"a+b+"};

		ASSERT_TRUE(regex.accept("ab"));
		ASSERT_TRUE(regex.accept("aaabbbb"));
		ASSERT_TRUE(regex.accept("abbb"));
		ASSERT_FALSE(regex.accept("aaaa"));
		ASSERT_FALSE(regex.accept("abba"));
		ASSERT_FALSE(regex.accept("ababab"));
	}

	{
		Re::Regex regex{"a+b*"};

		ASSERT_TRUE(regex.accept("ab"));
		ASSERT_TRUE(regex.accept("aaabbbb"));
		ASSERT_TRUE(regex.accept("abbb"));
		ASSERT_TRUE(regex.accept("aaaa"));
		ASSERT_FALSE(regex.accept("abba"));
		ASSERT_FALSE(regex.accept("ababab"));
	}

	{
		Re::Regex regex{"a{3}"};

		ASSERT_TRUE(regex.accept("aaa"));
		ASSERT_FALSE(regex.accept("aa"));
		ASSERT_FALSE(regex.accept("aaaa"));
	}

	{
		Re::Regex regex{"a{10,}"};
		
		for (sizet i = 0; i < 100; ++i)
		{
			if (i >= 10)
			{
				// Should accept any string which is
				// a multiple of `aaa`
				ASSERT_TRUE(regex.accept(String{i, 'a'}));
			}
			else
			{
				ASSERT_FALSE(regex.accept(String{i, 'a'}));
			}
		}
	}

	{
		Re::Regex regex{"a{10,20}"};
		
		for (sizet i = 0; i < 100; ++i)
		{
			if (i >= 10 && i <= 20)
			{
				// Should accept any string which is
				// a multiple of `aaa`
				ASSERT_TRUE(regex.accept(String{i, 'a'}));
			}
			else
			{
				ASSERT_FALSE(regex.accept(String{i, 'a'}));
			}
		}
	}

	{
		// Not default behaviour, some engines
		// refuse to compile this because they
		// say 'there is nothing to repeat'.
		// I agree to disagree...
		Re::Regex regex{"a{3}+"};
		
		for (sizet i = 1; i < 1000; ++i)
		{
			if (i % 3 == 0)
			{
				// Should accept any string which is
				// a multiple of `aaa`
				ASSERT_TRUE(regex.accept(String{i, 'a'}));
			}
			else
			{
				ASSERT_FALSE(regex.accept(String{i, 'a'}));
			}
		}
	}

	SUCCEED();
}

TEST(regex, groups)
{
	{
		// Match at least one
		Re::Regex regex{"(ab)+"};

		ASSERT_TRUE(regex.accept("ab"));
		ASSERT_FALSE(regex.accept("aaabbbb"));
		ASSERT_FALSE(regex.accept("abbb"));
		ASSERT_FALSE(regex.accept("aaaa"));
		ASSERT_FALSE(regex.accept("abba"));
		ASSERT_TRUE(regex.accept("ababab"));
	}

	{
		// Match at least one
		Re::Regex regex{"(ab){4}"};

		ASSERT_FALSE(regex.accept("ab"));
		ASSERT_FALSE(regex.accept("aaabbbb"));
		ASSERT_TRUE(regex.accept("abababab"));
		ASSERT_FALSE(regex.accept("ababababab"));
	}

	SUCCEED();
}

TEST(regex, character_classes)
{
	{
		Re::Regex regex{"[abc]"};

		for (char cc = 0; cc < 127; ++cc)
		{
			if (cc == 'a' || cc == 'b' || cc == 'c')
			{
				ASSERT_TRUE(regex.accept(String{1, cc}));
			}
			else
			{
				ASSERT_FALSE(regex.accept(String{1, cc}));
			}
		}
	}

	{
		Re::Regex regex{"[a-zA-Z0-9_]"};

		for (char cc = 0; cc < 127; ++cc)
		{
			if ((cc >= 'A' && cc <= 'Z') || (cc >= 'a' && cc <= 'z') || (cc >= '0' && cc <= '9') || cc == '_')
			{
				ASSERT_TRUE(regex.accept(String{1, cc}));
			}
			else
			{
				ASSERT_FALSE(regex.accept(String{1, cc}));
			}
		}
	}

	{
		// Equivalent to any
		Re::Regex regex{"[^]"};

		for (char cc = 1; cc < 127; ++cc)
		{
			ASSERT_TRUE(regex.accept(String{1, cc}));
		}

		ASSERT_FALSE(regex.accept("\0"));
	}
	
	SUCCEED();
}
