#pragma once

#include "gtest/gtest.h"

#include "templates/types.h"
#include "templates/atomic.h"
#include "templates/optional.h"

TEST(templates, types)
{
	// Same type
	ASSERT_TRUE((IsSameType<void, void>::value));
	ASSERT_TRUE((IsSameType<int*, int*>::value));
	ASSERT_FALSE((IsSameType<float, double>::value));
	ASSERT_FALSE((IsSameType<int, const int>::value));

	// Is pointer
	ASSERT_TRUE((IsPointer<void*>::value));
	ASSERT_TRUE((IsPointer<void**>::value));
	ASSERT_FALSE((IsPointer<int&>::value));

	// Is reference
	ASSERT_TRUE((IsReference<float&>::value));
	ASSERT_TRUE((IsReference<const double&>::value));
	ASSERT_FALSE((IsReference<int*>::value));

	// Is integral
	ASSERT_TRUE((IsIntegral<int32>::value));
	ASSERT_TRUE((IsIntegral<sizet>::value));
	ASSERT_FALSE((IsIntegral<float32>::value));

	// Remove pointer
	ASSERT_TRUE((IsSameType<void, RemovePointer<void*>::Type>::value));
	ASSERT_TRUE((IsSameType<void*, RemovePointer<void**>::Type>::value));
	ASSERT_FALSE((IsSameType<void*, RemovePointer<void***>::Type>::value));
	
	// Remove all pointers
	ASSERT_TRUE((IsSameType<void, RemoveAllPointers<void***>::Type>::value));

	// Remove reference
	ASSERT_TRUE((IsSameType<short, RemoveReference<short&>::Type>::value));
	ASSERT_TRUE((IsSameType<short, RemoveReference<short&&>::Type>::value));

	// Remove const qualifier
	ASSERT_TRUE((IsSameType<int, RemoveConst<const int>::Type>::value));
	ASSERT_TRUE((IsSameType<int*, RemoveConst<int* const>::Type>::value));
	ASSERT_FALSE((IsSameType<int&, RemoveConst<const int&>::Type>::value));
	ASSERT_FALSE((IsSameType<int*, RemoveConst<const int*>::Type>::value));
}

TEST(templates, atomic)
{
	Atomic<int32> a = 2;

	ASSERT_EQ(a.load(), 2);

	a.store(10);

	ASSERT_EQ(a.load(), 10);

	a = 0;

	ASSERT_EQ(a.load(), 0);
	
	a = 0;

	ASSERT_EQ(++a, 1);
	ASSERT_EQ(a.load(), 1);
	ASSERT_EQ(a++, 1);
	ASSERT_EQ(a.load(), 2);
	
	a = 0;

	ASSERT_EQ(--a, -1);
	ASSERT_EQ(a.load(), -1);
	ASSERT_EQ(a--, -1);
	ASSERT_EQ(a.load(), -2);

	a = 0;

	ASSERT_EQ(a += 5, 5);
	ASSERT_EQ(a.load(), 5);

	a = 0;

	ASSERT_EQ(a -= 5, -5);
	ASSERT_EQ(a.load(), -5);

	a = 11;

	ASSERT_EQ(static_cast<int32>(a), 11);

	SUCCEED();
}

TEST(templates, optional)
{
	Optional<int32> a;

	ASSERT_FALSE(a.hasValue());
	ASSERT_FALSE(static_cast<bool>(a));

	a.emplace();

	ASSERT_TRUE(a.hasValue());
	ASSERT_TRUE(static_cast<bool>(a));
	
	a.reset();

	ASSERT_FALSE(a.hasValue());
	ASSERT_FALSE(static_cast<bool>(a));

	Optional<int32> b{3};

	ASSERT_TRUE(b.hasValue());
	ASSERT_TRUE(static_cast<bool>(b));
	ASSERT_EQ(*b, 3);

	Optional<int32> c{a};
	Optional<int32> d{b};

	ASSERT_FALSE(c.hasValue());
	ASSERT_FALSE(static_cast<bool>(c));
	ASSERT_TRUE(d.hasValue());
	ASSERT_TRUE(static_cast<bool>(d));
	ASSERT_EQ(*d, 3);

	a = b;
	b = c;
	c = d;

	ASSERT_TRUE(a.hasValue());
	ASSERT_TRUE(static_cast<bool>(a));
	ASSERT_EQ(*a, 3);
	ASSERT_FALSE(b.hasValue());
	ASSERT_FALSE(static_cast<bool>(b));
	ASSERT_TRUE(c.hasValue());
	ASSERT_TRUE(static_cast<bool>(c));
	ASSERT_EQ(*c, 3);
	ASSERT_TRUE(d.hasValue());
	ASSERT_TRUE(static_cast<bool>(d));
	ASSERT_EQ(*d, 3);

	b = 9;
	c = 1;

	ASSERT_TRUE(b.hasValue());
	ASSERT_TRUE(static_cast<bool>(b));
	ASSERT_EQ(*b, 9);
	ASSERT_TRUE(c.hasValue());
	ASSERT_TRUE(static_cast<bool>(c));
	ASSERT_EQ(*c, 1);

	SUCCEED();
}
