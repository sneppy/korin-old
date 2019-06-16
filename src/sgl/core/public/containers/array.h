#pragma once

#include "core_types.h"
#include "containers/containers_fwd.h"
#include "misc/utility.h"
#include "templates/utility.h"
#include "hal/platform_memory.h"
#include "hal/malloc_ansi.h"

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
	 * Default constructs elements in range
	 * 
	 * @param [in] begin,end array range
	 */
	FORCE_INLINE void constructDefaultElements(T * begin, T * end)
	{
		for (; begin != end; ++begin)
			new (begin) T();
	}
	
	/**
	 * Copy elements to empty storage
	 * 
	 * @param [in] dst,src destination and source
	 * @param [in] n number of elements to copy
	 * @{
	 */
	template<typename _T>
	FORCE_INLINE typename EnableIf<!IsTriviallyConstructible<_T>::value>::Type constructCopyElements(_T * RESTRICT dst, const _T * RESTRICT src, uint64 n)
	{
		for (uint32 i = 0; i < n; ++i)
			new (dst + i) _T(src[i]);
	}

	template<typename _T>
	FORCE_INLINE typename EnableIf<IsTriviallyConstructible<_T>::value>::Type constructCopyElements(_T * RESTRICT dst, const _T * RESTRICT src, uint64 n)
	{
		Memory::memcpy(dst, src, n * sizeof(_T));
	}
	///@}

	/**
	 * Destroy 
	 */

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
		if (capacity)
		{
			const sizet bufferSize = capacity * sizeof(T);
			buffer = reinterpret_cast<T*>(allocator->alloc(bufferSize, alignof(T)));
		}

		// Default construct elements
		if (count)
			constructDefaultElements(buffer, buffer + count);
	}

	/**
	 * Copy constructor with same allocator
	 * 
	 * Performs a copy of all elements
	 */
	Array(const Array & other)
		: Array(other.capacity, 0, nullptr)
	{
		// Set count manually (we don't want to default construct)
		count = other.count;

		// Copy elements
		if (count)
			constructCopyElements(buffer, other.buffer, count);
	}

	/**
	 * Copy constructor with different allocator type
	 * 
	 * Performs a copy of all elements
	 */
	template<typename MallocU>
	Array(const Array<T, MallocU> & other)
		: Array(other.capacity, 0, nullptr)
	{
		// Set count manually
		count = other.count;

		// Copy elements
		if (count)
			constructCopyElements(buffer, other.buffer, count);
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
	 * Default constructor
	 */
	FORCE_INLINE Array()
		: Array(0, 0, nullptr)
	{
		//
	}

	/**
	 * Array destructor, destroys elements,
	 * deallocates buffer and destroys managed
	 * allocator
	 */
	FORCE_INLINE ~Array()
	{
		if (buffer)
		{
			allocator->free(buffer);

			// TODO: Destroy elements
		}

		// Destroy managed allocator
		if (bHasOwnAlloc) delete allocator;
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

protected:
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
			T * inBuffer = reinterpret_cast<T*>(allocator->alloc(capacity * sizeof(T), alignof(T)));

			// Copy elements and free old buffer
			if (buffer)
			{
				constructCopyElements(inBuffer, buffer, count);
				allocator->free(buffer);
			}

			buffer = inBuffer;
			return true;
		}

		return false;
	}

public:
	/**
	 * Insert a new element at the end of the
	 * array
	 * 
	 * @param [in] t element to insert
	 * @return reference to element
	 */
	template<typename _T>
	T & add(_T && t)
	{
		resizeIfNecessary(count + 1);
		return *(new (buffer + count++) T(forward<_T>(t)));
	}
};