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

	/// Binary node type
	using NodeT = BinaryNode<PairT, typename PairT::FindPair>;

	/// Iterator type
	using Iterator = NodeIterator<NodeT>;

	/// Const iterator type
	using ConstIterator = NodeConstIterator<NodeT>;

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
		: malloc{nullptr}
		, bHasOwnMalloc{true}
		, root{nullptr}
		, numNodes{0ull}
	{
		malloc = new MallocAnsi;
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
	 * Returns a new iterator that points
	 * to the element with the minimum key
	 * in the map (i.e. the first, leftmost
	 * element)
	 * @{
	 */
	Iterator begin()
	{
		return Iterator{root->getMin()};
	}
	/// @}

	/**
	 * Returns a new iterator that points
	 * to the end of the map
	 * @{
	 */
	Iterator end()
	{
		return Iterator{nullptr};
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
		if (UNLIKELY(root == nullptr))
		{
			// Pair surely does not exists.
			// Create a new pair
			numNodes = 1;

			root = createNode(PairT{forward<KeyU>(key), forward<ValU>(val)});
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
					return (next->data.second = forward<ValU>(val));
			}
			
			numNodes++;

			// Create and insert node
			NodeT * node = createNode(PairT{forward<KeyU>(key), forward<ValU>(val)});
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

protected:
	/**
	 * Returns pointer to map node
	 * 
	 * @param key pair key
	 * @return node ptr, otherwise nullptr
	 * @{
	 */
	const NodeT * findNode(const KeyT & key) const
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

	NodeT * findNode(const KeyT & key)
	{
		return const_cast<NodeT*>(static_cast<const Map&>(*this).findNode(key));
	}
	/// @}

public:
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
		return ConstIterator{findNode(key)};
	}

	FORCE_INLINE Iterator find(const KeyT & key)
	{
		return Iterator{findNode(key)};
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
		const NodeT * node = findNode(key);

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
		return findNode(key) != nullptr;
	}

protected:
	/**
	 * Remove node from map. Note that
	 * this operation may invalidate
	 * pointers to map nodes
	 * 
	 * @param node node to remove
	 */
	void removeNode(NodeT * node)
	{
		CHECKF(find(node->data.first) == node, "node %p is not part of this map", node)

		// Root may have changed
		if (node->remove() == root) root = root->left ? root->left : root->right;
		if (root) root = root->getRoot();

		numNodes--;
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
		if (LIKELY(it.node != nullptr))
			removeNode(it.node);
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
		NodeT * node = findNode(key);
		if (!node) return false;

		// Remove node
		removeNode(node);
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
		NodeT * node = findNode(key);
		if (!node) return false;

		// Copy out value
		val = move(node->data.second);

		// Remove node
		removeNode(node);
		return true;
	}
};