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
	using PairT = Pair<KeyT, ValT, CompareT>;
	using NodeT = BinaryNode<PairT, typename PairT::FindPair>;

protected:
	/// Node allocator
	mutable MallocBase * malloc;

	/// Has own allocator flag
	bool bHasOwnMalloc;

	/// Map root node
	NodeT * root;

	/// Number of nodes
	uint64 numNodes;

	/**
	 * Allocate node
	 */
	template<typename PairU>
	NodeT * createNode(PairU && pair) const
	{
		return new (reinterpret_cast<NodeT*>(malloc->alloc(sizeof(NodeT), alignof(NodeT)))) NodeT{forward<PairU>(pair)};
	}

	/**
	 * Deallocate node
	 */
	void destroyNode(NodeT * node)
	{
		// Destroy and deallocate
		node->~NodeT();
		malloc->free(node);
	}

public:
	/**
	 * Default constructor
	 */
	FORCE_INLINE Map()
		: malloc{new MallocPooled(1024, sizeof(NodeT), alignof(NodeT))}
		, bHasOwnMalloc{true}
		, root{nullptr}
		, numNodes{0ull}
	{
		//
	}

	/**
	 * Returns number of nodes
	 * @{
	 */
	FORCE_INLINE uint64 getNumNodes() const
	{
		return numNodes;
	}

	METHOD_ALIAS_CONST(getCount, getNumNodes)
	METHOD_ALIAS_CONST(getSize, getNumNodes)
	/// @}

	/**
	 * Insert in map, replace value if
	 * key already exists
	 * 
	 * @param key pair key
	 * @param val pair value
	 * @return ref to value
	 */
	template<typename KeyU, typename ValU>
	ValT & insert(KeyU && key, ValU && val)
	{
		if (UNLIKELY(root == nullptr))
		{
			numNodes = 1;

			root = createNode(PairT{forward<KeyU>(key), forward<ValU>(val)});
			root->color = BinaryNodeColor::BLACK;
			return root->data.second;
		}
		else
		{
			// Traverse tree
			NodeT * next = root, * prev = nullptr;
			while (next)
			{
				prev = next;

				const int32 cmp = typename PairT::FindPair()(key, next->data);
				if (cmp < 0)
					next = next->left;
				else if (cmp > 0)
					next = next->right;
				else break;
			}

			if (next)
			{
				// Key already exists,
				// don't create new node but replace
				return (prev->data.second = forward<ValU>(val));
			}
			else
			{
				numNodes++;

				NodeT * node = createNode(PairT{forward<KeyU>(key), forward<ValU>(val)});
				prev->insert(node);

				return node->data.second;
			}
		}
	}

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
		if (UNLIKELY(root == nullptr))
		{
			// Pair surely does not exists.
			// Create a new pair
			numNodes = 1;

			root = createNode(PairT{forward<KeyU>(key), ValT{}});
			root->color = BinaryNodeColor::BLACK;
			return root->data.second;
		}
		else
		{
			// Traverse tree
			NodeT * next = root, * prev = nullptr;
			while (next)
			{
				prev = next;

				const int32 cmp = typename PairT::FindPair()(key, next->data);
				if (cmp < 0)
					next = next->left;
				else if (cmp > 0)
					next = next->right;
				else break;
			}

			if (next)
			{
				// Node exists, return ref
				return next->data.second;
			}
			else
			{
				// Create and insert node
				numNodes++;

				NodeT * node = createNode(PairT{forward<KeyU>(key), ValT{}});
				prev->insert(node);

				return node->data.second;
			}
		}
	}

	/**
	 * Returns pointer to map node
	 * 
	 * @param key pair key
	 * @return node ptr, otherwise nullptr
	 * @{
	 */
	const NodeT * find(const KeyT & key) const
	{
		if (UNLIKELY(root == nullptr))
			return nullptr;
		else
		{
			NodeT * next = root;
			while (next)
			{
				const int32 cmp = typename PairT::FindPair()(key, next->data);
				if (cmp < 0)
					next = next->left;
				else if (cmp > 0)
					next = next->right;
				else break;
			}
			
			return next;
		}
	}

	NodeT * find(const KeyT & key)
	{
		return const_cast<NodeT*>(static_cast<const Map&>(*this).find(key));
	}
	/// @}

	/**
	 * Returns pair value and true if
	 * andy pair matches key. Not the
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
		const NodeT * node = find(key);

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
		return find(key) != nullptr;
	}

	/**
	 * Remove element from map and
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
		NodeT * node = find(key);
		if (!node) return false;

		// Copy out value
		val = move(node->data.second);

		// Remove node
		NodeT * removed = node->remove();
		numNodes--;

		// Update root
		if (removed == node)
			root = root->getRoot();
		else
			root = node->getRoot();
		
		return true;
	}
};