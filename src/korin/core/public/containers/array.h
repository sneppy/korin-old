#pragma once

#include "../core_types.h"
#include "../misc/utility.h"
#include "../misc/assert.h"
#include "../hal/platform_memory.h"
#include "../hal/platform_math.h"
#include "../hal/malloc_ansi.h"
#include "../hal/malloc_object.h"
#include "../templates/utility.h"
#include "containers_types.h"

/**
 * 
 */
template<typename T>
struct ArrayIterator
{
	template<typename, typename> class Array;

	//////////////////////////////////////////////////
	// Iterator types
	//////////////////////////////////////////////////
	
	using RefT = T&;
	using PtrT = T*;

	/**
	 * Default constructor, removed
	 */
	ArrayIterator() = delete;

	/**
	 * Initialize pointer
	 */
	FORCE_INLINE ArrayIterator(T * inPtr)
		: ptr{inPtr}
	{
		//
	}

	//////////////////////////////////////////////////
	// BaseIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE RefT operator*() const
	{
		return *ptr;
	}

	FORCE_INLINE PtrT operator->() const
	{
		return ptr;
	}

	FORCE_INLINE bool operator==(const ArrayIterator & other) const
	{
		return ptr == other.ptr;
	}

	FORCE_INLINE bool operator!=(const ArrayIterator & other) const
	{
		return !(*this == other);
	}

	//////////////////////////////////////////////////
	// ForwardIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE ArrayIterator & operator++()
	{
		++ptr;
		return *this;
	}

	FORCE_INLINE ArrayIterator operator++(int)
	{
		return ArrayIterator{ptr++};
	}

	//////////////////////////////////////////////////
	// BidirectionalIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE ArrayIterator & operator--()
	{
		--ptr;
		return *this;
	}

	FORCE_INLINE ArrayIterator operator--(int)
	{
		return ArrayIterator{ptr--};
	}

	//////////////////////////////////////////////////
	// RandomAccessIterator interface
	//////////////////////////////////////////////////
	
	// TODO
	
protected:
	/// Array pointer
	T * ptr;
};

/**
 * 
 */
template<typename T>
struct ArrayConstIterator
{
	template<typename, typename> class Array;

	//////////////////////////////////////////////////
	// Iterator types
	//////////////////////////////////////////////////
	
	using RefT = const T&;
	using PtrT = const T*;

	/**
	 * Default constructor, removed
	 */
	ArrayConstIterator() = delete;

	/**
	 * Initialize pointer
	 */
	FORCE_INLINE ArrayConstIterator(const T * inPtr)
		: ptr{inPtr}
	{
		//
	}

	/**
	 * Cast non-const iterator to
	 * const iterator
	 */
	FORCE_INLINE ArrayConstIterator(const ArrayIterator<T> & other)
		: ptr{other.ptr}
	{
		//
	}

	//////////////////////////////////////////////////
	// BaseIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE RefT operator*() const
	{
		return *ptr;
	}

	FORCE_INLINE PtrT operator->() const
	{
		return ptr;
	}

	FORCE_INLINE bool operator==(const ArrayConstIterator & other) const
	{
		return ptr == other.ptr;
	}

	FORCE_INLINE bool operator!=(const ArrayConstIterator & other) const
	{
		return !(*this == other);
	}

	//////////////////////////////////////////////////
	// ForwardIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE ArrayConstIterator & operator++()
	{
		++ptr;
		return *this;
	}

	FORCE_INLINE ArrayConstIterator operator++(int)
	{
		return ArrayConstIterator{ptr++};
	}

	//////////////////////////////////////////////////
	// BidirectionalIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE ArrayConstIterator & operator--()
	{
		--ptr;
		return *this;
	}

	FORCE_INLINE ArrayConstIterator operator--(int)
	{
		return ArrayConstIterator{ptr--};
	}

	//////////////////////////////////////////////////
	// RandomAccessIterator interface
	//////////////////////////////////////////////////
	
	// TODO
	
protected:
	/// Array pointer
	const T * ptr;
};

