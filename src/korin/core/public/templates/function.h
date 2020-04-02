#pragma once

#include "core_types.h"
#include "templates/types.h"
#include "templates/utility.h"

/**
 * @{
 */
template<typename FunctorT, typename ... Args>
FORCE_INLINE auto invoke(FunctorT && fn, Args && ... args) -> decltype(forward<FunctorT>(fn)(forward<Args>(args)...))
{
	return forward<FunctorT>(fn)(forward<Args>(args)...);
}
/// @}

/**
 * @see Function
 */
template<typename> class Function;

/**
 * Returns true if given type is
 * of type Function<FnT>
 */
template<typename T>
struct IsFunction
{
	enum {value = false};
};

template<typename T> struct IsFunction<Function<T>> { enum {value = true}; };
template<typename T> struct IsFunction<const T> { enum {value = IsFunction<T>::value}; };

/**
 * 
 */
struct FunctionOwnedObjectInterface
{
	/**
	 * 
	 */
	virtual ~FunctionOwnedObjectInterface() = default;

	/**
	 * 
	 */
	virtual void * getAddress() = 0;

	/**
	 * 
	 */
	virtual void destroy() = 0;
};

/**
 * 
 */
struct FunctionOwnedObjectInline : public FunctionOwnedObjectInterface
{
	//////////////////////////////////////////////////
	// FunctionOwnedObjectInterface interface
	//////////////////////////////////////////////////
	
	virtual FORCE_INLINE void destroy() override
	{
		this->~FunctionOwnedObjectInline();
	}
};

/**
 * 
 */
struct FunctionOwnedObjectHeap : public FunctionOwnedObjectInterface
{
	//////////////////////////////////////////////////
	// FunctionOwnedObjectInterface interface
	//////////////////////////////////////////////////
	
	virtual FORCE_INLINE void destroy() override
	{
		this->~FunctionOwnedObjectHeap();
		MallocAnsi{}.free(this);
	}
};

/**
 * 
 */
template<typename T>
struct FunctionOwnedObject : public FunctionOwnedObjectHeap
{
	T object;

	/**
	 * 
	 */
	template<typename ... Args>
	FORCE_INLINE FunctionOwnedObject(Args && ... args)
		: object{forward<Args>(args)...}
	{
		//
	}

	//////////////////////////////////////////////////
	// FunctionOwnedObjectInterface interface
	//////////////////////////////////////////////////
	
	virtual FORCE_INLINE void * getAddress() override
	{
		return &object;
	}
};

/**
 * 
 */
struct FunctionStorageBase
{
	/// The heap allocation storage
	void * heap;

	/**
	 * Default constructor
	 */
	FORCE_INLINE FunctionStorageBase()
		: heap{nullptr}
	{
		//
	}

	/**
	 * Move constructor
	 */
	FORCE_INLINE FunctionStorageBase(FunctionStorageBase && other)
		: heap{other.heap}
	{
		other.heap = nullptr;
	}

	/**
	 * Returns ptr to owned object
	 */
	FORCE_INLINE FunctionOwnedObjectInterface * getOwnedObject() const
	{
		return static_cast<FunctionOwnedObjectInterface*>(heap);
	}

	/**
	 * Returns ptr to callable object
	 */
	FORCE_INLINE void * getAddress() const
	{
		return getOwnedObject()->getAddress();
	}

	/**
	 * Destroy owned object
	 */
	FORCE_INLINE void unbind()
	{
		getOwnedObject()->destroy();
	}
};

struct FunctionStorage : FunctionStorageBase
{
	/**
	 * 
	 */
	template<typename FunctorT>
	typename DecayType<FunctorT>::Type * bind(FunctorT && fn)
	{
		using OwnedT = FunctionOwnedObject<FunctorT>;

		// TODO: Use global allocator
		heap = MallocAnsi{}.alloc(sizeof(OwnedT), alignof(OwnedT));
		OwnedT * object = new (heap) OwnedT{forward<FunctorT>(fn)};

		return &object->object;
	}
};

