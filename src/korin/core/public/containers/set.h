#pragma once

#include "core_types.h"
#include "misc/utility.h"
#include "templates/functional.h"
#include "./tree.h"

template<typename T, typename CompareT>
class Set
{
	using TreeT = BinaryTree<T, CompareT>;
	using NodeT = typename TreeT::NodeT;
	using Iterator = typename TreeT::Iterator;
	using ConstIterator = typename TreeT::ConstIterator;

public:
	/**
	 * 
	 */
	FORCE_INLINE Set()
		: tree{}
	{
		//
	}

	/**
	 * Returns a ref to the underlying tree.
	 */
	FORCE_INLINE const TreeT & getTree() const
	{
		return tree;
	}

	FORCE_INLINE TreeT & getTree()
	{
		return tree;
	}
	/** @} */

	/**
	 * Returns count of items in set.
	 * @{
	 */
	FORCE_INLINE uint64 getCount() const
	{
		return tree.getNumNodes();
	}
	
	METHOD_ALIAS_CONST(getSize, getCount);
	METHOD_ALIAS_CONST(getNumItems, getCount);
	/** @} */

	/**
	 * Returns iterator that points to
	 * the beginning of the set.
	 * @{
	 */
	FORCE_INLINE ConstIterator begin() const
	{
		return tree.begin();
	}

	FORCE_INLINE Iterator begin()
	{
		return tree.begin();
	}
	/** @} */

	/**
	 * Returns iterator that points to
	 * the end of the set.
	 * @{
	 */
	FORCE_INLINE ConstIterator end() const
	{
		return tree.end();
	}

	FORCE_INLINE Iterator end()
	{
		return tree.end();
	}
	/** @} */

	/**
	 * Returns iterator that starts from
	 * the first item found.
	 * 
	 * @param item item to search for
	 * @return iterator that points to
	 * 	found item
	 * @{
	 */
	FORCE_INLINE ConstIterator begin(const T & item) const
	{
		return tree.begin(item);
	}

	FORCE_INLINE Iterator begin(const T & item)
	{
		return tree.begin(item);
	}
	/** @} */

	/**
	 * Returns iterator that point to
	 * the item after the searched for
	 * item.
	 * 
	 * @param item item to search for
	 * @return iterator that points to
	 * 	the item after the found item
	 */
	FORCE_INLINE ConstIterator end(const T & item) const
	{
		// TODO: return tree.end(item);
		return tree.end();
	}

	FORCE_INLINE Iterator end(const T & item)
	{
		return tree.end();
	}
	/// @}

	/**
	 * Returns true if Set has item.
	 * 
	 * @param item item that will be
	 * 	searched for in the set
	 * @param outValue if provided and
	 * 	item is found, is filled with
	 * 	stored item value ref
	 * @return true if item exists in set
	 * @{
	 */
	FORCE_INLINE bool get(const T & item) const
	{
		return tree.findNode(item) != nullptr;
	}

	FORCE_INLINE bool get(const T & item, T const & outValue) const
	{
		// Get node to fetch value
		if (NodeT * node = tree.findNode(item))
		{
			outValue = node->data;
			return true;
		}

		return false;
	}

	FORCE_INLINE bool get(const T & item, T & outValue)
	{
		// Get node to fetch value
		if (NodeT * node = tree.findNode(item))
		{
			outValue = node->data;
			return true;
		}

		return false;
	}
	/** @} */

	/**
	 * Returns true if some of the provided
	 * items exist in the set,
	 * 
	 * @param item,items one or more items
	 * 	to search
	 * @return true if at least one item
	 * 	exists in the set
	 */
	FORCE_INLINE bool any(const T & item) const
	{
		return get(item);
	}

	template<typename ...ItemListT>
	FORCE_INLINE bool any(const T & item, ItemListT && ...items) const
	{
		return get(item) || any(forward<ItemListT>(items)...);
	}
	/** @} */

	/**
	 * Returns true if all of the provided
	 * items exist in the set.
	 * 
	 * @param item,items one or more items
	 * 	to search
	 * @return true if all the items exist
	 * in the set
	 * @{
	 */
	FORCE_INLINE bool all(const T & item) const
	{
		return get(item);
	}

	template<typename ...ItemListT>
	FORCE_INLINE bool all(const T & item, ItemListT && ...items) const
	{
		return get(item) && all(forward<ItemListT>(items)...);
	}
	/** @} */
	
