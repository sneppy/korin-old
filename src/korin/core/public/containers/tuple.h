#pragma once

#include "core_types.h"
#include "templates/enable_if.h"
#include "./containers_types.h"

/**
 * A simple implementation of
 * a tuple class, that mimics
 * the behaviour of @c std::tuple
 * 
 * Example
 * ```cpp
 * Tuple<int, std::string, bool> tup{0, "Hello, world!", true};
 * std::string & msg = tup.get<std::string>();
 * int flag = tup.get<2>();
 * int val = tup.get<0>();
 * tup.get<bool>() == tup.get<2>(); // True
 * ```
 * 
 * @param ItemListT list of
 * 	item types
 */
template<typename ...ItemListT>
class Tuple : public TupleBase<0, ItemListT...>
{
	using BaseT = TupleBase<0, ItemListT...>;
	using BaseT::BaseT;
};

/**
 * Recursive implementation of
 * Tuple.
 * 
 * @param inIdx inde of current
 * 	item
 * @param HeadT type of the head
 * 	item
 * @param TailT types of the tail
 * 	items
 */
template<uint64 inIdx, typename HeadT, typename ...TailT>
class TupleBase<inIdx, HeadT, TailT...> : public TupleBase<inIdx + 1, TailT...>
{
	using BaseT = TupleBase<inIdx + 1, TailT...>;

public:
	TupleBase() = default;

	/**
	 * Initialize this and all tail items.
	 * 
	 * @param inItem init value for the
	 * 	current item
	 * @param inItems init value for the
	 * 	other items
	 */
	template<typename ItemT, typename ...ItemListT>
	constexpr FORCE_INLINE TupleBase(ItemT && inItem, ItemListT && ...inItems)
		: BaseT{forward<ItemListT>(inItems)...}
		, item{forward<ItemT>(inItem)}
	{
		//
	}

	/**
	 * Returns the item at position
	 * @c idx or fails to compile
	 * if no @c idx is out of range.
	 * 
	 * Example:
	 * ```cpp
	 * Tuple<int, float, long> tup;
	 * long foo = tup.get<2>();
	 * ```
	 * 
	 * @param idx position of the
	 * 	item to get
	 * @return ref to item
	 * @{
	 */
	template<uint64 idx, typename = typename EnableIf<idx == inIdx>::Type>
	constexpr FORCE_INLINE const HeadT & get() const
	{
		return item;
	}

	template<uint64 idx, typename = typename EnableIf<(idx != inIdx)>::Type>
	constexpr FORCE_INLINE const auto & get() const
	{
		static_assert(idx > inIdx, "Item with idx not found");
		return BaseT::template get<idx>();
	}

	template<uint64 idx, typename = typename EnableIf<idx == inIdx>::Type>
	constexpr FORCE_INLINE HeadT & get()
	{
		return item;
	}

	template<uint64 idx, typename = typename EnableIf<(idx != inIdx)>::Type>
	constexpr FORCE_INLINE auto & get()
	{
		static_assert(idx > inIdx, "Item with idx not found");
		return BaseT::template get<idx>();
	}
	/** @} */

	/**
	 * Returns first item (left to
	 * right) with type @c ItemT or
	 * fails to compile if no such
	 * item exists.
	 * 
	 * Example:
	 * ```cpp
	 * Tuple<int, float, long> tup;
	 * float foo = tup.get<float>();
	 * ```
	 * 
	 * @param ItemT type of the item
	 * 	to get
	 * @return ref to item
	 * @{
	 */
	template<typename ItemT, typename = typename EnableIf<IsSameType<ItemT, HeadT>::value>::Type>
	constexpr FORCE_INLINE const HeadT & get() const
	{
		return item;
	}

	template<typename ItemT, typename = typename EnableIf<!IsSameType<ItemT, HeadT>::value>::Type>
	constexpr FORCE_INLINE const auto & get() const
	{
		return BaseT::template get<ItemT>();
	}

	template<typename ItemT, typename = typename EnableIf<IsSameType<ItemT, HeadT>::value>::Type>
	constexpr FORCE_INLINE HeadT & get()
	{
		return item;
	}

	template<typename ItemT, typename = typename EnableIf<!IsSameType<ItemT, HeadT>::value>::Type>
	constexpr FORCE_INLINE auto & get()
	{
		return BaseT::template get<ItemT>();
	}
	/** @} */

protected:
	/// Contained item
	HeadT item;
};

/**
 * @c TupleBase base recursion
 * case.
 * 
 * @param inIdx current position
 * @param HeadT type of the item
 * 	at this position
 */
template<uint64 inIdx, typename HeadT>
class TupleBase<inIdx, HeadT>
{
public:
	TupleBase() = default;

	/**
	 * Initialize this item.
	 * 
	 * @param inItem init value
	 */
	template<typename ItemT>
	constexpr FORCE_INLINE TupleBase(ItemT && inItem)
		: item{forward<ItemT>(inItem)}
	{
		//
	}

	/**
	 * Returns the number of items
	 * in the tuple.
	 * @{
	 */
	constexpr uint64 getCount() const
	{
		return inIdx + 1;
	}

	constexpr uint64 getLength() const
	{
		return inIdx + 1;
	}

	constexpr sizet getSize() const
	{
		return inIdx + 1;
	}
	/** @} */

	/**
	 * Returns item at position @c idx
	 * or fails to compile.
	 * 
	 * @param idx position of the item
	 * 	to get
	 * @return ref to item
	 * @{
	 */
	template<uint64 idx, typename = typename EnableIf<idx == inIdx>::Type>
	constexpr FORCE_INLINE const HeadT & get() const
	{
		return item;
	}

	template<uint64 idx, typename = typename EnableIf<idx == inIdx>::Type>
	constexpr FORCE_INLINE HeadT & get()
	{
		return item;
	}
	/** @} */

	/**
	 * Returns first item with type
	 * @c ItemT or fails to compile.
	 * 
	 * @param ItemT type of the item
	 * 	to get
	 * @return ref to item
	 * @{
	 */
	template<typename ItemT>
	constexpr FORCE_INLINE const HeadT & get() const
	{
		static_assert(IsSameType<ItemT, HeadT>::value, "Item with type not found");
		return item;
	}

	template<typename ItemT>
	constexpr FORCE_INLINE HeadT & get()
	{
		static_assert(IsSameType<ItemT, HeadT>::value, "Item with type not found");
		return item;
	}
	/** @} */

protected:
	/// Contained item
	HeadT item;
};