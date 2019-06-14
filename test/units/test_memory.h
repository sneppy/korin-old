#pragma once

#include <gtest/gtest.h>

#include "hal/platform_memory.h"
#include "hal/malloc_ansi.h"
#include "hal/malloc_object.h"

TEST(memory, malloc_ansi)
{
	void * dst = nullptr, * src = nullptr;
	MallocAnsi * malloc = new MallocAnsi;

	// Do some allocations
	dst = malloc->alloc(4096, 32);
	src = malloc->alloc(8192, 16);
	
	ASSERT_TRUE(dst != nullptr);
	ASSERT_TRUE(src != nullptr);

	malloc->free(dst);
	malloc->free(src);

	// Check alignment
	dst = malloc->alloc(65536, 1u << 10);
	ASSERT_EQ(reinterpret_cast<uintp>(dst) & ((1u << 10) - 1), 0x0);

	dst = malloc->realloc(dst, 65536, 1u << 16);
	ASSERT_EQ(reinterpret_cast<uintp>(dst) & ((1u << 16) - 1), 0x0);

	SUCCEED();
}

TEST(memory, malloc_object)
{
	struct Foo
	{
		uint32 a;
		float32 b;
		void * something;
	} * foo;

	MallocObject<Foo, MallocAnsi> * malloc = new MallocObject<Foo, MallocAnsi>(new MallocAnsi);

	// Do some allocations
	foo = malloc->alloc(1);
	foo->a = 1;
	foo->b = 3.14f;

	foo = malloc->realloc(foo, 16);
	foo[0].a = 2;
	foo[10].something = foo;

	malloc->free(foo);

	SUCCEED();
}