#pragma once

#include "generic/generic_platform_atomics.h"
#include "templates/enable_if.h"
#include "templates/types.h"

/**
 * Unix specific atomic operations
 */
struct UnixPlatformAtomics : public GenericPlatformAtomics
{
	/**
	 * 
	 */
	template<AtomicOrder>
	struct GetPlatformAtomicOrder
	{
		//
	};

	/**
	 * 
	 */
	template<typename AtomicT, typename IntT>
	static FORCE_INLINE AtomicT add(volatile AtomicT * value, IntT amount)
	{
		return __sync_fetch_and_add(value, amount);
	}

	/**
	 * 
	 */
	template<typename AtomicT, typename IntT>
	static FORCE_INLINE AtomicT sub(volatile AtomicT * value, IntT amount)
	{
		return __sync_fetch_and_sub(value, amount);
	}

	/**
	 * 
	 */
	template<typename AtomicT, typename IntT>
	static FORCE_INLINE AtomicT exchange(volatile AtomicT * value, IntT other)
	{
		return __sync_lock_test_and_set(value, other);
	}

	/**
	 * 
	 */
	template<AtomicOrder order = AtomicOrder::Sequential, typename AtomicT>
	static FORCE_INLINE AtomicT read(volatile const AtomicT * dst)
	{
		AtomicT out{};
		__atomic_load(const_cast<volatile AtomicT*>(dst), &out, GetPlatformAtomicOrder<order>::value);
		return out;
	}

	/**
	 * 
	 */
	template<AtomicOrder order = AtomicOrder::Sequential, typename AtomicT, typename IntT>
	static FORCE_INLINE void store(volatile AtomicT * dst, IntT src)
	{
		__atomic_store(dst, &src, GetPlatformAtomicOrder<order>::value);
	}
};


template<> struct UnixPlatformAtomics::GetPlatformAtomicOrder<UnixPlatformAtomics::AtomicOrder::Sequential>	{ enum {value = __ATOMIC_SEQ_CST}; };
template<> struct UnixPlatformAtomics::GetPlatformAtomicOrder<UnixPlatformAtomics::AtomicOrder::Relaxed>	{ enum {value = __ATOMIC_RELAXED}; };
