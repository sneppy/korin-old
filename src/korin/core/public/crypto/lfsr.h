#pragma once

#include "core_types.h"
#include "hal/platform_math.h"
#include "./crypto_types.h"
#include "./crypto_utils.h"

namespace Crypto
{
	/**
	 * @brief Straigth-forward implementation of
	 * a Left Feedback Shift Register (LFSR).
	 */
	class LFSR
	{
	public:
		/**
		 * @brief Initialize register with state.
		 * 
		 * @param inSize register size
		 * @param inState initial state
		 */
		constexpr FORCE_INLINE explicit LFSR(uint8 inSize, uint64 inState = 0ull)
			: state{inState}
			, size{inSize}
		{
			//
		}

		/**
		 * @brief Returns register state 
		 */
		constexpr FORCE_INLINE uint64 getState() const
		{
			const uint64 mask = (1ull << size) - 1;
			return state & mask;
		}

		/**
		 * @brief Shift register once, returns shifted bit.
		 */
		constexpr FORCE_INLINE ubyte shift()
		{
			// Store shifted away bit in lsb
			ubyte res = state >> (size - 1);

			// Compute feedback bit and push to state
			const uint64 fdbk = PlatformMath::parity(state & Utils::primitivePolys[size - 1]);
			state = (state << 1ull) | (fdbk & 0x1);

			return res & 0x1;
		}

		/**
		 * @brief Shift register n times.
		 * 
		 * @param numShifts number of shifts
		 * @return ref to self
		 */
		constexpr FORCE_INLINE LFSR & operator<<=(uint8 numShifts)
		{
			for (; numShifts > 0; --numShifts) shift();
			return *this;			
		}

	private:
		/// Register state
		uint64 state;

		/// Register size (max. 64-bits)
		uint8 size;
	};
} // namespace Crypto
