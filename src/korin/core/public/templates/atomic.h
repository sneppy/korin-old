#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "hal/platform_atomics.h"
#include "./enable_if.h"

template<typename> struct AtomicBase;
template<typename> struct AtomicIntegral;
template<typename> struct AtomicPointer;

/**
 * Utility to choose atomic class
 * based on atomic type.
 * 
 * @param AtomicT type of the atomic
 * 	value
 * @param inIsIntegral true if type
 * 	or pointer-type is integral
 * @{
 */
template<typename AtomicT, bool inIsVoidPointer, bool inIsIntegral, bool inUsePlatformAtomics>
struct ChooseAtomicType
{
	//
};

template<typename T> struct ChooseAtomicType<T, false, true, true>		{ using Type = AtomicIntegral<T>; };
template<typename T> struct ChooseAtomicType<T*, false, false, true>	{ using Type = AtomicPointer<T*>; };
/** @} */

/**
 * Sets value to true if @c T can use
 * platform atomics.
 */
template<typename T>
struct CanUsePlatformAtomics
{
	enum { value = IsTrivial<T>::value && (sizeof(T) == 1 || sizeof(T) == 2 || sizeof(T) == 4 || sizeof(T) == 8) };
};

/** Quick access to the atomic class type */
template<
	typename AtomicT,
	bool inIsVoidPointer = IsVoidPointer<AtomicT>::value,
	bool inIsIntegral = IsIntegral<AtomicT>::value,
	bool inUsePlatformAtomics = CanUsePlatformAtomics<AtomicT>::value
>
using ChooseAtomicTypeT = typename ChooseAtomicType<AtomicT, inIsVoidPointer, inIsIntegral, inUsePlatformAtomics>::Type;

/**
 * Base class for atomic types. Defines
 * methods to load and store an atomic
 * value.
 * 
 * @param IntT type of the atomic
 */
template<typename AtomicT>
struct AtomicBase
{
	using AtomicOrder = PlatformAtomics::AtomicOrder;

protected:
	AtomicBase() = default;

	/**
	 * Initialize value.
	 * 
	 * @param inValue initial value to set
	 */
	constexpr FORCE_INLINE AtomicBase(const AtomicT & inValue)
		: value{inValue}
	{
		//
	}

public:
	/**
	 * Load atomic value.
	 * 
	 * @param order consistency level (default
	 * 	is sequential consistency)
	 * @return loaded value
	 */
	FORCE_INLINE AtomicT load(AtomicOrder order = AtomicOrder::Sequential) const
	{
		switch (order)
		{
			case AtomicOrder::Sequential:
				return PlatformAtomics::read<AtomicOrder::Sequential>(&value);
			
			case AtomicOrder::Relaxed:
				return PlatformAtomics::read<AtomicOrder::Relaxed>(&value);
			
			default:
				CHECKF(false, "Unkown atomic order %d", order); // TODO: Enum values as strings
				return value;
		}
	}

	/**
	 * Store atomic value.
	 * 
	 * @param inValue value to store
	 * @param order consistency level (default
	 * 	is sequential consistency)
	 */
	FORCE_INLINE void store(AtomicT inValue, AtomicOrder order = AtomicOrder::Sequential)
	{
		switch (order)
		{
			case AtomicOrder::Sequential:
				PlatformAtomics::store<AtomicOrder::Sequential>(&value, inValue);
				break;
			
			case AtomicOrder::Relaxed:
				PlatformAtomics::store<AtomicOrder::Relaxed>(&value, inValue);
				break;
			
			default:
				CHECKF(false, "Unkown atomic order %d", order); // TODO: Enum values as strings
				break;
		}
	}

protected:
	/// Atomic value
	volatile AtomicT value;
};

/**
 * Implements arithemtic methods for
 * atomic types (e.g. increment, decrement,
 * add and sub).
 * 
 * @param AtomicT type of the atomic value
 * @param IntT arithemtic type, usually
 * 	equivalent to @c AtomicT
 */
template<typename AtomicT, typename IntT = AtomicT>
struct AtomicArithmetic : public AtomicBase<AtomicT>
{
protected:
	using AtomicBaseT = AtomicBase<AtomicT>;
	using AtomicBaseT::AtomicBaseT;

public:
	/**
	 * Increments atomic value by one
	 * and returns incremented value.
	 */
	FORCE_INLINE AtomicT operator++()
	{
		return PlatformAtomics::add(&this->value, 1) + 1;
	}

	/**
	 * Increments atomic value by one
	 * and returns value before increment.
	 */
	FORCE_INLINE AtomicT operator++(int32)
	{
		return PlatformAtomics::add(&this->value, 1);
	}

	/**
	 * Decrement atomic value by one
	 * and returns decremented value.
	 */
	FORCE_INLINE AtomicT operator--()
	{
		return PlatformAtomics::sub(&this->value, 1) - 1;
	}

	/**
	 * Decrement atomic value by one
	 * and returns value before decrement.
	 */
	FORCE_INLINE AtomicT operator--(int32)
	{
		return PlatformAtomics::sub(&this->value, 1);
	}

	/**
	 * Increment atomic value by an
	 * abitrary @c amount.
	 * 
	 * @param amount increment value
	 * @return incremented value
	 */
	FORCE_INLINE AtomicT operator+=(IntT amount)
	{
		return PlatformAtomics::add(&this->value, amount) + amount;
	}

	/**
	 * Decrement atomic value by an
	 * abitrary @c amount.
	 * 
	 * @param amount decrement value
	 * @return decremented value
	 */
	FORCE_INLINE AtomicT operator-=(IntT amount)
	{
		return PlatformAtomics::sub(&this->value, amount) - amount;
	}
};

/**
 * Implementation for pointer types that
 * simply extends arithemtic implementation.
 * 
 * @param PtrT pointer type
 */
template<typename PtrT>
struct AtomicPointer : public AtomicArithmetic<PtrT, intp>
{
protected:
	using AtomicBaseT = AtomicArithmetic<PtrT, intp>;
	using AtomicBaseT::AtomicBaseT; 
};

/**
 * Implementation for integral types that
 * implements bitwise operations.
 * 
 * @param IntT integral type
 */
template<typename IntT>
struct AtomicIntegral : public AtomicArithmetic<IntT>
{
protected:
	using AtomicBaseT = AtomicArithmetic<IntT>;
	using AtomicBaseT::AtomicBaseT; 
};

/**
 * Final implementation of the atomic
 * class.
 * 
 * @param AtomicT type of the atomic
 * 	value
 */
template<typename AtomicT>
struct Atomic final : public ChooseAtomicTypeT<AtomicT>
{
	Atomic() = default;

	/**
	 * Initialize atomic value.
	 * 
	 * @param inValue initial value to set
	 */
	constexpr FORCE_INLINE Atomic(const AtomicT & inValue)
		: ChooseAtomicTypeT<AtomicT>{inValue}
	{
		//
	}

	/**
	 * Assign a new value to the atomic.
	 * 
	 * @param inValue new value to assign
	 * @return assigned value
	 */
	FORCE_INLINE AtomicT operator=(const AtomicT & inValue)
	{
		return (this->store(inValue), this->value);
	}

private:
	Atomic(const Atomic&) = delete;
	Atomic(Atomic&&) = delete;

	Atomic & operator=(const Atomic&) = delete;
	Atomic & operator=(Atomic&&) = delete;

public:
	/**
	 * Returns loaded value.
	 */
	FORCE_INLINE operator AtomicT() const
	{
		return this->load();
	}
};