/**
 * An array is a collection of elements
 * of the same type T stored in a contiguos
 * memory block.
 */
template<typename T, typename MallocT = MallocAnsi>
class Array
{
	template<typename, typename>	friend class Array;
									friend String;
	
public:
	/// Iterator type
	using Iterator = ArrayIterator<T>;
	
	/// Const iterator type
	using ConstIterator = ArrayConstIterator<T>;

protected:
	/// Object allocator
	MallocObject<T, MallocT> malloc;

	/// Memory support
	T * buffer;

	/// Current max number of elements
	uint64 capacity;

	/// Number of elements
	uint64 count;

protected:
	/**
	 * Destroy array, also deallocates
	 * managed allocator
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
	 * Resize buffer if new capacity exceeds
	 * current capacity
	 * 
	 * @param [in] inCapacity required capacity
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
	 * Init constructor
	 * 
	 * @param inCapacity initial array capacity
	 * @param inCount initial number of default elements
	 * @param inAllocator allocator not owned by the array
	 */
	explicit Array(uint64 inCapacity, uint64 inCount = 0, MallocT * inAllocator = nullptr)
		: malloc{inAllocator}
		, buffer{nullptr}
		, capacity{inCapacity}
		, count{inCount}
	{
		// Create initial buffer
		if (capacity > 0)
			buffer = malloc.alloc(capacity);

		// Default construct elements
		if (count > 0)
			Memory::constructDefaultElements(buffer, buffer + count);
	}

	/**
	 * List constructor
	 * 
	 * @param a, b, c list items
	 */
	/* template<typename TT, typename ... TTT>
	Array(TT && a, typename EnableIf<IsSameType<TT, T>::value, TT>::Type && b, TTT && ... c)
		: Array{2 + sizeof ... (c), 2 + sizeof ... (c), nullptr}
	{
		// Insert elements
		insertNoResize(0, forward<TT>(a), forward<TT>(b), forward<TTT>(c) ...);
	} */

	/**
	 * Init constructor with malloc arguments
	 * 
	 * @param inCapacity initial array capacity
	 * @param inCount initial number of default elements
	 * @param inMalloc temporary object allocator
	 */
	template<typename ... MallocArgs>
	Array(uint64 inCapacity, uint64 inCount, MallocObject<T, MallocT> && inMalloc)
		: malloc{move(inMalloc)}
		, buffer{nullptr}
		, capacity{inCapacity}
		, count{inCount}
	{
		// Create initial buffer
		if (capacity > 0)
			buffer = malloc.alloc(capacity);

		// Default construct elements
		if (count > 0)
			Memory::constructDefaultElements(buffer, buffer + count);
	}

	/**
	 * Tuple constructor, copies tuple values
	 * in array. Use the conversion operator
	 * if you don't want to specify an allocator.
	 * 
	 * @param tuple tuple object
	 * @param inMalloc temporary objet allocator
	 */
	template<sizet n>
	Array(const Tuple<T, n> & tuple, MallocObject<T, MallocT> && inMalloc)
		: Array{n, n, move(inMalloc)}
	{
		// Copy construct elements
		Memory::constructCopyElements(buffer, tuple.values, n);
	}

	/**
	 * Default constructor
	 */
	FORCE_INLINE Array()
		: Array{0, 0, nullptr}
	{
		//
	}

	/**
	 * Buffer constructor
	 * 
	 * @param [in] src source buffer
	 * @param [in] inCount elements count
	 * @param [in] slack extra space
	 * @param [in] inAllocator optional allocator
	 */
	FORCE_INLINE Array(const T * src, uint64 inCount, uint64 slack = 0, MallocT * inAllocator = nullptr)
		: Array{inCount + slack, inCount, inAllocator}
	{
		CHECK(buffer != nullptr)

		// Copy source buffer to destination
		if (count) Memory::constructCopyElements(buffer, src, count);
	}

	/**
	 * Copy constructor with same allocator
	 * 
	 * Performs a copy of all elements
	 * @{
	 */
	Array(const Array & other)
		: Array{other.buffer, other.count, 0, nullptr}
	{
		//
	}

