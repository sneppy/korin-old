#pragma once

#include "core_types.h"
#include "templates/utility.h"
#include "hal/malloc_ansi.h"
#include "./containers_types.h"
#include "./string.h"

/**
 * Node class with next and prev
 * links.
 * 
 * @param T type of the items
 */
template<typename T>
struct Link
{
	using DataT = T;

	/// Pointer to next link
	Link * next;

	/// Pointer to previous link
	Link * prev;

	/// Link data
	T data;

	/**
	 * Initialize data.
	 */
	template<typename ItemT>
	Link(ItemT && inData)
		: next{nullptr}
		, prev{nullptr}
		, data{forward<ItemT>(inData)}
	{
		//
	}
};

/**
 * 
 */
template<typename LinkT, typename IteratorTraitsT = BidirectionalIterator<typename LinkT::DataT>>
struct ListIteratorBase
{
	template<typename, typename>	friend struct ListIteratorBase;
	template<typename, typename>	friend class List;

	using RefT = typename IteratorTraitsT::RefT;
	using PtrT = typename IteratorTraitsT::PtrT;

	/**
	 * Create iterator that points
	 * on link.
	 * 
	 * @param inLink current link
	 */
	FORCE_INLINE explicit ListIteratorBase(LinkT * inLink = nullptr)
		: link{inLink}
	{
		//
	}

	/**
	 * Returns ref to data pointed by
	 * current link.
	 */
	FORCE_INLINE RefT operator*() const
	{
		return link->data;
	}

	/**
	 * Returns ptr to data pointed
	 * by current link.
	 */
	FORCE_INLINE PtrT operator->() const
	{
		return &(**this);
	}

	/**
	 * Returns true if both iterators
	 * point to the same link.
	 */
	FORCE_INLINE bool operator==(const ListIteratorBase & other) const
	{
		return link == other.link;
	}

	/**
	 * Returns true if iterators point
	 * to different links.
	 */
	FORCE_INLINE bool operator!=(const ListIteratorBase & other) const
	{
		return !(*this == other);
	}

	/**
	 * Advances iterator to the next
	 * link and returns a ref to the
	 * iterator.
	 */
	FORCE_INLINE ListIteratorBase & operator++()
	{
		link = link->next;
		return *this;
	}

	/**
	 * Advances iterator to the next
	 * link and returns a new iterator
	 * that points to the current link.
	 */
	FORCE_INLINE ListIteratorBase operator++(int32)
	{
		ListIteratorBase it{*this};
		link = link->next;
		return it;
	}

	/**
	 * Move iterator backward and
	 * returns a ref to the iterator.
	 */
	FORCE_INLINE ListIteratorBase & operator--()
	{
		link = link->prev;
		return *this;
	}

	/**
	 * Moves iterator to the previous
	 * link and returns a new iterator
	 * that points to the current link.
	 */
	FORCE_INLINE ListIteratorBase operator--(int32)
	{
		ListIteratorBase it{*this};
		link = link->prev;
		return it;
	}

private:
	/// Current link
	LinkT * link;
};

/// Iterator types
/// @{
template<typename LinkT>
using ListConstIterator = ListIteratorBase<const LinkT, BidirectionalIterator<const typename LinkT::DataT>>;

template<typename LinkT>
using ListIterator = ListIteratorBase<LinkT, BidirectionalIterator<typename LinkT::DataT>>;
/// @}

/**
 * 
 */
template<typename T>
class List<T, void>
{
	using DataT = T;
	using LinkT = Link<T>;
	using Iterator = ListIterator<LinkT>;
	using ConstIterator = ListConstIterator<LinkT>;

public:
	/**
	 * Default constructor, creates empty
	 * list with global allocator.
	 */
	FORCE_INLINE List()
		: malloc{}
		, head{nullptr}
		, tail{nullptr}
		, length{0}
	{
		//
	}

	/**
	 * Allocator initializer.
	 */
	FORCE_INLINE List(MallocBase * inMalloc)
		: malloc{inMalloc}
		, head{nullptr}
		, tail{nullptr}
		, length{0}
	{
		//
	}

protected:
	/**
	 * Create a single link with
	 * the provided data.
	 * 
	 * @param data link data
	 * @return pointer to link
	 */
	template<typename DataT>
	FORCE_INLINE LinkT * createLink(DataT && a) const
	{
		return new (malloc.alloc()) LinkT{forward<DataT>(a)};
	}
	
