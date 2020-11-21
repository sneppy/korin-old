#pragma once

#include "core_types.h"
#include "templates/types.h"

/**
 * Surrogate for @c std::unique_ptr.
 * 
 * Unlike @c std::unique_ptr, a @c
 * UniquePtr may never own the object
 * itself (i.e. it is never responsible
 * for allocating and deallocating
 * memory).
 * 
 * TODO: Add unit tests
 * 
 * @param T type of the pointed
 * 	object
 */
template<typename T>
struct UniquePtr : NonCopyable
{
	/**
	 * Default constructor, initializes to
	 * nullptr.
	 */
	constexpr FORCE_INLINE UniquePtr()
		: obj{nullptr}
	{
		//
	}

	/**
	 * Initialize pointer.
	 */
	constexpr FORCE_INLINE UniquePtr(T * inObj)
		: obj{inObj}
	{
		//
	}

	/**
	 * Move constructor.
	 */
	FORCE_INLINE UniquePtr(UniquePtr && other)
		: obj{other.obj}
	{
		other.obj = nullptr;
	}

	/**
	 * Move assignment.
	 */
	FORCE_INLINE UniquePtr & operator=(UniquePtr && other)
	{
		obj = other.obj;
		other.obj = nullptr;
	}

	/**
	 * Returns the pointer itself.
	 * @{
	 */
	FORCE_INLINE const T * operator->() const
	{
		return obj;
	}

	FORCE_INLINE T * operator->()
	{
		return obj;
	}
	/** @} */

	/**
	 * Returns a reference to the object.
	 * @{
	 */
	FORCE_INLINE const T & operator*() const
	{
		return obj;
	}

	FORCE_INLINE T & operator*()
	{
		return obj;
	}
	/** @} */

	/**
	 * Cast to bool, returns true if
	 * pointer is not nullptr.
	 */
	FORCE_INLINE operator bool() const
	{
		return !!obj;
	}

	/**
	 * Returns true if pointer is valid,
	 * i.e. is not nullptr.
	 */
	FORCE_INLINE bool isValid() const
	{
		return static_cast<bool>(*this);
	}

	/**
	 * Explicit cast to naked pointer
	 * type.
	 */
	FORCE_INLINE explicit operator T*()
	{
		return obj;
	}

private:
	/// Pointer to the object
	T * obj;
};