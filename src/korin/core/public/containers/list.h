#pragma once

#include "../core_types.h"
#include "../templates/utility.h"
#include "../hal/malloc_ansi.h"
#include "containers_types.h"

/**
 * 
 */
template<typename T>
struct Link
{
	/// Pointer to next link
	Link * next;

	/// Pointer to previous link
	Link * prev;

	/// Link data
	T data;

	/**
	 * Initialize data
	 */
	template<typename TT>
	Link(TT && inData)
		: next{nullptr}
		, prev{nullptr}
		, data{forward<TT>(inData)}
	{
		//
	}
};

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
	
	using LinkT = Link<T>;
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
	 * Initialize current link
	 */
	FORCE_INLINE ListIterator(LinkT * inLink)
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
	LinkT * link;
};

/**
 * 
 */
template<typename T>
struct ListConstIterator
{
	template<typename> friend class List;

	//////////////////////////////////////////////////
	// Iterator types
	//////////////////////////////////////////////////
	
	using LinkT = Link<T>;
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
	 * Initialize current link
	 */
	FORCE_INLINE ListConstIterator(const LinkT * inNode)
		: link{inNode}
	{
		//
	}

	/**
	 * Cast non-const to const iterator
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
	LinkT * link;
};

/**
 * 
 */
template<typename T>
class List
{
public:
	/// Data type
	using DataT = T;

	/// Link type
	using LinkT = Link<T>;

	/// Iterator type
	using Iterator = ListIterator<T>;

	/// Const iterator type
	using ConstIterator = ListConstIterator<T>;

protected:
	/// Allocator
	MallocBase * malloc;

	/// Has own allocator flag
	bool bHasOwnMalloc;

	/// Pointer to list head
	LinkT * head;

	/// Pointer to list tail
	LinkT * tail;

	/// Length of the list (number of links)
	uint64 length;

public:
	/**
	 * Default constructor, creates empty
	 * list
	 */
	FORCE_INLINE List()
		: malloc{nullptr}
		, bHasOwnMalloc{true} 
		, head{nullptr}
		, tail{nullptr}
		, length{0ull}
	{
		malloc = new MallocAnsi;
	}

	/**
	 * Provide external allocator
	 */
	FORCE_INLINE List(MallocBase * inMalloc)
		: malloc{inMalloc}
		, bHasOwnMalloc{true}
		, head{nullptr}
		, tail{nullptr}
		, length{0ull}
	{
		CHECKF(inMalloc != nullptr, "Invalid allocator provided");
	}

protected:
	/**
	 * Create a single link
	 * 
	 * @param data link data
	 * @return pointer to link
	 */
	template<typename TT>
	FORCE_INLINE LinkT * createLink(TT && a) const
	{
		return new (malloc->alloc(sizeof(LinkT), alignof(LinkT))) LinkT{forward<TT>(a)};
	}
	
	/**
	 * Destroy link, explicitly call
	 * link destructor
	 */
	FORCE_INLINE void destroyLink(LinkT * link) const
	{
		link->~LinkT();
		malloc->free(link);
	}

public:
	/**
	 * Copy constructor
	 */
	List(const List & other)
		: List{}
	{
		LinkT * it = other.head;
		while (it) pushBack(it->data), it = it->next;
	}

	/**
	 * Move constructor
	 */
	FORCE_INLINE List(List && other)
		: malloc{other.malloc}
		, bHasOwnMalloc{other.bHasOwnMalloc}
		, head{other.head}
		, tail{other.tail}
		, length{other.length}
	{
		other.malloc = nullptr;
		other.bHasOwnMalloc = false;
		other.head = other.tail = nullptr;
		other.length = 0ull;
	}

