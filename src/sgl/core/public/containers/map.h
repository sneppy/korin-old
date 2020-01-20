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
	FORCE_INLINE NodeT * createNode(PairU && pair) const
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
	 * Use custom allocator
	 */
	FORCE_INLINE Map(MallocBase * inMalloc)
		: malloc{inMalloc}
		, bHasOwnMalloc{false}
		, root{nullptr}
		, numNodes{0ull}
	{
		//
	}

protected:
	/**
	 * Destroy tree
	 */
	void destroyTree(NodeT * node)
	{
		if (node->left) destroyTree(node->left);
		if (node->right) destroyTree(node->right);
		destroyNode(node);
	}

public:
	/**
	 * Destructor
	 */
	FORCE_INLINE ~Map()
	{
		numNodes = 0;

		if (root)
		{
			destroyTree(root);
			root = nullptr;
		}
		
		if (bHasOwnMalloc)
		{
			delete malloc;
			malloc = nullptr;
		}
	}

	/**
	 * Returns number of nodes
	 * @{
	 */
	FORCE_INLINE uint64 getNumNodes() const
	{
		CHECK(numNodes == root->getNumNodes())
		return numNodes;
	}

	METHOD_ALIAS_CONST(getCount, getNumNodes)
	METHOD_ALIAS_CONST(getSize, getNumNodes)
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
			int32 cmp;
			NodeT * next = root, * prev = nullptr;
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
			
			numNodes++;

			// Create and insert node
			NodeT * node = createNode(PairT{forward<KeyU>(key), ValT{}});
			if (cmp < 0)
			{
				prev->setPrevNode(node);
				prev->setLeftChild(node);
				NodeT::repairInserted(node);
			}
			else
			{
				prev->setNextNode(node);
				prev->setRightChild(node);
				NodeT::repairInserted(node);
			}
			
			// Root may be changed
			root = root->getRoot();

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
	ValT & insert(KeyU && key, ValU && val)
	{
		return (operator[](forward<KeyU>(key)) = forward<ValU>(val));
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
	 * Remove node from map. Note that
	 * this operation may invalidate
	 * pointers to map nodes
	 * 
	 * @param node node to remove
	 */
	void remove(NodeT * node)
	{
		CHECKF(find(node->data.first) == node, "node %p is not part of this map", node)

		// Root may have changed
		if (node->remove() == root) root = root->left ? root->left : root->right;
		if (root) root = root->getRoot();

		numNodes--;
	}

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
		NodeT * node = find(key);
		if (!node) return false;

		// Remove node
		remove(node);
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
		NodeT * node = find(key);
		if (!node) return false;

		// Copy out value
		val = move(node->data.second);

		// Remove node
		remove(node);
		return true;
	}
};