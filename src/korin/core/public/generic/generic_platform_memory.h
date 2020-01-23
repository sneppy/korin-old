#pragma once

#include "core_types.h"
#include "hal/platform_crt.h"
#include "../templates/utility.h"

/**
 * Platform independent memory definitions
 */
struct GenericPlatformMemory
{
	//////////////////////////////////////////////////
	// Standard C memory utils
	//////////////////////////////////////////////////

	/**
	 * Copy memory from source to destination
	 * 
	 * @param [in] dst destination memory
	 * @param [in] src source memory
	 * @param [in] size memory size (in Bytes)
	 * @return destination address
	 */
	static FORCE_INLINE void * memcpy(void * RESTRICT dst, const void * RESTRICT src, sizet size)
	{
		return ::memcpy(dst, src, size);
	}
	
	/**
	 * Copy memory using an intermediate buffer
	 * allowing destination and source to overlap
	 * 
	 * @param [in] dst destination memory
	 * @param [in] src source memory
	 * @param [in] size memory size (in Bytes)
	 * @return destination address
	 */
	static FORCE_INLINE void * memmov(void * dst, const void * src, sizet size)
	{
		return ::memmove(dst, src, size);
	}

	/**
	 * Default constructs elements in range
	 * 
	 * @param begin, end array range
	 */
	template<typename T>
	static FORCE_INLINE void constructDefaultElements(T * begin, T * end)
	{
		for (; begin != end; ++begin)
			new (begin) T{};
	}

	/**
	 * Copy elements from source to destination.
	 * If elements are not trivially constructible
	 * invoke copy constructor on each element,
	 * otherwise a simple memory copy is performed.
	 * 
	 * @param dst destination buffer
	 * @param src source buffer
	 * @param m num of elements
	 */
	template<typename T>
	static FORCE_INLINE typename EnableIf<!IsTriviallyConstructible<T>::value>::Type constructCopyElements(T * RESTRICT dst, const T * RESTRICT src, uint64 n)
	{
		for (uint32 i = 0; i < n; ++i)
			new (dst + i) T{src[i]};
	}

	template<typename T>
	static FORCE_INLINE typename EnableIf<IsTriviallyConstructible<T>::value>::Type constructCopyElements(T * RESTRICT dst, const T * RESTRICT src, uint64 n)
	{
		memcpy(dst, src, n * sizeof(T));
	}
	///@}

	/**
	 * Copy elements from source to destination.
	 * Source and destination buffer must not
	 * overlap.
	 * 
	 * @param dst destination buffer
	 * @param src source buffer
	 * @param n number of elements
	 * @{
	 */
	template<typename TT>
	static FORCE_INLINE typename EnableIf<!IsTriviallyCopyable<TT>::value>::Type copyElements(TT * RESTRICT dst, const TT * RESTRICT src, uint64 n)
	{
		for (uint64 i = 0; i < n; ++n, ++dst, ++src)
			// Non overlapping required
			*dst = *src;
	}

	template<typename TT>
	static FORCE_INLINE typename EnableIf<IsTriviallyCopyable<TT>::value>::Type copyElements(TT * RESTRICT dst, const TT * RESTRICT src, uint64 n)
	{
		memcpy(dst, src, n * sizeof(TT));
	}
	/// @}

	/**
	 * Move elements from source to destination.
	 * Use this when source and destination
	 * buffers could overlap.
	 * 
	 * @param dst destination buffer
	 * @param src source buffer
	 * @param n number of elements
	 * @{
	 */
	template<typename TT>
	static FORCE_INLINE typename EnableIf<!IsTriviallyCopyable<TT>::value>::Type moveElements(TT * dst, TT * src, uint64 n)
	{
		if (LIKELY(dst < src))
		{
			// Copy left to right
			for (uint64 i = 0; i < n; ++i, ++dst, ++src)
				// Move rather than copy
				*dst = move(*src);
		}
		else if (LIKELY(src > dst))
		{
			// Copy right to left
			for (uint64 i = 0, j = n - 1; i < n; ++i, --j)
				// Move rather than copy
				dst[j] = move(src[j]);
		}
		else
			; // Source and destination are the same
	}

	template<typename TT>
	static FORCE_INLINE typename EnableIf<IsTriviallyCopyable<TT>::value>::Type moveElements(TT * dst, TT * src, uint64 n)
	{
		memmove(dst, src, n * sizeof(TT));
	}
	/// @}

	/**
	 * Destroy elements in range
	 * 
	 * @param [in] begin,end array range
	 */
	template<typename T>
	static FORCE_INLINE void destroyElements(T * begin, T * end)
	{
		for (; begin != end; ++begin)
			begin->~T();
	}
};