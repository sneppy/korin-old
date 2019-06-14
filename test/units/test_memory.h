#pragma once

#include <gtest/gtest.h>

#include "hal/platform_memory.h"
#include "hal/malloc_ansi.h"

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
}