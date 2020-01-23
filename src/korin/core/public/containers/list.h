#pragma once

#include "../core_types.h"
#include "../templates/utility.h"
#include "../hal/malloc_ansi.h"
#include "containers_types.h"

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
	template<typename TT>
	explicit FORCE_INLINE Link(TT && inData)
		: next{nullptr}
		, prev{nullptr}
		, data{forward<TT>(inData)}
	{
		//
	}
};

//////////////////////////////////////////////////
// List
//////////////////////////////////////////////////

/**
 * 
 */
template<typename T>
struct ListIterator
{
	template<typename> friend struct ListConstIterator;

	//////////////////////////////////////////////////
	// Iterator types
	//////////////////////////////////////////////////
	
	using RefT = T&;
	using PtrT = T*;

	/**
	 * Default constructor
	 */
	FORCE_INLINE ListIterator()
		: link{nullptr}
	{
		//
	}

	/**
	 * Initialize link
	 */
	FORCE_INLINE ListIterator(Link<T> * inLink)
		: link{inLink}
	{
		//
	}

	//////////////////////////////////////////////////
	// BaseIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE RefT operator*() const
	{
		return link->data;
	}

	FORCE_INLINE PtrT operator->() const
	{
		return &operator*();
	}

	FORCE_INLINE bool operator==(const ListIterator & other) const
	{
		return link == other.link;
	}

	FORCE_INLINE bool operator!=(const ListIterator & other) const
	{
		return !(*this == other);
	}

	//////////////////////////////////////////////////
	// ForwardIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE ListIterator & operator++()
	{
		link = link->next;
		return *this;
	}

	FORCE_INLINE ListIterator operator++(int)
	{
		auto other = ListIterator{link->next};
		link = link->next;
		return other;
	}

	//////////////////////////////////////////////////
	// BidirectionalIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE ListIterator & operator--()
	{
		link = link->prev;
		return *this;
	}

	FORCE_INLINE ListIterator operator--(int) const
	{
		return ListIterator{link->prev};
	}

protected:
	/// Current link
	Link<T> * link;
};

/**
 * 
 */
template<typename T>
struct ListConstIterator
{
	template<typename, typename> friend class List;

	//////////////////////////////////////////////////
	// Iterator types
	//////////////////////////////////////////////////
	
	using RefT = const T&;
	using PtrT = const T*;

	/**
	 * Default constructor
	 */
	FORCE_INLINE ListConstIterator()
		: link{nullptr}
	{
		//
	}

	/**
	 * Initialize with link
	 */
	FORCE_INLINE ListConstIterator(Link<T> * inLink)
		: link{inLink}
	{
		//
	}

	/**
	 * Cast iterator to const iterator
	 */
	FORCE_INLINE ListConstIterator(const ListIterator<T> & other)
		: link{other.link}
	{
		//
	}

	//////////////////////////////////////////////////
	// BaseIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE RefT operator*() const
	{
		return link->data;
	}

	FORCE_INLINE PtrT operator->() const
	{
		return &operator*();
	}

	FORCE_INLINE bool operator==(const ListConstIterator & other) const
	{
		return link == other.link;
	}

	FORCE_INLINE bool operator!=(const ListConstIterator & other) const
	{
		return !(*this == other);
	}

	//////////////////////////////////////////////////
	// ForwardIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE ListConstIterator & operator++()
	{
		link = link->next;
		return *this;
	}

	FORCE_INLINE ListConstIterator operator++(int)
	{
		auto other = ListConstIterator{link->next};
		link = link->next;
		return other;
	}

	//////////////////////////////////////////////////
	// BidirectionalIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE ListConstIterator & operator--()
	{
		link = link->prev;
		return *this;
	}

	FORCE_INLINE ListConstIterator operator--(int) const
	{
		return ListConstIterator{link->prev};
	}

protected:
	/// Current link
	Link<T> * link;
};

/**
 * A doubly-linked list
 */
template<typename T, typename AllocT = MallocAnsi>
class List
{
public:
	/// Link type
	using Link = ::Link<T>;

	/// Iterator type
	using Iterator = ListIterator<T>;

	/// Const iterator type
	using ConstIterator = ListConstIterator<T>;

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
	template<typename TT>
	FORCE_INLINE Link * createLink(TT && data)
	{
		return new(reinterpret_cast<Link*>(allocator->alloc(sizeof(Link), alignof(Link)))) Link(forward<TT>(data));
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
	 * Returns a new iterator that points
	 * to the first element of the list
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
	/// @}

	/**
	 * Returns a new iterator that points
	 * to the end of the list
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
	 * Push element(s) to the front of the list
	 * 
	 * @param [in] arg,args inserted element(s)
	 * @return reference to inserted data
	 * @{
	 */
	template<typename TT>
	T & pushFront(TT && arg)
	{
		++length;
		
		// Create link
		Link * link = createLink(forward<TT>(arg));

		if (head)
		{
			link->next = head;
			head->prev = link;

			return (head = link)->data;
		}
		else
			return (head = tail = link)->data;
	}

	template<typename TT, typename ... TTT>
	void pushFront(TT && arg, TTT && ... args)
	{
		++length;
		
		// Create link
		Link * link = createLink(forward<TT>(arg));

		if (head)
		{
			link->next = head;
			head->prev = link;
			head = link;
		}
		else head = tail = link;

		// Push back other elements
		// TODO: not what we expect, we should
		// TODO: push the last one first
		pushFront(forward<TTT>(args) ...);
	}
	/// @}

	/**
	 * Push element(s) to the back of the list
	 * 
	 * @param [in] arg,args inserted element(s)
	 * @return reference to inserted data
	 * @{
	 */
	template<typename TT>
	T & pushBack(TT && arg)
	{
		++length;
		
		// Create link
		Link * link = createLink(forward<TT>(arg));

		if (tail)
		{
			link->prev = tail;
			tail->next = link;

			return (tail = link)->data;
		}
		else
			return (tail = head = link)->data;
	}

	template<typename TT, typename ... TTT>
	void pushBack(TT && arg, TTT && ... args)
	{
		++length;
		
		// Create link
		Link * link = createLink(forward<TT>(arg));

		if (tail)
		{
			link->prev = tail;
			tail->next = link;
			tail = link;
		}
		else tail = head = link;

		// Push back other elements
		pushBack(forward<TTT>(args) ...);
	}
	/// @}
	
	/**
	 * Remove element from list
	 * 
	 * @param [in] it iterator position
	 * @param [in] link link of the element
	 * @{
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

	FORCE_INLINE void remove(const ConstIterator & it)
	{
		if (it.link) remove(it.link);
	}
	/// @}

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