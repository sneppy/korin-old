#pragma once

#include "../core_types.h"
#include "../templates/types.h"
#include "../templates/utility.h"

struct Sort
{
	/**
	 * Sorting algorithms
	 */
	enum SortClass
	{
		SELECTION,
		BUBBLE,
		MERGE,
		INSERTION,
		QUICK,
		MIN
	};

	/**
	 * Partition algorithm.
	 * Given a forward iterator and a
	 * pivot, moves all elements smaller
	 * than the pivot to the left, all
	 * bigger or equal elements to the
	 * right
	 * 
	 * @param begin,end begin and end iterators
	 * @param pivot pivot element (iterator or value as well)
	 * @param cmp compare function
	 */
	template<typename It, typename T, typename CompareT>
	static It partition(It begin, It end, T pivot, CompareT cmp/* = TreeWayCompare() */)
	{
		It i = begin, j = begin;
		for (; i != end; ++i)
			if (cmp(*i, pivot) < 0) swap(*i, *j), ++j;
	}

	/**
	 * Quicksort algorithm.
	 * Given a forward iterator sorts
	 * the container in place in O(log N)
	 * time complexity and constant space
	 * complexity.
	 * O(N) worst case
	 * 
	 * @param begin,end begin and end iterators
	 * @param cmp compare function
	 */
	template<typename It, typename CompareT>
	static void quicksort(It begin, It end, CompareT && cmp/* = ThreeWayCompare() */)
	{
		It i = begin, j = begin, p = begin;

		if (i == end || ++i == end)
			// Single element
			return;
		else if (++i == end)
		{
			// Two elements
			if (cmp(*begin, *(++j)) > 0) swap(*begin, *j);
			return;
		}

		// We need a third iterator because
		// of the assumption that we cannot
		// decrement an iterator (i.e. --j)
		i = j = begin;
		for (++i, ++j; i != end; ++i)
			if (cmp(*i, *begin) < 0) swap(*i, *(p = j)), ++j;
		
		// Swap pivot
		swap(*begin, *p);

		// Recursive calls
		quicksort(begin, p, forward<CompareT>(cmp));
		quicksort(j, end, forward<CompareT>(cmp));
	}

	/**
	 * Quicksort algorithm.
	 * Given a forward iterator and an
	 * argument iterator sorts both
	 * the container and the arguments
	 * in place in O(log N) time
	 * complexity and constant space
	 * complexity.
	 * O(N) worst case
	 * 
	 * @param begin,end begin and end iterators
	 * @param args arguments begin iterator
	 * @param cmp compare function
	 */
	template<typename ItT, typename ItU, typename CompareT>
	static void argquicksort(ItT begin, ItT end, ItU args, CompareT && cmp/*  = ThreeWayCompare() */)
	{
		ItT i = begin, j = begin, k = begin;
		ItU x = args, y = args, z = args;

		if (i == end || ++i == end)
			// Single element
			return;
		else if (++i == end)
		{
			// Two elements
			if (cmp(*begin, *(++j)) > 0) swap(*begin, *j), swap(*args, *(++y));
			return;
		}

		// We need a third iterator because
		// of the assumption that we cannot
		// decrement an iterator (i.e. --j)
		i = j = begin;
		for (++i, ++j, ++x, ++y; i != end; ++i, ++x)
			if (cmp(*i, *begin) < 0) swap(*i, *(k = j)), swap(*x, *(z = y)), ++j, ++y;
		
		// Swap pivot
		swap(*begin, *k);
		swap(*args, *z);

		// Recursive calls
		argquicksort(begin, k, args, forward<CompareT>(cmp));
		argquicksort(j, end, y, forward<CompareT>(cmp));
	}
};