#pragma once

#include "core_types.h"

/**
 * A node with next and prev
 * connections that extends
 * the given type.
 * 
 * @param T item type
 */
template<typename T>
struct InlineList : public T
{
	using BaseT = T;
	using BaseT::BaseT;

	/// Next link
	InlineList * next = nullptr;

	/// Previous link
	InlineList * prev = nullptr;

	/**
	 * Insert a node in front of this
	 * one.
	 * 
	 * @param other ptr to node to
	 * 	insert
	 * @return ptr to the inserted
	 * 	node
	 */
	FORCE_INLINE InlineList * pushFront(InlineList * other)
	{
		CHECK(other != nullptr)
		other->next = this;
		
		if ((other->prev = prev))
		{
			CHECK(prev->next == this)
			prev->next = other;
		}

		prev = other;

		return other;
	}

	/**
	 * Insert a node after this one.
	 * 
	 * @param other ptr to node to
	 * 	insert
	 * @return ptr to the inserted
	 * 	node
	 */
	FORCE_INLINE InlineList * pushBack(InlineList * other)
	{
		CHECK(other != nullptr)
		other->prev = this;

		if ((other->next = next))
		{
			CHECK(next->prev == this)
			next->prev = other;
		}

		next = other;

		return other;
	}

	/**
	 * Removes this node from the
	 * list.
	 */
	FORCE_INLINE void pop()
	{
		InlineList * oldprev = prev;

		if (prev)
		{
			// Deatch prev node, if any
			prev->next = next;
			prev = nullptr;
		}

		if (next)
		{
			// Detach next node if any
			next->prev = oldprev;
			next = nullptr;
		}
	}

	/**
	 * Remove head node. Node must
	 * be head of the list. Returns
	 * ptr to next node.
	 */
	FORCE_INLINE InlineList * popHead()
	{
		CHECKF(prev == nullptr, "Node must be head of the list, prev node (%p) found.", prev)

		if (auto head = next)
		{
			next->prev = nullptr;
			next = nullptr;

			return head;
		}

		return nullptr;
	}

	/**
	 * Remove tail node. Node must
	 * be tail of the list. Returns
	 * ptr to prev node.
	 */
	FORCE_INLINE InlineList * popTail()
	{
		CHECKF(next == nullptr, "Node must be tail of the list, next node (%p) found.", next)

		if (auto tail = prev)
		{
			prev->next = nullptr;
			prev = nullptr;

			return tail;
		}

		return nullptr;
	}
};
