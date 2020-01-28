#pragma once

#include "../core_types.h"
#include "../misc/assert.h"
#include "../misc/utility.h"
#include "../templates/utility.h"
#include "../hal/platform_memory.h"
#include "../hal/malloc_pool.h"
#include "containers_types.h"
#include "tree.h"
#include "pair.h"

template<typename KeyT, typename ValT, typename CompareT>
class Map
{
public:
	/// Pair type
	using PairT = Pair<KeyT, ValT, CompareT>;

	/// Binary tree type
	using TreeT = BinaryTree<PairT, typename PairT::FindPair>;

	/// Binary node type, induced by tree type
	using NodeT = typename TreeT::NodeT;

	/// Iterator type
	using Iterator = typename TreeT::Iterator;

	/// Const iterator type
	using ConstIterator = typename TreeT::ConstIterator;

protected:
	/// Binary tree
	TreeT tree;

public:
	/**
	 * Default constructor
	 */
	FORCE_INLINE Map()
		: tree{}
	{
		//
	}

	/**
	 * Use custom allocator
	 */
	FORCE_INLINE Map(MallocBase * inMalloc)
		: tree{inMalloc}
	{
		//
	}

	/**
	 * Returns number of nodes
	 * @{
	 */
	FORCE_INLINE uint64 getNumNodes() const
	{
		return tree.getNumNodes();
	}

	METHOD_ALIAS_CONST(getCount, getNumNodes)
	METHOD_ALIAS_CONST(getSize, getNumNodes)
	/// @}

	/**
	 * Returns a new iterator that points
	 * to the element with the minimum key
	 * in the map (i.e. the first, leftmost
	 * element)
	 * @{
	 */
	FORCE_INLINE ConstIterator begin() const
	{
		return tree.begin();
	}

	FORCE_INLINE Iterator begin()
	{
		return tree.begin();
	}
	/// @}

	/**
	 * Returns a new iterator that points
	 * to the end of the map
	 * @{
	 */
	FORCE_INLINE ConstIterator end() const
	{
		return tree.end();
	}

	FORCE_INLINE Iterator end()
	{
		return tree.end();
	}
	/// @}

	/**
	 * Returns reference to a pair
	 * identified by the provided
	 * key. If such pair does not
	 * already exist, a new pair
	 * with a default value is
	 * created
	 * 
	 * @param key pair key
	 * @return ref to pair value
	 */
	template<typename KeyU>
	ValT & operator[](KeyU && key)
	{
		// This method is specific for
		// maps, thus we need to craft
		// it using node methods

		if (!tree.root)
		{
			// Pair surely does not exists.
			// Create a new pair
			tree.numNodes = 1;

			tree.root = tree.createNode(PairT{forward<KeyU>(key), ValT{}});
			tree.root->color = BinaryNodeColor::BLACK;
			
			return tree.root->data.second;
		}
		else
		{
			// Traverse tree
			int32 cmp;
			typename TreeT::NodeT * next = tree.root, * prev;
			while (next)
			{
				prev = next;

				cmp = typename PairT::FindPair()(key, next->data);
				if (cmp < 0)
					next = next->left;
				else if (cmp > 0)
					next = next->right;
				else
					// Node exists, return ref
					return next->data.second;
			}
			
			tree.numNodes++;

			// Create and insert node
			auto node = prev->insert(tree.createNode(PairT{forward<KeyU>(key), ValT{}}));
			
			// Root may be changed
			tree.root = tree.root->getRoot();

			return node->data.second;
		}
	}

	/**
	 * Insert in map, replace value if
	 * key already exists
	 * 
	 * @param key pair key
	 * @param val pair value
	 * @return ref to value
	 */
	template<typename KeyU, typename ValU>
	FORCE_INLINE ValT & insert(KeyU && key, ValU && val)
	{
		return tree.replace(PairT{forward<KeyU>(key), forward<ValU>(val)}).second;
	}

	/**
	 * Returns a new iterator pointing
	 * to the found element or the end
	 * of the array otherwise
	 * 
	 * @param key pair key
	 * @return new iterator
	 * @{
	 */
	FORCE_INLINE ConstIterator find(const KeyT & key) const
	{
		return tree.find(key);
	}

	FORCE_INLINE Iterator find(const KeyT & key)
	{
		return tree.find(key);
	}
	/// @}

	/**
	 * Returns pair value and true if
	 * any pair matches key. Note the
	 * value is copied, this method
	 * may not be suitable for complex
	 * types. See Map::find overloads
	 * instead
	 * 
	 * @param [in] key pair key
	 * @param [out] val retrieved pair value
	 * @return true if pair exists, false
	 * 	otherwise
	 */
	FORCE_INLINE bool find(const KeyT & key, ValT & val) const
	{
		const auto * node = tree.findNode(key);

		if (node)
		{
			val = node->data.second;
			return true;
		}
		else return false;
	}

	/**
	 * Returns true if map has pair
	 * that matches key
	 */
	FORCE_INLINE bool has(const KeyT & key) const
	{
		return tree.findNode(key) != nullptr;
	}
	
public:
	/**
	 * Remove element pointed by
	 * iterator
	 * 
	 * @param it iterator
	 * @return next iterator
	 * @{
	 */
	FORCE_INLINE void remove(const ConstIterator & it)
	{
		tree.remove(it);
	}
	/// @}

	/**
	 * Remove pair from map. Note that
	 * this operation may invalidate
	 * pointers to map nodes
	 * 
	 * @param key pair key
	 * @return true if pair exists
	 */
	FORCE_INLINE bool remove(const KeyT & key)
	{
		// Retrieve node
		auto node = tree.findNode(key);
		if (!node) return false;

		// Remove node
		tree.removeNode(node);
		return true;
	}

	/**
	 * Remove pair from map and
	 * return its value. Note that
	 * this operation may invalidate
	 * pointers to map nodes
	 * 
	 * @param [in] key pair key
	 * @param [out] val out pair value
	 * @return true if pair exists
	 */
	FORCE_INLINE bool pop(const KeyT & key, ValT & val)
	{
		// Retrieve node
		auto node = tree.findNode(key);
		if (!node) return false;

		// Copy out value
		val = move(node->data.second);

		// Remove node
		tree.removeNode(node);
		return true;
	}
};