	/**
	 * Destroy link.
	 */
	FORCE_INLINE void destroyLink(LinkT * link) const
	{
		link->~LinkT();
		malloc.free(link);
	}

public:
	/**
	 * Empty list, removes all links.
	 */
	void empty()
	{
		while (head)
		{
			LinkT * link = head;
			head = head->next;
			destroyLink(link);
		}
		
		head = tail = nullptr;
		length = 0;
	}

protected:
	/**
	 * Destroy list.
	 */
	FORCE_INLINE void destroy()
	{
		empty();
	}

public:
	/**
	 * Copies data from iterators.
	 * 
	 * @param inBegin,inEnd begin and
	 * 	end iterators
	 */
	template<typename It>
	List(It inBegin, It inEnd)
		: List{}
	{
		for (; inBegin != inEnd; ++inBegin)
		{
			pushBack(*inBegin);
		}
	}


	/**
	 * Copy constructor.
	 */
	List(const List & other)
		: List{other.begin(), other.end()}
	{
		//
	}

	/**
	 * Move constructor.
	 */
	FORCE_INLINE List(List && other)
		: malloc{move(other.malloc)}
		, head{other.head}
		, tail{other.tail}
		, length{other.length}
	{
		other.head = other.tail = nullptr;
		other.length = 0ull;
	}

	/**
	 * Copy assignment.
	 */
	List & operator=(const List & other)
	{
		LinkT * it = head;
		LinkT * jt = other.head;

		if (!jt)
		{
			// Empty array
			empty();
		}
		else if (!it)
		{
			for (const auto & data : other)
			{
				// Copy all items
				pushBack(data);
			}
		}
		else
		{
			for (; it && jt; it = (tail = it)->next, jt = jt->next)
			{
				// Copy data
				it->data = jt->data;
			}
			
			while (it)
			{
				LinkT * link = it;
				it = it->next;
				destroyLink(link);
			}

			// Push remaining objects
			while (jt)
			{
				pushBack(jt->data);
				jt = jt->next;
			}

			// Ensure tail is valid
			if (tail) tail->next = nullptr;
		}

		return *this;
	}

	/**
	 * Move assignment.
	 */
	FORCE_INLINE List & operator=(List && other)
	{
		// Remove all
		empty();

		// Take from other
		malloc = move(other.malloc);
		head = other.head;
		tail = other.tail;
		length = other.length;

		other.head = other.tail = nullptr;
		other.length = 0;
	}

	/**
	 * Destructor, destroys all links.
	 */
	FORCE_INLINE ~List()
	{
		destroy();
	}

	/**
	 * Returns head link.
	 * @{
	 */
	FORCE_INLINE const LinkT * getHead() const
	{
		return head;
	}

	FORCE_INLINE LinkT * getHead()
	{
		return head;
	}
	/** @} */

	/**
	 * Returns tail link.
	 * @{
	 */
	FORCE_INLINE const LinkT * getTail() const
	{
		return tail;
	}

	FORCE_INLINE LinkT * getTail()
	{
		return tail;
	}
	/** @} */

	/**
	 * Returns a new iterator that
	 * points to the beginning of
	 * the list.
	 * @{
	 */
	FORCE_INLINE ConstIterator begin() const
	{
		return ConstIterator{head};
	}

	FORCE_INLINE Iterator begin()
	{
		return Iterator{head};
	}
	/** @} */

	/**
	 * Returns a new iterator that
	 * points to the end of the list.
	 * @{
	 */
	FORCE_INLINE ConstIterator end() const
	{
		return ConstIterator{nullptr};
	}

	FORCE_INLINE Iterator end()
	{
		return Iterator{nullptr};
	}
	/** @} */

	/**
	 * Returns list length.
	 */
	FORCE_INLINE uint64 getLength() const
	{
		return length;
	}

	/**
	 * Push element(s) to the end of
	 * the array.
	 * 
	 * @param data,datas data to
	 * 	insert in list
	 * @return ref to inserted data
	 * @{
	 */
	template<typename DataT>
	T & pushBack(DataT && data)
	{
		LinkT * link = createLink(forward<DataT>(data));

		if (!tail)
		{
			head = tail = link;
		}
		else
		{
			link->prev = tail;
			tail->next = link;
			tail = link;
		}

		++length;

		return link->data;
	}

	template<typename ...DataListT>
	FORCE_INLINE void pushBack(DataListT && ... datas)
	{
		(pushBack(forward<DataT>(datas)), ...);
	}
	/** @} */