	/**
	 * Copy assignment
	 */
	List & operator=(const List & other)
	{
		LinkT * it = head, * jt = other.head;

		if (jt)
		{
			while (it && jt)
			{
				tail = it;

				// Copy value
				it->data = jt->data;

				// Next
				it = it->next;
				jt = jt->next;
			}
			
			while (it)
			{
				LinkT * link = it;
				it = it->next;

				destroyLink(link);
			}

			// Ensure tail is valid
			if (tail) tail->next = nullptr;

			// Push remaining objects
			while (jt) pushBack(jt->data), jt = jt->next;
		}
		// Corner case: other is empty
		else empty();

		return *this;
	}

	/**
	 * Move assignment
	 */
	List & operator=(List && other)
	{
		// Remove all
		empty();

		// Delete allocator
		if (bHasOwnMalloc) delete malloc;

		// Take from other
		malloc = other.malloc;
		bHasOwnMalloc = other.bHasOwnMalloc;
		head = other.head;
		tail = other.tail;
		length = other.length;

		other.malloc = nullptr;
		other.bHasOwnMalloc = false;
		other.head = other.tail = nullptr;
		other.length = 0ull;
	}

	/**
	 * Remove and destroy all links
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

	/**
	 * Destructor, destroys all links
	 */
	FORCE_INLINE ~List()
	{
		empty();

		if (bHasOwnMalloc)
		{
			delete malloc;

			malloc = nullptr;
			bHasOwnMalloc = false;
		}
	}

	/**
	 * Returns head link
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
	/// @}

	/**
	 * Returns tail link
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
	/// @}

	/**
	 * Returns a new iterator that
	 * points to the beginning of
	 * the list
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
	 * Returns a new iterator that
	 * points to the end of the list
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
	 * Returns list length
	 */
	FORCE_INLINE uint64 getLength() const
	{
		return length;
	}

	/**
	 * Push element(s) to the end of
	 * the array
	 * 
	 * @param arg,args element(s)
	 * @return inserted element
	 * @{
	 */
	template<typename TT>
	T & pushBack(TT && arg)
	{
		LinkT * link = createLink(forward<TT>(arg));

		if (!tail)
			head = tail = link;
		else
		{
			link->prev = tail;
			tail->next = link;
			tail = link;
		}

		++length;
		return link->data;
	}

	template<typename TT, typename ... TTT>
	FORCE_INLINE void pushBack(TT && arg, TTT && ... args)
	{
		pushBack(forward<TT>(arg));
		pushBack(forward<TTT>(args) ...);
	}
	/// @}

	/**
	 * Push element(s) to the beginning
	 * of the array
	 * 
	 * @param arg,args element(s)
	 * @return inserted element
	 * @{
	 */
	template<typename TT>
	FORCE_INLINE T & pushFront(TT && arg)
	{
		LinkT * link = createLink(forward<TT>(arg));

		if (!head)
			head = tail = link;
		else
		{
			link->next = head;
			head->prev = link;
			head = link;
		}

		++length;
		return link->data;
	}

	template<typename TT, typename ... TTT>
	FORCE_INLINE void pushFront(TT && arg, TTT && ... args)
	{
		// Neat trick, since we expect
		// pushFront(a, b, c, d, ...)
		// to result in {a, b, c, d}
		// we first use recursion and
		// so that a is pushed last,
		// after b, c, d, etc.
		pushFront(forward<TTT>(args) ...);
		pushFront(forward<TT>(arg));
	}
	/// @}

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
	 * iterator
	 */
	FORCE_INLINE void remove(ConstIterator it)
	{
		if (it->link) remove(it->link);
	}

	/**
	 * Remove tail element
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
				head = tail = nullptr;
			else
				(tail = tail->prev)->next = nullptr;
			
			--length;
			
			// Destroy link
			destroyLink(link);
			
			return true;
		}
		else return false;
	}

	/**
	 * Remove head element
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
				head = tail = nullptr;
			else
				(head = head->next)->prev = nullptr;
			
			--length;
			
			// Destroy link
			destroyLink(link);
			
			return true;
		}
		else return false;
	}

	/**
	 * Remove tail element and
	 * return its data.
	 * 
	 * @param [in] outData returned tail
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
	 * @param [in] outData returned head
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
};