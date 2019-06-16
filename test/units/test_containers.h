#pragma once

#include <gtest/gtest.h>

#include "containers/array.h"
#include "containers/string.h"

#include "hal/malloc_ansi.h"
#include "hal/malloc_pool.h"

TEST(containers, array)
{
	Array<uint64, MallocAnsi> a(0, 0, nullptr);
	Array<uint64, MallocAnsi> b(32, 32, nullptr);
	
	ASSERT_EQ(a.getCount(), 0);
	ASSERT_EQ(b.getCount(), 32);

	uint64 s, r, t;
	r = rand();
	t = a.add(r);

	ASSERT_EQ(a.getCount(), 1);
	ASSERT_EQ(t, r);
	ASSERT_EQ(a[0], t);

	a.add(t);

	Array<uint64, MallocAnsi> c(a);

	ASSERT_EQ(c[0], a[0]);
	ASSERT_EQ(c[1], a[1]);
	ASSERT_EQ(c.getCount(), a.getCount());

	Array<Array<Array<uint64>>> d;

	d.add(Array<Array<uint64>>(16, 8, nullptr));
	d.add(Array<Array<uint64>>(8, 8, nullptr));

	ASSERT_EQ(d.getCount(), 2);
	ASSERT_EQ(d[0].getCount(), 8);
	ASSERT_EQ(d[1].getCount(), 8);

	SUCCEED();
}

TEST(containers, string)
{
	String a = "sneppy";
	String b = "rulez";

	ASSERT_EQ(a.getLength(), 6);
	ASSERT_EQ(b.getLength(), 5);
	ASSERT_EQ(a[6], '\0');
	ASSERT_EQ(b[5], '\0');
	ASSERT_TRUE(strcmp(a.getData(), "sneppy") == 0);
	ASSERT_TRUE(strcmp(b.getData(), "rulez") == 0);

	const String c = a + ' ' + b;
	ASSERT_EQ(c.getLength(), 12);
	ASSERT_EQ(c[12], '\0');
	ASSERT_TRUE(strcmp(c.getData(), "sneppy rulez") == 0);

	SUCCEED();
}