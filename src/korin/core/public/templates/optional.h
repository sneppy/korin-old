#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "./utility.h"

/**
 * Simple surrogate for std::optional.
 * 
 * @param T type of the optional object
 */
template<typename T>
struct Optional
{
	Optional() = default;

	/**
	 * Copy constructor.
	 * @{
	 */
	constexpr FORCE_INLINE Optional(const Optional & other)
	{
		if (other.engaged)
		{
			// Copy object
			emplace(*other);
		}
	}

	constexpr FORCE_INLINE Optional(Optional & other)
		: Optional{const_cast<const Optional&>(other)}
	{
		
	}
	/** @} */

	/**
	 * Move constructor
	 */
	constexpr FORCE_INLINE Optional(Optional && other)
	{
		if (other.engaged)
		{
			// Move object
			emplace(move(*other));
		}
	}

	/**
	 * Copy assignment.
	 */
	constexpr FORCE_INLINE Optional & operator=(const Optional & other)
	{
		if (other.engaged && this->engaged)
		{
			// Copy object
			storage.value = *other;
		}
		else if (other.engaged && !this->engaged)
		{
			// Copy construct object
			emplace(*other);
		}
		else if (!other.engaged && this->engaged)
		{
			// Reset storage
			reset();
		}

		return *this;
	}

	/**
	 * Move assignment
	 */
	constexpr FORCE_INLINE Optional & operator=(Optional && other)
	{
		if (other.engaged && this->engaged)
		{
			// Move object
			storage.value = move(*other);
			other.engaged = false;
		}
		else if (other.engaged && !this->engaged)
		{
			// Move costruct object
			emplace(move(*other));
			other.engaged = false;
		}
		else if (!other.engaged && this->engaged)
		{
			// Reset storage
			reset();
		}

		return *this;
	}

	/**
	 * Destructor.
	 */
	~Optional()
	{
		if (engaged)
		{
			reset();
		}
	}

	/**
	 * Initialize value.
	 * 
	 * @param args arguments passed to
	 * 	optional object constructor
	 */
	template<typename ...ArgsT>
	constexpr FORCE_INLINE explicit Optional(ArgsT && ...args)
	{
		// Construct object
		emplace(forward<ArgsT>(args)...);
	}

	/**
	 * Construct optional object in
	 * place.
	 * 
	 * @param args arguments passed to
	 * 	optional object constructor
	 */
	template<typename ...ArgsT>
	FORCE_INLINE void emplace(ArgsT && ...args)
	{
		engaged = true;
		new (&storage.value) T{forward<ArgsT>(args)...};
	}

	/**
	 * Assign value to optional object.
	 * 
	 * @param inValue new value to
	 * 	assign
	 * @{
	 */
	FORCE_INLINE Optional & operator=(const T & inValue)
	{
		if (engaged)
		{
			// Already engaged, assign value
			storage.value = inValue;
		}
		else
		{
			// Copy construct value
			emplace(inValue);
		}

		return *this;
	}

	FORCE_INLINE Optional & operator=(T && inValue)
	{
		if (engaged)
		{
			// Assign rvalue
			storage.value = move(inValue);
		}
		else
		{
			// Move construct value
			emplace(move(inValue));
		}

		return *this;
	}
	/** @} */

	/**
	 * Reset storage, destroying the
	 * existing object, if any.
	 */
	FORCE_INLINE void reset()
	{
		if (engaged)
		{
			engaged = false;
			storage.~OptionalStorage();
		}
	}

	/**
	 * Returns ref to the contained value.
	 * @{
	 */
	FORCE_INLINE const T & operator*() const&
	{
		CHECK(engaged)
		return storage.value;
	}

	FORCE_INLINE T & operator*() &
	{
		CHECK(engaged)
		return storage.value;
	}

	FORCE_INLINE const T&& operator*() const&&
	{
		CHECK(engaged)
		return move(storage.value);
	}

	FORCE_INLINE T&& operator*() &&
	{
		CHECK(engaged)
		return move(storage.value);
	}
	/** @} */

	/**
	 * Returns ptr to the contained value.
	 * @{
	 */
	FORCE_INLINE const T * operator->() const
	{
		CHECK(engaged)
		return &storage.value;
	}

	FORCE_INLINE T * operator->()
	{
		CHECK(engaged)
		return &storage.value;
	}
	/** @} */

	/**
	 * Returns true if has value, false
	 * otherwise.
	 */
	FORCE_INLINE bool hasValue() const
	{
		return engaged;
	}

	/**
	 * Case to true if has value, to false
	 * otherwise.
	 */
	FORCE_INLINE explicit operator bool() const
	{
		return engaged;
	}

protected:
	/**
	 * Optional storage union, prevents
	 * object construction.
	 * 
	 * @param U type of the optional
	 */
	template<typename U>
	union OptionalStorage
	{
		/**
		 * Prevents construction of data.
		 */
		FORCE_INLINE OptionalStorage()
			: empty{}
		{
			//
		}

		/**
		 * Destructor.
		 */
		~OptionalStorage()
		{
			value.~U();
		}

		/// Empty byte
		ubyte empty;

		/// Actual storage data
		U value;
	};

	/// Storage object
	OptionalStorage<T> storage;

	/// Engaged flag, if true value exists
	bool engaged = false;
};
