#pragma once

#include "core_types.h"
#include "misc/utility.h"
#include "misc/assert.h"
#include "templates/iterator.h"
#include "templates/types.h"
#include "templates/utility.h"
#include "templates/array.h"
#include "hal/platform_memory.h"
#include "hal/platform_math.h"
#include "hal/malloc_ansi.h"
#include "hal/malloc_object.h"
#include "./containers_types.h"

/**
 * Iterator used to iterate over the
 * items of a dynamic array. it keeps
 * a ref to the underlying array, which
 * makes it non assignable.
 * 
 * @param ArrayT type of array
 * @param IteratorTraitsT type that
 * 	define the iterator types
 */
template<typename ArrayT, typename IteratorTraitsT = ContiguousIterator<typename ArrayT::ItemT>>
struct ArrayIteratorBase : public IteratorTraitsT
{
	using RefT = typename IteratorTraitsT::RefT;
	using PtrT = typename IteratorTraitsT::PtrT;

	/**
	 * Construct for array, pointing at
	 * idx-th element.
	 * 
	 * @param inArray ref to array
	 * @param [inIdx = 0] start index
	 */
	FORCE_INLINE ArrayIteratorBase(ArrayT & inArray, int64 inIdx = 0)
		: array{inArray}
		, idx{inIdx}
	{
		//
	}

	/**
	 * Dereference iterator, return ref to
	 * idx-th item.
	 */
	FORCE_INLINE RefT operator*() const
	{
		return array[idx];
	}

	/**
	 * Dereference iterator, return pointer
	 * to idx-th item.
	 */
	FORCE_INLINE PtrT operator->() const
	{
		return &(**this);
	}

	/**
	 * Compare two iterators, they must both
	 * point to the same item of the same
	 * array. Returns true if they point
	 * to the same item.
	 * 
	 * @param other another iterator
	 * @return true if they point to the
	 * 	same item
	 */
	FORCE_INLINE bool operator==(const ArrayIteratorBase & other) const
	{
		return &(**this) == &(*other);
	}

	/**
	 * Returns true if they don't point to
	 * the same item.
	 * 
	 * @param other another iterator
	 * @return true if they dont' point
	 * 	to the same item
	 */
	FORCE_INLINE bool operator!=(const ArrayIteratorBase & other) const
	{
		return !(*this == other);
	}
	
	/**
	 * Advances iterator to the next item and
	 * returns a ref to the iterator.
	 */
	FORCE_INLINE ArrayIteratorBase & operator++()
	{
		++idx;
		return *this;
	}

	/**
	 * Moves iterator to the next item and
	 * returns a new iterator that points
	 * to the current item instead.
	 */
	FORCE_INLINE ArrayIteratorBase operator++(int32)
	{
		ArrayIteratorBase it{*this};

		++idx;
		return it;
	}

	/**
	 * Moves iterator to the previous item
	 * and returns a ref to the iterator.
	 */
	FORCE_INLINE ArrayIteratorBase & operator--()
	{
		--idx;
		return *this;
	}

	/**
	 * Moves iterator to the previous item
	 * and returns a new iterator that
	 * points to the current item instead.
	 */
	FORCE_INLINE ArrayIteratorBase operator--(int32)
	{
		ArrayIteratorBase it{*this};

		--idx;
		return it;
	}

	/**
	 * Increments iterator by that amount
	 * and returns a ref to it.
	 * 
	 * @param n increment amount
	 * @return ref to self
	 */
	FORCE_INLINE ArrayIteratorBase & operator+=(uint64 n)
	{
		idx += n;
		return *this;
	}

	/**
	 * Returns an iterator incremented by
	 * that amount, without modifying this.
	 * 
	 * @param n increment amount
	 * @return incremented iterator
	 */
	FORCE_INLINE ArrayIteratorBase operator+(uint64 n) const
	{
		ArrayIteratorBase it{*this};
		return (it += n);
	}

	/**
	 * Decrements iterator by that amount
	 * and returns a ref to it.
	 * 
	 * @param n decrement amount
	 * @return ref to self
	 */
	FORCE_INLINE ArrayIteratorBase operator-=(uint64 n)
	{
		idx -= n;
		return *this;
	}

	/**
	 * Returns an iterator decremented by
	 * that amount, without modifying the
	 * original iterator.
	 * 
	 * @param n decrement amount
	 * @return decremented iterator
	 */
	FORCE_INLINE ArrayIteratorBase operator-(uint64 n) const
	{
		ArrayIteratorBase it{*this};
		return (it -= n);
	}