	/**
	 * Push element(s) to the beginning
	 * of the array. Data is inserted
	 * as if pushFront would be called
	 * on the parameters in the order
	 * with which they are presented
	 * (left to right)
	 * 
	 * Example:
	 * 
	 * ```
	 * pushFront(1) ... (3) = {3, 2, 1}
	 * pushFront(1, 2, 3) = {3, 2, 1}
	 * ```
	 * 
	 * @param data,datas data(s) to push
	 * 	at the beginning of the list
	 * @return inserted element
	 * @{
	 */
	template<typename DataT>
	FORCE_INLINE T & pushFront(DataT && arg)
	{
		LinkT * link = createLink(forward<DataT>(arg));

		if (!head)
		{
			head = tail = link;
		}
		else
		{
			link->next = head;
			head->prev = link;
			head = link;
		}

		++length;

		return link->data;
	}

	template<typename ...DataListT>
	FORCE_INLINE void pushFront(DataListT && ... datas)
	{
		(... ,pushFront(forward<DataT>(datas)));
	}
	/** @} */

	/**
	 * Remove link from list
	 */
	void remove(LinkT * link)
	{
		if (link != head && link != tail)
		{
			LinkT * prev = link->prev;
			LinkT * next = link->next;
			
			if (prev) prev->next = next;
			if (next) next->prev = prev;
		}
		else
		{
			if (link == head) head = head->next;
			if (link == tail) tail = tail->prev;
		}

		--length;

		// Destroy link
		destroyLink(link);
	}

	/**
	 * Remove element pointed by
	 * iterator.
	 */
	FORCE_INLINE void remove(Iterator it)
	{
		if (it.link) remove(it.link);
	}

	/**
	 * Remove tail element.
	 * 
	 * @return false if list is empty,
	 * 	true otherwise
	 */
	bool removeBack()
	{
		if (tail)
		{
			LinkT * link = tail;

			if (tail == head)
			{
				head = tail = nullptr;
			}
			else
			{
				(tail = tail->prev)->next = nullptr;
			}
			
			--length;
			
			// Destroy link
			destroyLink(link);
			
			return true;
		}
		
		return false;
	}

	/**
	 * Remove head element.
	 * 
	 * @return false if list is empty,
	 * 	true otherwise
	 */
	bool removeFront()
	{
		if (head)
		{
			LinkT * link = head;

			if (head == tail)
			{
				head = tail = nullptr;
			}
			else
			{
				(head = head->next)->prev = nullptr;
			}
			
			--length;
			
			// Destroy link
			destroyLink(link);
			
			return true;
		}
		
		return false;
	}

	/**
	 * Remove tail element and
	 * return its data.
	 * 
	 * @param outData returned tail
	 * 	data
	 * @return false if list is empty,
	 * 	true otherwise
	 */
	bool popBack(T & outData)
	{
		if (tail)
		{
			outData = move(tail->data);
			return removeBack();
		}
		else return false;
	}

	/**
	 * Remove head element and
	 * return its data.
	 * 
	 * @param outData returned head
	 * 	data
	 * @return false if list is empty,
	 * 	true otherwise
	 */
	bool popFront(T & outData)
	{
		if (tail)
		{
			outData = move(head->data);
			return removeFront();
		}
		else return false;
	}

	/**
	 * Prints list to string.
	 */
	String toString() const
	{
		if (!head) return "[]";

		String out = "[ ";

		for (auto it = head; it != tail; it = it->next)
		{
			out += it->data;
			out += "-> ";
		}

		return out + tail->data + " ]";
	}

protected:
	/// Allocator
	mutable MallocObject<LinkT> malloc;

	/// Pointer to list head
	LinkT * head;

	/// Pointer to list tail
	LinkT * tail;

	/// Length of the list (number of links)
	uint64 length;
};

template<typename T, typename MallocT>
class List : public List<T, void>
{
	using Base = List<T, void>;

public:
	/**
	 * Constructs the allocator on
	 * the stack and passes it to
	 * the list.
	 */
	template<typename ...MallocCreateArgsT>
	FORCE_INLINE List(MallocCreateArgsT && ...mallocCreateArgs)
		: Base{&autoMalloc}
		, autoMalloc{forward<MallocCreateArgsT>(mallocCreateArgs)...}
	{
		//
	}

protected:
	/// Managed allocator
	MallocT autoMalloc;
};
