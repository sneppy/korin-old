#pragma once

#include <gtest/gtest.h>

#include "templates/types.h"

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
	ASSERT_TRUE((IsSameType<void, typename RemovePointer<void*>::Type>::value));
	ASSERT_TRUE((IsSameType<void*, typename RemovePointer<void**>::Type>::value));
	ASSERT_FALSE((IsSameType<void*, typename RemovePointer<void***>::Type>::value));
	
	// Remove all pointers
	ASSERT_TRUE((IsSameType<void, typename RemoveAllPointers<void***>::Type>::value));

	// Remove reference
	ASSERT_TRUE((IsSameType<short, typename RemoveReference<short&>::Type>::value));
	ASSERT_FALSE((IsSameType<short, typename RemoveReference<short&&>::Type>::value));
}