	/**
	 * Subscript operation, returns i-th
	 * element realtive to this iterator.
	 * 
	 * @param i index relative to this
	 * 	iterator
	 * @return ref to i-th item
	 */
	FORCE_INLINE RefT operator[](uint64 i) const
	{
		return array[idx + i];
	}

	/**
	 * Ordering operations.
	 * @see https://en.cppreference.com/w/cpp/named_req/RandomAccessIterator
	 * 
	 * Per spec, at least one of the
	 * following comparison should be
	 * true:

	 * ```
	 * a < b || a > b || a == b
	 * ```
	 * 
	 * @param other another iterator
	 * @return true if the comparison
	 * 	stands
	 * @{
	 */
	FORCE_INLINE bool operator<(const ArrayIteratorBase & other) const
	{
		// We don't care about array, otherwise
		// we'd fail to comply to the specs
		return &(**this) < &(*other);
	}

	FORCE_INLINE bool operator>(const ArrayIteratorBase & other) const
	{
		return &(**this) > &(*other);
	}

	FORCE_INLINE bool operator<=(const ArrayIteratorBase & other) const
	{
		return !(*this > other);
	}

	FORCE_INLINE bool operator>=(const ArrayIteratorBase & other) const
	{
		return !(*this < other);
	}
	/** @} */

private:
	/// Underlying array
	ArrayT & array;

	/// Current index
	int64 idx;
};

/// Array iterator types
/// @{
template<typename ArrayT>
using ArrayConstIterator = ArrayIteratorBase<const ArrayT, ContiguousIterator<const typename ArrayT::ItemT>>;

template<typename ArrayT>
using ArrayIterator = ArrayIteratorBase<ArrayT, ContiguousIterator<typename ArrayT::ItemT>>;
/// @}

/**
 * An array is a collection of elements
 * of the same type T stored in a contiguos
 * memory block.
 * 
 * @param T type of the items of the array
 */
template<typename T>
class Array<T, void>
{
	template<typename, typename>	friend class Array;
	template<typename>				friend class StringBase;
	
public:
	using ArrayT = Array<T, void>;
	using ItemT = T;
	using ConstIterator = ArrayConstIterator<ArrayT>;
	using Iterator = ArrayIterator<ArrayT>;
	template<uint64 count> using StaticArrayT = StaticArray<T, count>;

protected:
	/**
	 * Destroys array, deallocates all
	 * items.
	 */
	FORCE_INLINE void destroy()
	{
		if (buffer)
		{
			// Destroy elements and deallocate buffer
			Memory::destroyElements(buffer, buffer + count);

			malloc.free(buffer);
			buffer = nullptr;
		}

		count = capacity = 0;
	}

	/**
	 * Resizes buffer if new capacity exceeds
	 * current capacity.
	 * 
	 * @param inCapacity required capacity
	 * @return true if buffer was resized
	 */
	FORCE_INLINE bool resizeIfNecessary(uint64 inCapacity)
	{
		if (inCapacity > capacity)
		{
			// Compute minimum capacity according to policy
			capacity = capacity ? capacity : 2;
			while (capacity < inCapacity) capacity *= 2;

			// Allocate new memory
			T * inBuffer = malloc.alloc(capacity);

			// Copy elements and free old buffer
			if (buffer)
			{
				Memory::constructCopyElements(inBuffer, buffer, count);
				malloc.free(buffer);
			}

			buffer = inBuffer;
			return true;
		}

		return false;
	}

public:
	/**
	 * Default constructor
	 */
	FORCE_INLINE Array()
		: malloc{}
		, buffer{nullptr}
		, capacity{0}
		, count{0}
	{
		//
	}

	/**
	 * Default constructor with custom
	 * allocator.
	 * 
	 * @param inMalloc pointer to external
	 * 	allocator
	 */
	FORCE_INLINE explicit Array(MallocBase * inMalloc)
		: malloc{inMalloc}
		, buffer{nullptr}
		, capacity{0}
		, count{0}
	{
		//
	}

	/**
	 * Initialize array capacity and count,
	 * and default construct items.
	 * 
	 * @param inCapacity array max capacity
	 */
	FORCE_INLINE Array(uint64 inCapacity, uint64 inCount = 0)
		: malloc{}
		, buffer{nullptr}
		, capacity{inCapacity}
		, count{inCount}
	{
		// Create buffer of capacity
		buffer = malloc.alloc(inCapacity);
		
		if (count > 0)
		{
			// Default construct all items
			Memory::constructDefaultElements(buffer, buffer + count);
		}
	}

