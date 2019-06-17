#pragma once

#include "core_types.h"
#include "templates/utility.h"

/**
 * 
 */
template<typename T, typename CompareT>
struct BinaryNode
{
	/// Parent node
	BinaryNode * parent;

	/// Left child
	BinaryNode * left;

	/// Right child
	BinaryNode * right;

	/// Next node
	BinaryNode * next;

	/// Previous node
	BinaryNode * prev;

	/// Node data
	T data;

	/// Node color
	enum Color : ubyte
	{
		BLACK = 0,
		RED = 1
	} color;

public:
	/**
	 * Default constructor, initializes data
	 * 
	 * @param [in] inData node data
	 */
	template<typename _T>
	FORCE_INLINE BinaryNode(_T && inData)
		: parent{nullptr}
		, left{nullptr}
		, right{nullptr}
		, next{nullptr}
		, prev{nullptr}
		, data{forward<_T>(inData)}
		, color{Color::RED}
	{
		//
	}

	/**
	 * Returns true if node is black or is null
	 */
	static FORCE_INLINE bool isBlack(BinaryNode * node)
	{
		return node == nullptr || node->color == Color::BLACK;
	}

	/**
	 * Returns true if node is not null and is red
	 */
	static FORCE_INLINE bool isRed(BinaryNode * node)
	{
		return node && node->color == Color::RED;
	}

	/**
	 * Returns tree root
	 * @{
	 */
	FORCE_INLINE const BinaryNode * getRoot() const
	{
		return parent ? parent->getRoot() : this;
	}

	FORCE_INLINE BinaryNode * getRoot()
	{
		return parent ? parent->getRoot() : this;
	}
	/// @}

	/**
	 * Returns subtree leftmost node
	 * @{
	 */
	FORCE_INLINE const BinaryNode * getMin() const
	{
		return left ? left->getMin() : this;
	}

	FORCE_INLINE BinaryNode * getMin()
	{
		return left ? left->getMin() : this;
	}
	/// @}

	/**
	 * Returns subtree rightmost node
	 * @{
	 */
	FORCE_INLINE const BinaryNode * getMax() const
	{
		return right ? right->getMax() : this;
	}

	FORCE_INLINE BinaryNode * getMax()
	{
		return right ? right->getMax() : this;
	}
	/// @}

	/**
	 * Find node with search key
	 * 
	 * @param [in] key search key
	 */
	const BinaryNode * find(const T & key) const
	{
		int32 cmp = CompareT()(key, this->data);

		if (cmp < 0)
			return left ? left->find(key) : nullptr;
		else if (cmp > 0)
			return right ? right->find(key) : nullptr;
		else
			return this;
	}
	
	FORCE_INLINE BinaryNode * find(const T & key)
	{
		return const_cast<BinaryNode*>(static_cast<const BinaryNode&>(*this).find(key));
	}
	/// @}

protected:
	/**
	 * Replace left[right] child
	 * 
	 * ! Doesn't update prev[next] pointer
	 * 
	 * @param [in] node new left[right] child
	 * @return left[right] child
	 * @{
	 */
	FORCE_INLINE BinaryNode * setLeftChild(BinaryNode * node)
	{
		if (left = node)
			left->parent = this;

		return left;
	}

	FORCE_INLINE BinaryNode * setRightChild(BinaryNode * node)
	{
		if (right = node)
			right->parent = this;

		return right;
	}
	/// @}

	/**
	 * Set previous[next] node
	 * 
	 * ! Node cannot be null
	 * 
	 * @param [in] node new prev[next] node
	 * @return prev[next] node
	 * @{
	 */
	FORCE_INLINE BinaryNode * setPrevNode(BinaryNode * node)
	{
		if (prev != nullptr)
			prev->next = node;
		
		node->prev = prev;
		node->next = this;
		this->prev = node;

		return node;
	}

	FORCE_INLINE BinaryNode * setNextNode(BinaryNode * node)
	{
		if (next != nullptr)
			next->prev = node;
		
		node->next = next;
		this->next = node;
		next->prev = this;

		return node;
	}
	/// @}

public:
	/**
	 * Insert node in the subtree
	 * Allows duplicates
	 * 
	 * @param [in] node node to insert
	 * @return pointer to inserted node
	 */
	BinaryNode * insert(BinaryNode * node)
	{
		int32 cmp = CompareT()(node->data, this->data);

		if (cmp < 0)
		{
			if (left)
				return left->insert(node);
			else
			{
				setPrevNode(node);
				setLeftChild(node);

				return node;
			}
		}
		else
		{
			if (right)
				return right->insert(node);
			else
			{
				setNextNode(node);
				setRightChild(node);

				return node;
			}
		}

	}
};