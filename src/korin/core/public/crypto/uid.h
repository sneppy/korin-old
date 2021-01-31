#pragma once

#include "core_types.h"
#include "hal/platform_time.h"
#include "templates/types.h"
#include "./crypto_types.h"
#include "./lfsr.h"

namespace Crypto
{
	/**
	 * @brief A Unique Identifier (UID). 
	 */
	struct UID
	{
	public:
		/**
		 * @brief Initializes timestamp and identifiers
		 * to zero.
		 */
		constexpr FORCE_INLINE UID()
			: value{0ull}
		{
			//
		}

		/**
		 * @brief Initialize identifier and timestamp
		 * with the given values.
		 * 
		 * @param inIdentifier given unique identifier
		 * @param inTimestamp UID timestamp
		 */
		constexpr FORCE_INLINE explicit UID(uint32 inIdentifier, uint32 inTimestamp)
			: timestamp{inTimestamp}
			, identifier{inIdentifier}
		{
			//
		}

		/**
		 * @brief Initialize identifier and use
		 * current timestamp.
		 * 
		 * @param inIdentifier given unique identifier 
		 */
		FORCE_INLINE explicit UID(uint32 inIdentifier)
			: UID{inIdentifier, static_cast<uint32>(PlatformTime::getCurrentTimestamp())}
		{
			//
		}

		/**
		 * @brief Returns complete value. 
		 */
		constexpr FORCE_INLINE const uint64 & getValue() const
		{
			return value;
		}

		/**
		 * @brief Returns true if identifier is non-zero.
		 */
		constexpr FORCE_INLINE bool isValid() const
		{
			return identifier != 0x0;
		}

		/**
		 * @brief Returns true if two UIDs are equal.
		 * 
		 * @param other other UID
		 * @return true if same UID 
		 */
		constexpr FORCE_INLINE bool operator==(const UID & other) const
		{
			return value == other.value;
		}

		/**
		 * @brief Returns true if two UIDs differ.
		 * 
		 * @param other other UID
		 * @return true if not same UID
		 */
		constexpr FORCE_INLINE bool operator!=(const UID & other) const
		{
			return !(*this == other);
		}

		/**
		 * @brief Returns true if this UID was
		 * generated before the other.
		 * 
		 * @param other other UID
		 * @returns true if timestamp preceeds
		 * other timestamp
		 */
		constexpr FORCE_INLINE bool operator<(const UID & other) const
		{
			return timestamp < other.timestamp;
		}

		/**
		 * @brief Returns true if this UID was
		 * generated after the other.
		 * 
		 * @param other other UID
		 * @returns true if timestamp is older
		 * than the other timestamp
		 */
		constexpr FORCE_INLINE bool operator>(const UID & other) const
		{
			return timestamp > other.timestamp;
		}

	private:
		union
		{
			struct
			{
				/// Unix timestamp
				uint32 timestamp;

				/// Unique identifier
				uint32 identifier;
			};

			/// UID value as a 64-bit unsigned integer
			uint64 value;
		};
	};

	/**
	 * @brief A UID generator, generates up-to $2^32 - 1$
	 * different uids for a given timestamp. 
	 */
	class UIDGenerator
	{
	public:
		/**
		 * @brief Initialize UID generator with the
		 * given seed. 
		 */
		constexpr FORCE_INLINE explicit UIDGenerator(uint32 inSeed)
			: lfsr{idSize, inSeed}
		{
			//
		}

		/**
		 * @brief Initialize UID generator using
		 * current timestamp. 
		 */
		FORCE_INLINE UIDGenerator()
			: UIDGenerator{static_cast<uint32>(PlatformTime::getCurrentTimestamp())}
		{
			//
		}

		/**
		 * @brief Generates and returns new UID.
		 */
		FORCE_INLINE UID next()
		{
			const uint32 identifier = static_cast<uint32>(lfsr.shift(), lfsr.getState());
			return UID{identifier, static_cast<uint32>(PlatformTime::getCurrentTimestamp())};
		}

	private:
		/// LFSR used to generate the unique identifiers
		LFSR lfsr;

		/// Register size, 32-bit
		static constexpr uint32 idSize = 32;
	};
} // namespace Crypto