	/**
	 * Buffer constructor
	 * 
	 * @param inBuffer source buffer
	 * @param inCount elements count
	 * @param slack extra space required
	 */
	FORCE_INLINE Array(const T * inBuffer, uint64 inCount, uint64 slack = 0)
		: Array{inCount + slack, inCount}
	{
		CHECK(buffer != nullptr)

		// Copy source buffer to destination
		if (inBuffer && count > 0) Memory::constructCopyElements(buffer, inBuffer, count);
	}

	/**
	 * Construct a dynamic array from
	 * a static array.
	 */
	template<uint64 inCount>
	FORCE_INLINE Array(const StaticArrayT<inCount> & inArray)
		: Array{*inArray, inCount}
	{
		//
	}

	/**
	 * Copy constructor.
	 */
	Array(const Array & other)
		: Array{other.buffer, other.count, 0}
	{
		//
	}

	/**
	 * Move constructor.
	 */
	Array(Array && other)
		: malloc{move(other.malloc)}
		, buffer{other.buffer}
		, capacity{other.capacity}
		, count{other.count}
	{
		other.buffer = nullptr;
		other.capacity = 0;
		other.count = 0;
	}

	/**
	 * Array destructor, destroys items and
	 * deallocates buffer.
	 */
	FORCE_INLINE ~Array()
	{
		destroy();
	}

	/**
	 * Copy assignment.
	 */
	Array & operator=(const Array & other)
	{
		// Resize buffer
		resizeIfNecessary(other.count);

		if (other.count < count)
		{
			// Copy elements and destroy the extra elements
			Memory::copyElements(buffer, other.buffer, other.count);
			Memory::destroyElements(buffer + other.count, buffer + count);
		}
		else
		{
			// Copy elements and construct the extra elements
			Memory::copyElements(buffer, other.buffer, count);
			Memory::constructCopyElements(buffer + count, other.buffer + count, other.count - count);
		}

		// Set new count
		count = other.count;
		
		return *this;
	}

	/**
	 * Move assignment.
	 */
	Array & operator=(Array && other)
	{
		// Destroy first
		destroy();
		
		malloc = move(other.malloc);
		buffer = other.buffer;
		capacity = other.capacity;
		count = other.count;

		other.buffer = nullptr;
		other.capacity = other.count = 0;

		return *this;
	}

	/**
	 * Returns number of elements-
	 * @{
	 */
	FORCE_INLINE uint64 getCount() const
	{
		return count;
	}

	METHOD_ALIAS_CONST(getSize, getCount)
	METHOD_ALIAS_CONST(getNum, getCount)
	/** @} */

	/**
	 * Returns true if array is empty.
	 */
	FORCE_INLINE bool isEmpty() const
	{
		return count == 0;
	}

	/**
	 * Returns total data size (in Bytes).
	 */
	FORCE_INLINE sizet getBytes() const
	{
		return count * sizeof(T);
	}

	/**
	 * Returns pointer to array data.
	 * @{
	 */
	FORCE_INLINE T * operator*()
	{
		return buffer;
	}
	METHOD_ALIAS(getData, operator*)
	METHOD_ALIAS(getBuffer, operator*)

	FORCE_INLINE const T * operator*() const
	{
		return buffer;
	}

	METHOD_ALIAS_CONST(getData, operator*)
	METHOD_ALIAS_CONST(getBuffer, operator*)
	/** @} */

	/**
	 * Returns array item at idx-th position
	 * 
	 * @param idx item position
	 * @return reference to element
	 * @{
	 */
	FORCE_INLINE T & operator[](uint64 idx)
	{
		return buffer[idx];
	}

	METHOD_ALIAS(getAt, operator[])

	FORCE_INLINE const T & operator[](uint64 idx) const
	{
		return buffer[idx];
	}

	METHOD_ALIAS_CONST(getAt, operator[])
	/** @} */

	/**
	 * @brief Returns array item at idx-th
	 * position.
	 * If array size is lower then `idx`,
	 * allocate required space and return
	 * ref to idx-th position (non constructed).
	 * 
	 * @param idx item position
	 * @param initValue if item does not
	 * 	exist, init with value
	 * @return ref to item or to created space
	 * @{
	 */
	FORCE_INLINE T & operator()(uint64 idx)
	{
		// Resize to fit idx
		resizeIfNecessary(idx + 1);
		count = PlatformMath::max(count, idx + 1);

		return buffer[idx];
	}