	/**
	 * Insert a new value in the set and
	 * return value.
	 * 
	 * @see TreeT::insertUnique
	 * 
	 * @param item item inserted in the
	 * 	set
	 * @param items one or more items that
	 * 	will be inserted in the set
	 * @return inserted value
	 * @{
	 */
	template<typename ItemT>
	FORCE_INLINE T & set(ItemT && item)
	{
		return tree.insertUnique(forward<ItemT>(item));
	}

	template<typename ItemT, typename ...ItemListT>
	FORCE_INLINE void set(ItemT && item, ItemListT && ...items)
	{
		set(forward<ItemT>(item));
		set(forward<ItemListT>(items)...);
	}
	/** @} */

	/**
	 * Remove one item from the set.
	 * 
	 * @param item item to be removed
	 * @param outValue if provided, is
	 * 	assigned the value of the removed
	 * 	item
	 * @return true if item existed and
	 * 	was removed
	 */
	FORCE_INLINE bool remove(const T & item)
	{
		return tree.remove(item);
	}

	FORCE_INLINE bool remove(const T & item, T & outValue)
	{
		if (NodeT * node = tree.findNode(item))
		{
			// Set out value and return removed
			outValue = move(node->data);
			return tree.removeNode(node), true;
		}
		
		return false;
	}
	/** @} */

	/**
	 * Union operator. Inserts all unique
	 * items of other in this set, in linear
	 * time.
	 * 
	 * Example:
	 * {1, 3, 7} - {1, 5, 7} = {1, 3, 5, 7}
	 * 
	 * Both sets must use the same compare type,
	 * which ensures items use the same ordering
	 * system.
	 * 
	 * @param other other set which uses same
	 * 	compare type
	 * @return ref to self
	 */
	Set & operator+=(const Set & other)
	{
		NodeT * it = tree.getMin();
		const NodeT * jt = other.tree.getMin();

		while (it && jt)
		{
			int32 cmp = CompareT{}(it->data, jt->data);

			if (cmp < 0)
			{
				// Already in set, just advance
				it = it->next;
			}
			else if (cmp > 0)
			{
				// Set item and advance to next node
				set(jt->data);
				jt = jt->next;
			}
			else
			{
				// Advance both nodes
				it = it->next;
				jt = jt->next;
			}
		}
		
		// Set remaining items
		for (; jt; jt = jt->next) set(jt->data);

		return *this;
	}

	/**
	 * Intersect operator. Removes all items which
	 * don't exist on the other set, in linear time.
	 * 
	 * Example:
	 * {1, 3, 7} - {1, 5, 7} = {1, 7}
	 * 
	 * Both sets must use the same compare type,
	 * which ensures items use the same ordering
	 * system.
	 * 
	 * @param other other set which uses same
	 * 	compare type
	 * @return ref to self
	 */
	Set & operator*=(const Set & other)
	{
		NodeT * it = tree.getMin();
		const NodeT * jt = other.tree.getMin();

		while (it && jt)
		{
			int32 cmp = CompareT{}(it->data, jt->data);

			if (cmp < 0)
			{
				// Not in other set, advance node and remove
				it = tree.removeNode(it);
			}
			else if (cmp > 0)
			{
				// Just advance to next node
				jt = jt->next;
			}
			else
			{
				// Advance both nodes
				it = it->next;
				jt = jt->next;
			}
		}

		while (it) it = tree.removeNode(it);

		return *this;
	}

	/**
	 * Difference operator. Removes all items which
	 * exist on the other set, in linear time.
	 * 
	 * Example:
	 * {1, 3, 7} - {1, 5, 7} = {3}
	 * 
	 * Both sets must use the same compare type,
	 * which ensures items use the same ordering
	 * system.
	 * 
	 * @param other other set which uses same
	 * 	compare type
	 * @return ref to self
	 */
	Set & operator-=(const Set & other)
	{
		NodeT * it = tree.getMin();
		const NodeT * jt = other.tree.getMin();

		while (it && jt)
		{
			int32 cmp = CompareT{}(it->data, jt->data);

			if (cmp < 0)
			{
				// Just advance to next node
				it = it->next;
			}
			else if (cmp > 0)
			{
				// Just advance to next node
				jt = jt->next;
			}
			else
			{
				// Advance both nodes and remove common item
				it = tree.removeNode(it);
				jt = jt->next;
			}
		}

		// We are done

		return *this;
	}

	/// See equivalent compound operators
	/// @{
	FORCE_INLINE Set operator+(const Set & other) const
	{
		return Set{*this} += other;
	}

	FORCE_INLINE Set operator*(const Set & other) const
	{
		return Set{*this} *= other;
	}

	FORCE_INLINE Set operator-(const Set & other) const
	{
		return Set{*this} -= other;
	}
	/// @}

