#pragma once

#include "core_types.h"
#include "templates/utility.h"
#include "hal/malloc_ansi.h"

/**
 * A link with a double link to the next
 * link and to the previous link
 */
template<typename T>
struct Link
{
	/// Next link pointer
	Link * next;

	/// Previous link pointer
	Link * prev;

	/// Link data
	T data;

public:
	/**
	 * Data constructor
	 */
	template<typename _T>
	explicit FORCE_INLINE Link(_T && inData)
		: next{nullptr}
		, prev{nullptr}
		, data{forward<_T>(inData)}
	{
		//
	}
};

//////////////////////////////////////////////////
// List
//////////////////////////////////////////////////

/**
 * A doubly-linked list
 */
template<typename T, typename AllocT = MallocAnsi>
class List
{
public:
	/// Link type
	using Link = ::Link<T>;

protected:
	/// Used allocator
	AllocT * allocator;

	/// Has managed allocator flag
	bool bHasOwnAlloc;

	/// List head
	Link * head;

	/// List tail
	Link * tail;

	/// List length
	uint64 length;

public:
	/**
	 * Default constructor
	 */
	explicit List(AllocT * inAllocator = nullptr)
		: allocator{inAllocator}
		, bHasOwnAlloc{inAllocator == nullptr}
		, head{nullptr}
		, tail{nullptr}
		, length{0}
	{
		// Create allocator if necessary
		if (bHasOwnAlloc) allocator = new AllocT;
	}

protected:
	/**
	 * Create a new link using allocator
	 * 
	 * @param [in] data link data
	 * @return link pointer
	 */
	template<typename _T>
	FORCE_INLINE Link * createLink(_T && data)
	{
		return new(reinterpret_cast<Link*>(allocator->alloc(sizeof(Link), alignof(Link)))) Link(forward<_T>(data));
	}

	/**
	 * Destroy a previously created link
	 */
	FORCE_INLINE void destroyLink(Link * link)
	{
		link->~Link();
		allocator->free(link);
	}

public:
	/**
	 * Destructor, empties list and destroy
	 * managed allocator
	 */
	FORCE_INLINE ~List()
	{
		////empty()
		if (bHasOwnAlloc) delete allocator;
	}

	/**
	 * Returns list length
	 * @{
	 */
	FORCE_INLINE uint64 getLength() const
	{
		return length;
	}
	METHOD_ALIAS_CONST(getSize, getLength)
	METHOD_ALIAS_CONST(getCount, getLength)
	/// @}

	/**
	 * Returns list head
	 * @{
	 */
	FORCE_INLINE Link * getHead()
	{
		return head;
	}

	FORCE_INLINE const Link * getHead() const
	{
		return head;
	}
	/// @}

	/**
	 * Returns list tail
	 * @{
	 */
	FORCE_INLINE Link * getTail()
	{
		return tail;
	}

	FORCE_INLINE const Link * getTail() const
	{
		return tail;
	}
	/// @}

	/**
	 * Push element to the front of the list
	 * 
	 * @param [in] data inserted element
	 * @return reference to inserted data
	 */
	template<typename _T>
	T & pushFront(_T && data)
	{
		++length;
		
		// Create link
		Link * link = createLink(forward<_T>(data));

		if (head)
		{
			link->next = head;
			head->prev = link;

			return (head = link)->data;
		}
		else
			return (head = tail = link)->data;
	}

	/**
	 * Push element to the back of the list
	 * 
	 * @param [in] data inserted element
	 * @return reference to inserted data
	 */
	template<typename _T>
	T & pushBack(_T && data)
	{
		++length;
		
		// Create link
		Link * link = createLink(forward<_T>(data));

		if (tail)
		{
			link->prev = tail;
			tail->next = link;

			return (tail = link)->data;
		}
		else
			return (tail = head = link)->data;
	}
	
	/**
	 * Remove element from list
	 * 
	 * @param [in] link link of the element
	 */
	void remove(Link * link)
	{
		--length;

		if (link == head)
		{
			if (head = head->next)
				head->prev = nullptr;
			else
				head = tail = nullptr;
		}
		else if (link == tail)
		{
			if (tail = tail->prev)
				tail->next = nullptr;
			else
				tail = head = nullptr;
		}
		else
		{
			link->next->prev = link->prev;
			link->prev->next = link->next;
		}

		// Destroy link and dealloc
		destroyLink(link);
	}

	/**
	 * Pop front element of list and return
	 * stored data
	 * 
	 * @param [out] data returned data
	 * @return true if list is not empty
	 */
	bool popFront(T & data)
	{
		if (head)
		{
			--length;

			// Get link data
			Link * link = head;
			data = move(link->data);

			// Unlink from list
			if (head = link->next)
				head->prev = nullptr;
			else
				head = tail = nullptr;
			
			// Destroy ex-head
			destroyLink(link);

			return true;
		}

		return false;
	}

	/**
	 * Pop back element of list and return
	 * stored data
	 * 
	 * @param [out] data returned data
	 * @return true if list is not empty
	 */
	bool popBack(T & data)
	{
		if (tail)
		{
			--length;

			// Get link data
			Link * link = tail;
			data = move(link->data);

			// Unlink from list
			if (tail = link->prev)
				tail->next = nullptr;
			else
				tail = head = nullptr;
			
			// Destroy ex-tail
			destroyLink(link);

			return true;
		}

		return false;
	}
};