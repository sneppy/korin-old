#pragma once

#include <gtest/gtest.h>

#include "core_types.h"
#include "hal/platform_math.h"

TEST(math, general)
{
	// Align down
	ASSERT_EQ(PlatformMath::align2Down(16, 16), 16);
	ASSERT_EQ(PlatformMath::align2Down(15, 8), 8);
	ASSERT_EQ(PlatformMath::align2Down(0, 32), 0);
	ASSERT_EQ(PlatformMath::align2Down(16 - 1, 8), 8);

	// Align up
	ASSERT_EQ(PlatformMath::align2Up(16, 16), 32);
	ASSERT_EQ(PlatformMath::align2Up(15, 8), 16);
	ASSERT_EQ(PlatformMath::align2Up(0, 32), 32);
	ASSERT_EQ(PlatformMath::align2Up(16 - 1, 8), 16);
}