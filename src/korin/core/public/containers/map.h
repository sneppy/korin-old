#pragma once

#include "core_types.h"
#include "misc/assert.h"
#include "misc/utility.h"
#include "hal/platform_memory.h"
#include "hal/malloc_pool.h"
#include "templates/utility.h"
#include "./containers_types.h"
#include "./tree.h"
#include "./pair.h"

template<typename KeyT, typename ValT, typename CompareT, typename MallocT>
class Map
{
	using PairT = Pair<KeyT, ValT, CompareT>;
	using TreeT = BinaryTree<PairT, typename PairT::FindPair, MallocT>;
	using NodeT = typename TreeT::NodeT;
	using Iterator = typename TreeT::Iterator;
	using ConstIterator = typename TreeT::ConstIterator;

public:
	/**
	 * Default constructor.
	 */
	FORCE_INLINE Map()
		: tree{}
	{
		//
	}

	/**
	 * Allocator initializer.
	 */
	FORCE_INLINE Map(MallocBase * inMalloc)
		: tree{inMalloc}
	{
		//
	}

	/**
	 * Returns number of pairs.
	 * @{
	 */
	FORCE_INLINE uint64 getCount() const
	{
		return tree.getNumNodes();
	}

	METHOD_ALIAS_CONST(getNumNodes, getCount)
	METHOD_ALIAS_CONST(getSize, getCount)
	/// @}

	/**
	 * Returns a new iterator that points
	 * to the element with the minimum key
	 * in the map (i.e. the first, leftmost
	 * element).
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
	/** @} */

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
	 * Returns an iterator that
	 * points to the first pair
	 * that matches the key.
	 * 
	 * @param key key to search
	 * 	for
	 * @return iterator that
	 * 	points to the found
	 * 	pair
	 */
	template<typename KeyAnyT>
	FORCE_INLINE ConstIterator begin(const KeyAnyT & key) const
	{
		return tree.begin(key);
	}

	template<typename KeyAnyT>
	FORCE_INLINE Iterator begin(const KeyAnyT & key)
	{
		return tree.begin(key);
	}
	/** @} */

	/**
	 * Returns an iterator that
	 * points to the pair next to
	 * the one that matches the
	 * search key.
	 * 
	 * @param key key to search for
	 * @return iterator that points
	 * 	to the pair next to the one
	 * 	found
	 * @{
	 */
	template<typename KeyAnyT>
	FORCE_INLINE ConstIterator end(const KeyAnyT & key) const
	{
		return tree.end(key);
	}

	template<typename KeyAnyT>
	FORCE_INLINE Iterator end(const KeyAnyT & key)
	{
		return tree.end(key);
	}
	/** @} */

	/**
	 * Returns reference to a pair
	 * identified by the provided
	 * key. If such pair does not
	 * already exist, a new pair
	 * with a default value is
	 * created.
	 * 
	 * @param key key to search for
	 * @return ref to pair value
	 */
	template<typename KeyAnyT>
	ValT & operator[](KeyAnyT && key)
	{
		// This method is specific for
		// maps, thus we need to craft
		// it using node methods

		if (!tree.root)
		{
			// Pair surely does not exists.
			// Create a new pair
			tree.numNodes = 1;

			tree.root = tree.createNode(PairT{forward<KeyAnyT>(key), ValT{}});
			tree.root->color = BinaryNodeColor::BLACK;
			
			return tree.root->data.second;
		}
		else
		{
			// Traverse tree
			typename TreeT::NodeT * next = tree.root, * prev;
			while (next)
			{
				prev = next;

				int32 cmp = typename PairT::FindPair()(key, next->data);
				if (cmp < 0)
				{
					// Go left
					next = next->left;
				}
				else if (cmp > 0)
				{
					// Go right
					next = next->right;
				}
				else
					// Node exists, return ref
					return next->data.second;
			}
			
			tree.numNodes++;

			// Create and insert node.
			// Value type must be default constructible.
			// Update root, it may have changed
			auto node = prev->insert(tree.createNode(PairT{forward<KeyAnyT>(key), ValT{}}));
			tree.root = tree.root->getRoot();

			return node->data.second;
		}
	}

	/**
	 * Insert in map, replace value if
	 * key already exists.
	 * 
	 * @param key pair key
	 * @param val pair value
	 * @return ref to value
	 */
	template<typename KeyAnyT, typename ValAnyT>
	FORCE_INLINE ValT & insert(KeyAnyT && key, ValAnyT && val)
	{
		return tree.replace(PairT{forward<KeyAnyT>(key), forward<ValAnyT>(val)}).second;
	}

	/**
	 * Returns a new iterator pointing
	 * to the found element or the end
	 * of the array otherwise.
	 * 
	 * @param key pair key
	 * @return iterator that points to
	 * 	the found value
	 * @{
	 */
	template<typename AnyKeyT>
	FORCE_INLINE ConstIterator find(const AnyKeyT & key) const
	{
		return tree.find(key);
	}

	template<typename AnyKeyT>
	FORCE_INLINE Iterator find(const AnyKeyT & key)
	{
		return tree.find(key);
	}
	/** @} */

	/**
	 * Returns pair value and true if
	 * any pair matches key. Requires
	 * ValT::operator= to be well
	 * defined.
	 * 
	 * @param key pair key
	 * @param outVal retrieved pair value
	 * @return true if pair exists, false
	 * 	otherwise
	 */
	template<typename AnyKeyT>
	FORCE_INLINE bool find(const AnyKeyT & key, ValT & outVal) const
	{
		const auto * node = tree.findNode(key);

		if (node)
		{
			outVal = node->data.second;
			return true;
		}
		else return false;
	}

	/**
	 * Returns true if map has pair
	 * that matches key
	 */
	template<typename AnyKeyT>
	FORCE_INLINE bool has(const AnyKeyT & key) const
	{
		return tree.findNode(key) != nullptr;
	}
	
	/**
	 * Remove element pointed by
	 * iterator
	 * 
	 * @param it iterator that points
	 * 	to the pair to be removed
	 * @return iterator that points
	 * 	to the next valid pair
	 * @{
	 */
	FORCE_INLINE Iterator remove(const Iterator & it)
	{
		return tree.remove(it);
	}
	/// @}

	/**
	 * Remove pair from map. Note that
	 * this operation may invalidate
	 * pointers to map nodes.
	 * 
	 * @param key key to search for
	 * @return true if pair was
	 * 	removed
	 */
	template<typename AnyKeyT>
	FORCE_INLINE bool remove(const AnyKeyT & key)
	{
		// Retrieve node
		auto node = tree.findNode(key);
		if (!node) return false;

		// Remove node
		tree.removeNode(node);

		return true;
	}

	/**
	 * Removes pair from map and
	 * returns its value. Note that
	 * this operation may invalidate
	 * pointers to map nodes.
	 * 
	 * @param key key to search for
	 * @param outVal copied out pair
	 * 	value
	 * @return true if pair was
	 * 	removed
	 */
	template<typename AnyKeyT>
	FORCE_INLINE bool pop(const AnyKeyT & key, ValT & outVal)
	{
		// Retrieve node
		auto node = tree.findNode(key);
		if (!node) return false;

		// Copy out value
		outVal = move(node->data.second);

		// Remove node
		tree.removeNode(node);
		return true;
	}

protected:
	/// Binary tree
	TreeT tree;
};