	/**
	 * Copy constructor with different allocator type
	 * 
	 * Performs a copy of all elements
	 */
	template<typename MallocU>
	Array(const Array<T, MallocU> & other)
		: Array{other.buffer, other.count, 0, nullptr}
	{
		//
	}

	/**
	 * Move constructor
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
	 * Array destructor, destroys elements,
	 * deallocates buffer and destroys managed
	 * allocator
	 */
	FORCE_INLINE ~Array()
	{
		destroy();
	}

	/**
	 * Copy assignment with same allocator
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
	 * Copy assignment with different allocator
	 */
	template<typename MallocU>
	Array & operator=(const Array<T, MallocU> & other)
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
	 * Move assignment
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
	 * Returns number of elements
	 * @{
	 */
	FORCE_INLINE uint64 getCount() const
	{
		return count;
	}
	METHOD_ALIAS_CONST(getSize, getCount)
	METHOD_ALIAS_CONST(getNum, getCount)
	/// @}

	/**
	 * Returns true if array is empty
	 */
	FORCE_INLINE bool isEmpty() const
	{
		return count == 0;
	}

	/**
	 * Returns total data size (in Bytes)
	 */
	FORCE_INLINE sizet getBytes() const
	{
		return count * sizeof(T);
	}

	/**
	 * Returns pointer to array data
	 * @{
	 */
	FORCE_INLINE T * operator*()
	{
		return buffer;
	}
	METHOD_ALIAS(getData, operator*)

	FORCE_INLINE const T * operator*() const
	{
		return buffer;
	}
	METHOD_ALIAS_CONST(getData, operator*)
	/// @}

	/**
	 * Returns array element at idx-th position
	 * 
	 * @param [in] idx element position
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
	 * @return ref to item or to created space
	 */
	FORCE_INLINE T & operator()(uint64 idx)
	{
		// Resize to fit idx
		resizeIfNecessary(idx + 1);
		count = PlatformMath::max(count, idx + 1);

		return buffer[idx];
	}

	/**
	 * Returns a new iterator that points
	 * to the first element
	 * @{
	 */
	FORCE_INLINE ConstIterator begin() const
	{
		return ConstIterator{buffer};
	}

	FORCE_INLINE Iterator begin()
	{
		return Iterator{buffer};
	}
	/// @}

	/**
	 * Returns a new iterator that points
	 * to the end of the array
	 * @{
	 */
	FORCE_INLINE ConstIterator end() const
	{
		return ConstIterator{buffer + count};
	}

	FORCE_INLINE Iterator end()
	{
		return Iterator{buffer + count};
	}
	/// @}

protected:
	/**
	 * Insert element without resizing.
	 * 
	 * @param idx position in which to insert
	 * @param t element to insert
	 * @return ref to inserted element
	 */
	template<typename TT>
	FORCE_INLINE T & insertNoResize(uint64 idx, TT && t)
	{
		return *(new (buffer + idx) T{forward<TT>(t)});
	}

	/**
	 * Insert multiple elements without
	 * resizing container.
	 * 
	 * @param idx position in which to insert
	 * @param a,b,c elements to insert
	 */
	template<typename TT, typename ... TTT>
	FORCE_INLINE void insertNoResize(uint64 idx, TT && a, TT && b, TTT && ... c)
	{
		new (buffer + idx) T{forward<TT>(a)};
		insertNoResize(idx + 1, forward<TT>(b), forward<TTT>(c) ...);
	}

public:
	/**
	 * Insert element(s) at position
	 * 
	 * @param idx array position
	 * @param t,a,b,c element(s) to insert
	 * @return reference to inserted element
	 * @{
	 */
	template<typename TT>
	T & insertAt(uint64 idx, TT && t)
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
		return insertNoResize(idx, forward<TT>(t));
	}

	template<typename TT, typename ... TTT>
	void insertAt(uint64 idx, TT && a, TTT && ... b)
	{
		// Number of elements to insert
		constexpr uint64 numArgs = 1 + sizeof ... (TTT);
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
		insertNoResize(idx, forward<TT>(a), forward<TTT>(b) ...);
	}

