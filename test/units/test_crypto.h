#pragma once

#include "gtest/gtest.h"

#include "crypto/crypto.h"

TEST(crypto, lfsr)
{
	using namespace Crypto;

	static constexpr auto maxIter = UINT32_MAX;
	
	for (uint8 size = 2; size <= 16; ++size)
	{
		LFSR lfsr{size, static_cast<uint64>(rand())};
		const auto initialState = lfsr.getState();
		lfsr.shift();

		uint32 iter = 0; for (; lfsr.getState() != initialState && iter < maxIter; ++iter, lfsr.shift());
		ASSERT_EQ(iter + 1, (1u << size) - 1);
	}
}