	/**
	 * This operator returns true if this
	 * set and the other set have the
	 * exact same items.
	 * 
	 * Example:
	 * {4, 7, 10} == {4, 7, 10} = true
	 * {4, 5, 10} == {4, 5, 10} = false
	 * 
	 * Both sets must use the same compare
	 * type.
	 * 
	 * @param other another set
	 * @return true if they have the same
	 * 	set of items
	 */
	bool operator==(const Set & other) const
	{
		const NodeT * it = tree.getMin();
		const NodeT * jt = other.tree.getMin();

		while (it && jt)
		{
			// If two items are different, return false
			if (CompareT{}(it->data, jt->data) != 0) return false;

			// Advance both nodes
			it = it->next;
			jt = jt->next;
		}
		
		// Return true only if both iterators reached the end
		return !it && !jt;
	}

	/**
	 * Returns true if the two sets have
	 * at least one item not in common.
	 * 
	 * Example:
	 * {1, 3, 7} != {1, 5, 7} = true
	 * {1, 3, 7} != {1, 3, 5, 7} = true
	 * {1, 3, 7} != {1, 3, 7} = false
	 * 
	 * @param other another set
	 * @return true if at least one item
	 * 	not in common
	 */
	FORCE_INLINE bool operator!=(const Set & other) const
	{
		return !(*this == other);
	}

	/**
	 * This operator returns true if this
	 * set is strictly contained in the
	 * other set. It returns false if
	 * the sets are equal.
	 * 
	 * Example:
	 * ```
	 * {1, 3, 4} < {1, 2, 3, 4} = true
	 * {1, 3, 5} < {1, 2, 3, 4} = false, because of 5
	 * {1, 3, 4} < {1, 3, 4} = false, because sets are equal
	 * ```
	 * 
	 * @param other another set
	 * @return true if other set contains
	 * 	this set
	 */
	bool operator<(const Set & other) const
	{
		const NodeT * it = tree.getMin();
		const NodeT * jt = other.tree.getMin();
		bool eq = true;

		while (it && jt)
		{
			int32 cmp = CompareT{}(it->data, jt->data);

			if (cmp < 0)
			{
				return false;
			}
			else if (cmp > 0)
			{
				eq = false;

				// Advance to next node
				jt = jt->next;
			}
			else
			{
				// Advance both nodes
				it = it->next;
				jt = jt->next;
			}
		}
		
		// Return true only if other still has elements
		return !eq;
	}

	/**
	 * This operator returns true if this
	 * set strictly contains the other set.
	 * It returns false if the two sets are
	 * equal.
	 * 
	 * Example:
	 * ```
	 * {1, 2, 3, 4} > {1, 2, 3} = true
	 * {1, 2, 3, 4} > {1, 3, 5} = false, because of 5
	 * {1, 3, 4} > {1, 3, 4} = false, because sets are equal
	 * ```
	 * 
	 * @param other another set
	 * @return true if they have the same
	 * 	set of items
	 */
	FORCE_INLINE bool operator>(const Set & other) const
	{
		return other < *this;
	}

	/**
	 * This operator returns true if this
	 * set is contained in the other set.
	 * 
	 * Example:
	 * ```
	 * {1, 3, 4} < {1, 2, 3, 4} = true
	 * {1, 3, 5} < {1, 2, 3, 4} = false, because of 5
	 * {1, 3, 4} < {1, 3, 4} = true
	 * ```
	 * 
	 * @param other another set
	 * @return true if this set is contained
	 * 	in the other set
	 */
	bool operator<=(const Set & other) const
	{
		const NodeT * it = tree.getMin();
		const NodeT * jt = other.tree.getMin();

		while (it && jt)
		{
			int32 cmp = CompareT{}(it->data, jt->data);

			if (cmp < 0)
			{
				return false;
			}
			else if (cmp > 0)
			{
				// Advance to next node
				jt = jt->next;
			}
			else
			{
				// Advance both nodes
				it = it->next;
				jt = jt->next;
			}
		}
		
		return true;
	}

	/**
	 * This operator returns true if this
	 * set contains the other set.
	 * 
	 * Example:
	 * ```
	 * {1, 2, 3, 4} > {1, 2, 3} = true
	 * {1, 2, 3, 4} > {1, 3, 5} = false, because of 5
	 * {1, 3, 4} > {1, 3, 4} = true
	 * ```
	 * 
	 * @param other another set
	 * @return true if this set contains the
	 * 	other set
	 */
	FORCE_INLINE bool operator>=(const Set & other) const
	{
		return other <= *this;
	}

private:
	/// Binary tree used to store nodes
	TreeT tree;
};