	template<typename ItemT>
	FORCE_INLINE T & operator()(uint64 idx, ItemT && initValue)
	{
		if (idx >= count)
		{
			return *(new (&(operator()(idx))) T{forward<ItemT>(initValue)});
		}
		
		return operator()(idx);
	}
	/** @} */

	/**
	 * Returns a new iterator that points
	 * to the first element
	 * @{
	 */
	FORCE_INLINE ConstIterator begin() const
	{
		return ConstIterator{*this, 0};
	}

	FORCE_INLINE Iterator begin()
	{
		return Iterator{*this, 0};
	}
	/** @} */

	/**
	 * Returns a new iterator that points
	 * to the end of the array
	 * @{
	 */
	FORCE_INLINE ConstIterator end() const
	{
		return ConstIterator{*this, static_cast<int64>(count)};
	}

	FORCE_INLINE Iterator end()
	{
		return Iterator{*this, static_cast<int64>(count)};
	}
	/** @} */

protected:
	/**
	 * Insert item without resizing.
	 * 
	 * @param idx position in which to insert
	 * @param item item to insert
	 * @return ref to inserted item
	 */
	template<typename ItemT>
	FORCE_INLINE T & insertNoResize(uint64 idx, ItemT && item)
	{
		return *(new (buffer + idx) T{forward<ItemT>(item)});
	}

	/**
	 * Insert multiple elements without
	 * resizing container.
	 * 
	 * @param idx position in which to insert
	 * 	current item
	 * @param item,items items to insert
	 */
	template<typename ItemT, typename ... ItemListT>
	FORCE_INLINE void insertNoResize(uint64 idx, ItemT && item, ItemListT && ... items)
	{
		new (buffer + idx) T{forward<ItemT>(item)};
		insertNoResize(idx + 1, forward<ItemListT>(items) ...);
	}

public:
	/**
	 * Insert item(s) at position.
	 * 
	 * @param idx array position
	 * @param item.items item(s) to insert
	 * @return reference to inserted element
	 * @{
	 */
	template<typename ItemT>
	T & insertAt(uint64 idx, ItemT && item)
	{
		const uint64 j = idx + 1;

		if (idx < count)
		{
			resizeIfNecessary(count + 1);

			// Move elements up to accomodate
			Memory::moveElements(buffer + j, buffer + idx, count - idx);
			++count;
		}
		else resizeIfNecessary(count = j);
		
		// Copy construct element
		return insertNoResize(idx, forward<ItemT>(item));
	}

	template<typename ItemT, typename ... ItemListT>
	void insertAt(uint64 idx, ItemT && item, ItemListT && ... items)
	{
		// Number of elements to insert
		constexpr uint64 numArgs = 1 + sizeof... (ItemListT);
		const uint64 j = idx + numArgs;

		if (idx < count)
		{
			resizeIfNecessary(count + numArgs);

			// Move element up to accomodate
			Memory::moveElements(buffer + j, buffer + idx, count - idx);
			count += numArgs;
		}
		else resizeIfNecessary((count = j));
		
		// Copy construct elements
		insertNoResize(idx, forward<ItemT>(item), forward<ItemListT>(items) ...);
	}
	/** @} */

	/**
	 * Inserts one or more items at the
	 * beginning of the array.
	 * 
	 * @param items,items item(s) to insert
	 * @return ref to inserted item
	 * @{
	 */
	template<typename ItemT>
	T & insertFirst(ItemT && item)
	{
		resizeIfNecessary(count + 1);

		// Move elements up
		Memory::moveElements(buffer + 1, buffer, count);
		++count;

		// Copy construct element
		return insertNoResize(0, forward<ItemT>(item));
	}

	template<typename ItemT, typename ... ItemListT>
	void insertFirst(ItemT && item, ItemListT && ... items)
	{
		constexpr uint64 numArgs = 1 + sizeof... (items);

		// Resize and move elements up
		resizeIfNecessary(count + numArgs);
		Memory::moveElements(buffer + numArgs, buffer, count);
		count += numArgs;

		insertNoResize(0, forward<ItemT>(item), forward<ItemListT>(items) ...);
	}
	/// @}

