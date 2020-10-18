#pragma once

#include "core_types.h"

/**
 * Struct that defines the traits of
 * an iterator of type T.
 * 
 * @param T type of the data on which
 * 	to iterate
 */
template<typename T>
struct IteratorTraits
{
	using RefT = T&;
	using PtrT = T*;
};

/**
 * An input iterator defines an iterator
 * that invalidates its elements after
 * a pass. Once read and inremented, the
 * read value may be invalidated.
 * 
 * @param T type of the iterated items
 */
template<typename T>
struct InputIterator : IteratorTraits<T>
{

};

/**
 * An iterator that can be read and
 * incremented at will.
 * 
 * @param T type of the iterated items
 */
template<typename T>
struct ForwardIterator : InputIterator<T>
{

};

/**
 * An iterator that can be read and can
 * be both incremented and decremented
 * at will.
 * 
 * @param T type of the iterated items
 */
template<typename T>
struct BidirectionalIterator : ForwardIterator<T>
{

};

/**
 * A random access iterator can be
 * incremented and decremented at will
 * by an arbitrary amount.
 * 
 * @param T type of the iterated items
 */
template<typename T>
struct RandomAccessIterator : BidirectionalIterator<T>
{

};

/**
 * A random access iterator that behaves
 * like a contigous block of memory.
 * @see https://en.cppreference.com/w/cpp/named_req/ContiguousIterator
 * 
 * @param T type of the iterated items
 */
template<typename T>
struct ContiguousIterator : RandomAccessIterator<T>
{

};
