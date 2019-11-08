#pragma once

#include "core_types.h"
#include "misc/utility.h"
#include "hal/platform_memory.h"
#include "hal/malloc_ansi.h"
#include "templates/utility.h"
#include "templates/allocators.h"
#include "containers/containers_types.h"

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

protected:
	/// Used allocator
	MallocT * allocator;

	/// Has own allocator flag
	bool bHasOwnAlloc;

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
			allocator->free(buffer);
			
			buffer = nullptr;
		}

		count = capacity = 0;

		// Destroy managed allocator
		if (bHasOwnAlloc)
		{
			delete allocator;
			
			allocator = nullptr;
			bHasOwnAlloc = false;
		}
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
			T * inBuffer = ObjectAllocator<T>{allocator}.alloc(capacity);

			// Copy elements and free old buffer
			if (buffer)
			{
				Memory::constructCopyElements(inBuffer, buffer, count);
				allocator->free(buffer);
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
	 * @param [in] inCapacity initial array capacity
	 * @param [in] inCount initial number of default elements
	 * @param [in] allocator allocator not owned by the array
	 */
	Array(uint64 inCapacity, uint64 inCount, MallocT * inAllocator = nullptr)
		: allocator{inAllocator}
		, bHasOwnAlloc{allocator == nullptr}
		, buffer{nullptr}
		, capacity{inCapacity}
		, count{inCount}
	{
		// Create managed allocator
		if (bHasOwnAlloc)
			allocator = new MallocT;

		// Create initial buffer
		if (capacity > 0)
			buffer = ObjectAllocator<T>{allocator}.alloc(capacity);

		// Default construct elements
		if (count > 0)
			Memory::constructDefaultElements(buffer, buffer + count);
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
		: allocator{other.allocator}
		, bHasOwnAlloc{other.bHasOwnAlloc}
		, buffer{other.buffer}
		, capacity{other.capacity}
		, count{other.count}
	{
		other.allocator = nullptr;
		other.bHasOwnAlloc = false;
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

		bHasOwnAlloc = other.bHasOwnAlloc;
		allocator = other.allocator;
		buffer = other.buffer;
		capacity = other.capacity;
		count = other.count;

		other.bHasOwnAlloc = false;
		other.allocator = nullptr;
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
	/// @}

	/**
	 * Insert element at position
	 * 
	 * @param [in] inT element to insert
	 * @return reference to inserted element
	 */
	template<typename TT>
	T & insertAt(TT && inT, uint64 idx)
	{
		const uint64 j = idx + 1;

		if (idx < count)
		{
			resizeIfNecessary(count + 1);

			// Move elements up to accomodate
			Memory::moveElements(buffer + j, buffer + idx, count - j);
			++count;
		}
		else
			resizeIfNecessary(count = j);
		
		// Copy construct element
		return *(new (buffer + idx) T{forward<TT>(inT)});
	}

	/**
	 * Insert a new element at the beginning of
	 * the array
	 * 
	 * @param [in] inT element to insert
	 * @return reference to element
	 */
	template<typename TT>
	T & insertFirst(TT && inT)
	{
		resizeIfNecessary(count + 1);

		// Move elements up
		Memory::moveElements(buffer + 1, buffer, count);
		++count;

		// Copy construct element
		return *(new (buffer) T{forward<TT>(inT)});
	}

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
		return *(new (buffer + count++) T{forward<TT>(inT)});
	}
	METHOD_ALIAS(add, insertLast)
	METHOD_ALIAS(push, insertLast)
	/// @}

	/**
	 * Remove element at index (does not
	 * check index overflow)
	 * 
	 * @param [in] idx element index
	 */
	void removeAt(uint64 idx)
	{
		// Destroy element
		Memory::destroyElements(buffer + idx, buffer + idx);

		// Move elements to the left
		Memory::moveElements(buffer + idx, buffer + idx + 1, (count - 1) - idx);

		// Decrement count
		--count;
	}

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
		allocator->free(buffer);
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