	/**
	 * Insert one or more items at the
	 * end of the array
	 * 
	 * @param item,items item(s) to insert
	 * @return ref to inserted item
	 * @{
	 */
	template<typename ItemT>
	T & insertLast(ItemT && item)
	{
		resizeIfNecessary(count + 1);
		return insertNoResize(count++, forward<ItemT>(item));
	}

	template<typename ItemT, typename ... ItemListT>
	void insertLast(ItemT && item, ItemListT && ... items)
	{
		constexpr uint64 numArgs = 1 + sizeof... (items);

		// Resize and copy construct elements
		resizeIfNecessary(count + numArgs);
		insertNoResize(count, forward<ItemT>(item), forward<ItemListT>(items) ...);

		count += numArgs;
	}

	METHOD_ALIAS(add, insertLast)
	METHOD_ALIAS(push, insertLast)
	/** @} */

	/**
	 * Removes items at index (does not
	 * check index overflow).
	 * 
	 * @param idx element index
	 * @param [num=1] if provided, number
	 * 	of elements to be removed
	 */
	void removeAt(uint64 idx, uint64 num = 1)
	{
		if (num > 0)
		{
			// Destroy element
			Memory::destroyElements(buffer + idx, buffer + idx + num);

			// Move elements to the left
			Memory::moveElements(buffer + idx, buffer + idx + num, count - (idx + num));

			// Decrement count
			count -= num;
		}
	}

	/**
	 * Removes first element.
	 */
	FORCE_INLINE void removeFirst()
	{
		removeAt(0);
	}

	/**
	 * Removes last element.
	 */
	void removeLast()
	{
		// Decrement count
		--count;

		// Destroy element
		Memory::destroyElements(buffer + count, buffer + count);
	}

	/**
	 * Copys item out and removes it.
	 * 
	 * @param idx element idx
	 * @param outItem item copied out
	 */
	FORCE_INLINE void popAt(uint64 idx, T & outItem)
	{
		// Move out element
		outItem = move(buffer[idx]);

		// Remove it
		removeAt(idx);
	}

	/**
	 * Pop first element.
	 * @see popAt
	 */
	FORCE_INLINE void popFirst(T & outItem)
	{
		popAt(0, outItem);
	}

	/**
	 * Pop last element.
	 * @see PopAt
	 */
	FORCE_INLINE void popLast(T & outItem)
	{
		popAt(count - 1, outItem);
	}

	/**
	 * Wipes out array content.
	 * @{
	 */
	FORCE_INLINE void empty()
	{
		// Destroy all elements
		Memory::destroyElements(buffer, buffer + count);
		count = 0;
	}

	METHOD_ALIAS(wipe, empty)
	METHOD_ALIAS(clear, empty)
	/** @} */

	/**
	 * Resets array.
	 * Destroys all elements and resets storage.
	 */
	FORCE_INLINE void reset()
	{
		// Empty array
		empty();

		// Deallocate storage
		if (buffer)
		{
			malloc.free(buffer);
			buffer = nullptr;
		}
		
		count = capacity = 0;
	}

	/**
	 * Prints array to string.
	 */
	String toString() const;

protected:
	/// Object allocator
	MallocObject<T> malloc;

	/// Memory support
	T * buffer;

	/// Current max number of elements
	uint64 capacity;

	/// Number of elements
	uint64 count;
};

/**
 * Generalization that handles allocator
 * management.
 * 
 * An allocator of the specified type is
 * created on the stack and passed to the
 * array itself. After the array is
 * destructed the allocator is destructed
 * as well.
 * 
 * @param T type of items
 * @param MallocT allocator type
 */
template<typename T, typename MallocT>
class Array : public Array<T, void>
{
	static_assert(IsBaseOf<MallocBase, MallocT>::value, "Allocator must be a subclass of MallocBase");

	using Base = Array<T, void>;

public:
	/**
	 * Constructs allocator and creates an
	 * empty array that uses it.
	 * 
	 * @param createArgs allocator creation
	 * 	arguments
	 */
	template<typename ...MallocCreateArgsT>
	FORCE_INLINE Array(MallocCreateArgsT && ...mallocCreateArgs)
		: Base{&autoMalloc}
		, autoMalloc{forward<MallocCreateArgsT>(mallocCreateArgs)...}
	{
		//
	}

	/**
	 * Destructor, destroy array buffer
	 * here, because we need allocator,
	 * and thus it cannot happend after
	 * this function.
	 */
	FORCE_INLINE ~Array()
	{
		Base::destroy();
	}

protected:
	/// Managed allocator
	MallocT autoMalloc;
};