/**
 * @{
 */
template<typename FunctorT, typename FnT>
struct FunctionCaller;

template<typename FunctorT, typename RetT, typename ... Args>
struct FunctionCaller<FunctorT, RetT(Args...)>
{
	/**
	 * Execute function call
	 */
	static FORCE_INLINE RetT call(void * fn, Args && ... args)
	{
		return invoke(*(FunctorT*)fn, forward<Args>(args)...);
	}
};

template<typename FunctorT, typename ... Args>
struct FunctionCaller<FunctorT, void(Args...)>
{
	/**
	 * Execute function call
	 */
	static FORCE_INLINE void call(void * fn, Args && ... args)
	{
		invoke(*(FunctorT*)fn, forward<Args>(args)...);
	}
};
/// @}

/**
 * @{
 */
template<typename StorageT, typename FnT>
struct FunctionBase;

template<typename StorageT, typename RetT, typename ... Args>
struct FunctionBase<StorageT, RetT(Args...)>
{
	template<typename StorageU, typename FnU> friend struct FunctionBase;

	/**
	 * 
	 */
	FORCE_INLINE FunctionBase()
		: callable{nullptr}
	{

	}

	/**
	 * Copy constructor
	 */
	FORCE_INLINE FunctionBase(const FunctionBase & other)
		: callable{other.callable}
	{
		// Copy storage
		//if (callable) storage.bindCopy(other.storage);
	}

	/**
	 * Move constructor
	 */
	FORCE_INLINE FunctionBase(FunctionBase && other)
		: callable{other.callable}
		, storage{move(other.storage)}
	{
		other.callable = nullptr;
	}

	/**
	 * 
	 */
	template<typename FunctorT, typename = typename EnableIf<!IsSameType<FunctionBase, typename DecayType<FunctorT>::Type>::value>::Type>
	FORCE_INLINE FunctionBase(FunctorT && inFn)
	{
		auto * binding = storage.bind(forward<FunctorT>(inFn));
		if (binding)
		{
			// Get appropriate caller
			using DecayedFunctorT = typename RemovePointer<decltype(binding)>::Type;
			callable = &FunctionCaller<DecayedFunctorT, RetT(Args...)>::call;
		}
	}

	/// Deleted assignments @{
	FunctionBase & operator=(const FunctionBase&) = delete;
	FunctionBase & operator=(FunctionBase&&) = delete;
	/// @}

	/**
	 * 
	 */
	FORCE_INLINE ~FunctionBase()
	{
		if (callable) storage.unbind();
	}

	/**
	 * 
	 */
	FORCE_INLINE RetT operator()(Args ... args)
	{
		// Perform the call
		return callable(storage.getAddress(), args...);
	}

protected:
	/// Ptr function that invokes the call operator
	RetT (*callable)(void*, Args&...);

	/// Function storage object
	StorageT storage;
};

/**
 * 
 */
template<typename FnT>
class Function final : public FunctionBase<FunctionStorage, FnT>
{
	using Super = FunctionBase<FunctionStorage, FnT>;

public:
	/// Default constructors @{
	Function() = default;
	Function(const Function&) = default;
	Function(Function&&) = default;
	/// @}

	/**
	 * 
	 */
	template<typename FunctorT, typename = typename EnableIf<!IsFunction<typename DecayType<FunctorT>::Type>::value>::Type>
	FORCE_INLINE Function(FunctorT && inFn)
		: Super(forward<FunctorT>(inFn))
	{
		//
	}

	/**
	 * Copy assignment
	 */
	FORCE_INLINE Function & operator=(const Function & other)
	{
		swap(*this, Function{other});
		return *this;
	}

	/**
	 * Move assignment
	 */
	FORCE_INLINE Function & operator=(Function && other)
	{
		swap(*this, other);
		return *this;
	}

	/**
	 * Returns true if function is callable
	 */
	FORCE_INLINE operator bool() const
	{
		return !!this->callable;
	}
};