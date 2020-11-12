#pragma once

#include "core_types.h"

/**
 * Generic atomic operations.
 */
struct GenericPlatformAtomics
{
	/**
	 * Enum that defines the level of
	 * consistency of an atomic operation.
	 */
	enum AtomicOrder
	{
		Relaxed,
		Sequential
	};
};