	/* template<typename It>
	void insertAt(uint64 idx, It begin, It end)
	{
		// For iterators
	} */
	/// @}

	/**
	 * Insert a new element at the beginning of
	 * the array
	 * 
	 * @param [in] t,a,b,c element(s) to insert
	 * @return reference to element
	 * @{
	 */
	template<typename TT>
	T & insertFirst(TT && t)
	{
		resizeIfNecessary(count + 1);

		// Move elements up
		Memory::moveElements(buffer + 1, buffer, count);
		++count;

		// Copy construct element
		return insertNoResize(0, forward<TT>(t));
	}

	template<typename TT, typename ... TTT>
	void insertFirst(TT && a, TTT && ... b)
	{
		constexpr uint64 numArgs = 1 + sizeof ... (b);

		// Resize and move elements up
		resizeIfNecessary(count + numArgs);
		Memory::moveElements(buffer + numArgs, buffer, count);
		count += numArgs;

		insertNoResize(0, forward<TT>(a), forward<TTT>(b) ...);
	}
	/// @}

	/**
	 * Insert a new element at the end of the
	 * array
	 * 
	 * @param [in] inT element to insert
	 * @return reference to element
	 * @{
	 */
	template<typename TT>
	T & insertLast(TT && inT)
	{
		resizeIfNecessary(count + 1);
		return insertNoResize(count++, forward<TT>(inT));
	}

	template<typename TT, typename ... TTT>
	void insertLast(TT && a, TTT && ... b)
	{
		constexpr uint64 numArgs = 1 + sizeof ... (b);

		// Resize and copy construct elements
		resizeIfNecessary(count + numArgs);
		insertNoResize(count, forward<TT>(a), forward<TTT>(b) ...);

		count += numArgs;
	}
	METHOD_ALIAS(add, insertLast)
	METHOD_ALIAS(push, insertLast)
	/// @}

	/**
	 * Remove element at index (does not
	 * check index overflow)
	 * 
	 * @param idx element index
	 * @param [len=1] if provided, number
	 * 	of elements to be removed
	 */
	void removeAt(uint64 idx, uint64 len = 1)
	{
		if (len > 0)
		{
			// Destroy element
			Memory::destroyElements(buffer + idx, buffer + idx + len);

			// Move elements to the left
			Memory::moveElements(buffer + idx, buffer + idx + len, count - (idx + len));

			// Decrement count
			count -= len;
		}
	}
	/// @}

	/**
	 * Remove first element
	 */
	FORCE_INLINE void removeFirst()
	{
		removeAt(0);
	}

	/**
	 * Remove last element
	 */
	void removeLast()
	{
		// Decrement count
		--count;

		// Destroy element
		Memory::destroyElements(buffer + count, buffer + count);
	}

	/**
	 * Copy element out and remove it
	 * 
	 * @param [in] idx element idx
	 * @param [out] outT out element
	 */
	FORCE_INLINE void popAt(uint64 idx, T & outT)
	{
		// Move out element
		outT = move(buffer[idx]);

		// Remove it
		removeAt(idx);
	}

	/**
	 * Pop first element
	 */
	FORCE_INLINE void popFirst(uint64 idx, T & outT)
	{
		popAt(0, outT);
	}

	/**
	 * Pop last element
	 */
	FORCE_INLINE void popLast(uint64 idx, T & outT)
	{
		// Move out element
		outT = move(buffer[idx]);

		// Remove it
		removeLast();
	}

	/**
	 * Wipe out array content
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
	/// @}

	/**
	 * Reset array
	 * Destroy all elements and reset storage
	 */
	FORCE_INLINE void reset()
	{
		// Empty array
		empty();

		// Deallocate storage
		malloc.free(buffer);
		buffer = nullptr;
		capacity = 0;
	}

	/**
	 * Print array to string
	 * 
	 * @return formatted string
	 */
	String toString() const;
};