#pragma once

#include "../core_types.h"
#include "../misc/assert.h"
#include "../templates/utility.h"
#include "containers_types.h"
#include "string.h"

/// Red-black tree color
enum class BinaryNodeColor : ubyte
{
	BLACK,
	RED
};

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
	BinaryNodeColor color;

	/**
	 * Default constructor, initializes data
	 * 
	 * @param [in] inData node data
	 */
	template<typename TT>
	FORCE_INLINE BinaryNode(TT && inData)
		: parent{nullptr}
		, left{nullptr}
		, right{nullptr}
		, next{nullptr}
		, prev{nullptr}
		, data{forward<TT>(inData)}
		, color{BinaryNodeColor::RED}
	{
		//
	}

	/**
	 * Returns true if node is black or is null
	 */
	static FORCE_INLINE bool isBlack(const BinaryNode * node)
	{
		return node == nullptr || node->color == BinaryNodeColor::BLACK;
	}

	/**
	 * Returns true if node is not null and is red
	 */
	static FORCE_INLINE bool isRed(const BinaryNode * node)
	{
		return node && node->color == BinaryNodeColor::RED;
	}

	/**
	 * Returns tree root
	 * @{
	 */
	FORCE_INLINE BinaryNode * getRoot()
	{
		return parent ? parent->getRoot() : this;
	}

	FORCE_INLINE const BinaryNode * getRoot() const
	{
		return parent ? parent->getRoot() : this;
	}
	/// @}

	/**
	 * Returns subtree leftmost node
	 * @{
	 */
	FORCE_INLINE BinaryNode * getMin()
	{
		return left ? left->getMin() : this;
	}

	FORCE_INLINE const BinaryNode * getMin() const
	{
		return left ? left->getMin() : this;
	}
	/// @}

	/**
	 * Returns subtree rightmost node
	 * @{
	 */
	FORCE_INLINE BinaryNode * getMax()
	{
		return right ? right->getMax() : this;
	}

	FORCE_INLINE const BinaryNode * getMax() const
	{
		return right ? right->getMax() : this;
	}
	/// @}

	/**
	 * Find node with search key
	 * 
	 * @param key search key
	 * @return ptr to node, nullptr
	 * 	otherwise
	 * @{
	 */
	const BinaryNode * find(const T & key) const
	{
		const int32 cmp = CompareT()(key, this->data);

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

	/**
	 * Find leftmost node that matches
	 * search key
	 * 
	 * @param key search key
	 * @return ptr to node, nullptr
	 * 	otherwise
	 * @{
	 */
	const BinaryNode * findMin(const T & key) const
	{
		// Find matching node
		BinaryNode * next = find(key);

		// Now find leftmost
		BinaryNode * prev = next;
		do
		{
			prev = next;
			next = next->left;
		} while (next && CompareT()(key, next->data));	

		return prev;	
	}

	FORCE_INLINE BinaryNode * findMin(const T & key)
	{
		return const_cast<BinaryNode*>(static_cast<const BinaryNode&>(*this).find(key));
	}
	/// @}

	/**
	 * Find rightmost node that matches
	 * search key
	 * 
	 * @param key search key
	 * @return ptr to node, nullptr
	 * 	otherwise
	 * @{
	 */
	const BinaryNode * findMax(const T & key) const
	{
		// Find matching node
		BinaryNode * next = find(key);

		// Now find rightmost
		BinaryNode * prev = next;
		do
		{
			prev = next;
			next = next->right;
		} while (next && CompareT()(key, next->data));	

		return prev;	
	}

	FORCE_INLINE BinaryNode * findMax(const T & key)
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
		CHECK(node != nullptr)

		if (prev != nullptr)
			prev->next = node;
		
		node->prev = prev;
		node->next = this;
		this->prev = node;

		return node;
	}

	FORCE_INLINE BinaryNode * setNextNode(BinaryNode * node)
	{
		CHECK(node != nullptr)

		if (next != nullptr)
			next->prev = node;
		
		node->next = next;
		this->next = node;
		next->prev = this;

		return node;
	}
	/// @}
	
	/**
	 * Perform left[right] rotation on this node
	 * @{
	 */
	FORCE_INLINE void rotateLeft()
	{
		BinaryNode * root = parent;
		BinaryNode * pivot = right;

		// Replace right child whit right->left child
		setRightChild(right->left);

		// Rotate
		pivot->setLeftChild(this);

		if (root)
		{
			root->left == this
				? root->setLeftChild(pivot)
				: root->setRightChild(pivot);
		}
		else
			pivot->parent = nullptr;
	}

	FORCE_INLINE void rotateRight()
	{
		BinaryNode * root = parent;
		BinaryNode * pivot = left;

		// Replace left child whit left->right child
		setLeftChild(left->right);

		// Rotate
		pivot->setRightChild(this);

		if (root)
		{
			root->right == this
				? root->setRightChild(pivot)
				: root->setLeftChild(pivot);
		}
		else
			pivot->parent = nullptr;
	}
	/// @}

	/**
	 * Repair inserted node
	 * 
	 * @param [in] node inserted node
	 */
	static void repairInserted(BinaryNode * node)
	{
		// Case 0: I'm (g)root
		if (node->parent == nullptr)
			node->color = BinaryNodeColor::BLACK;
		
		// Case 1: parent is black
		else if (isBlack(node->parent))
			node->color = BinaryNodeColor::RED;
		
		else
		{
			// Get relatives
			BinaryNode
				* parent = node->parent,
				* grand = parent->parent,
				* uncle = grand
					? (grand->left == parent ? grand->right : grand->left)
					: nullptr;
			
			// Case 2: uncle is red
			if (isRed(uncle))
			{
				uncle->color = parent->color = BinaryNodeColor::BLACK;
				grand->color = BinaryNodeColor::RED;

				// Repair grand
				repairInserted(grand);
			}

			// Case 3: uncle is black
			else
			{
				if (grand->left == parent)
				{
					if (parent->right == node)
					{
						parent->rotateLeft();
						grand->rotateRight();

						node->color = BinaryNodeColor::BLACK;
						grand->color = BinaryNodeColor::RED;
					}
					else
					{
						grand->rotateRight();

						parent->color = BinaryNodeColor::BLACK;
						grand->color = BinaryNodeColor::RED;
					}
				}
				else
				{
					if (parent->left == node)
					{
						parent->rotateRight();
						grand->rotateLeft();

						node->color = BinaryNodeColor::BLACK;
						grand->color = BinaryNodeColor::RED;
					}
					else
					{
						grand->rotateLeft();

						parent->color = BinaryNodeColor::BLACK;
						grand->color = BinaryNodeColor::RED;
					}
				}
			}
		}
	}

protected:
	/**
	 * Repair tree structure after node deletion
	 * 
	 * Since NIL leaf are considered black nodes
	 * we must run the repair algorithm even if
	 * the node is NIL. For this reason we cannot
	 * use a non-static function
	 * 
	 * @param [in] node node to repair
	 * @param [in] parent node parent, necessary if node is NIL
	 */
	static void repairRemoved(BinaryNode * node, BinaryNode * parent)
	{
		/// Good ol' Wikipedia
		/// @ref https://en.wikipedia.org/wiki/Red%E2%80%93black_tree#Removal

		// Case -1: node is null and parent is null
		if (node == nullptr && parent == nullptr)
			; // Do nothing

		// Case 0: node is red or is root
		else if (node && (isRed(node) || parent == nullptr))
			node->color = BinaryNodeColor::BLACK;
		
		// Left child
		else if (parent->left == node)
		{
			BinaryNode * sibling = parent->right;

			// Case 1: sibling is red
			if (isRed(sibling))
			{
				sibling->color	= BinaryNodeColor::BLACK;
				parent->color	= BinaryNodeColor::RED;

				// Rotate around parent and update sibling
				parent->rotateLeft();
				sibling = parent->right;
			}

			// Case 2: sibling is black with black children
			if (
				isBlack(sibling) &&
				(sibling->left == nullptr || isBlack(sibling->left)) &&
				(sibling->right == nullptr || isBlack(sibling->right))
			)
			{
				sibling->color = BinaryNodeColor::RED;
				// Recursive call
				repairRemoved(parent, parent->parent);
			}
			else
			{
				// Case 3: sibling is black and inner child is red
				if (sibling->left != nullptr && isRed(sibling->left))
				{
					sibling->color			= BinaryNodeColor::RED;
					sibling->left->color	= BinaryNodeColor::BLACK;

					// Rotate around sibling, so that red child is outer
					// We also need to update the sibling
					sibling->rotateRight();
					sibling = sibling->parent;
				}

				// Case 4: sibling is black and outer child is red
				{
					sibling->color			= parent->color;
					parent->color			= BinaryNodeColor::BLACK;
					sibling->right->color	= BinaryNodeColor::BLACK;

					// Rotate around parent
					parent->rotateLeft();
				}
			}
		}
		// Right child
		else
		{
			BinaryNode * sibling = parent->left;

			if (isRed(sibling))
			{
				sibling->color	= BinaryNodeColor::BLACK;
				parent->color	= BinaryNodeColor::RED;

				// Rotate around parent and update sibling
				parent->rotateRight();
				sibling = parent->left;
			}
			
			if (
				isBlack(sibling) &&
				(sibling->left == nullptr || isBlack(sibling->left)) &&
				(sibling->right == nullptr || isBlack(sibling->right))
			)
			{
				sibling->color = BinaryNodeColor::RED;
				// Recursive call
				repairRemoved(parent, parent->parent);
			}
			else
			{
				// Case 3: sibling is black and inner child is red
				if (sibling->right != nullptr && isRed(sibling->right))
				{
					sibling->color			= BinaryNodeColor::RED;
					sibling->right->color	= BinaryNodeColor::BLACK;

					// Rotate around sibling, so that red child is outer
					// We also need to update the sibling
					sibling->rotateLeft();
					sibling = sibling->parent;
				}

				// Case 4: sibling is black and outer child is red
				{
					sibling->color			= parent->color;
					parent->color			= BinaryNodeColor::BLACK;
					sibling->left->color	= BinaryNodeColor::BLACK;

					// Rotate around parent
					parent->rotateRight();
				}
			}
		}
	}

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
				repairInserted(node);

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
				repairInserted(node);

				return node;
			}
		}
	}

	/**
	 * Insert node in subtree only if no
	 * duplicate exists
	 * 
	 * @param [in] node node to insert
	 * @return inserted node or existing duplicate node
	 */
	BinaryNode * insertUnique(BinaryNode * node)
	{
		int32 cmp = CompareT()(node->data, this->data);

		if (cmp < 0)
		{
			if (left)
				return left->insertUnique(node);
			else
			{
				setPrevNode(node);
				setLeftChild(node);
				repairInserted(node);

				return node;
			}
		}
		else if (cmp > 0)
		{
			if (right)
				return right->insertUnique(node);
			else
			{
				setNextNode(node);
				setRightChild(node);
				repairInserted(node);

				return node;
			}
		}
		else
			// Found duplicate node
			return this;
	}

	/**
	 * Remove node from tree
	 * 
	 * ! Only guarantees that the value is removed
	 * ! not the actual node (values are moved)
	 * 
	 * @return pointer to node actually evicted from tree
	 */
	BinaryNode * remove()
	{
		// Proceed with normal bt deleteion, then repair
		// @ref http://www.mathcs.emory.edu/~cheung/Courses/171/Syllabus/9-BinTree/BST-delete2.html
		
		BinaryNode * succ = this;

		// Get actual successor
		if (left != nullptr && right != nullptr)
			data = move((succ = right->getMin())->data);
		
		// Remove left or right child of successor
		BinaryNode * repl = nullptr;
		if (succ->left != nullptr)
		{
			if (((repl = succ->left)->next = succ->next) != nullptr)
				repl->next->prev = repl;
		}
		else if (succ->right != nullptr)
		{
			if (((repl = succ->right)->prev = succ->prev) != nullptr)
				repl->prev->next = repl;
		}
		else
		{
			if (succ->prev) succ->prev->next = succ->next;
			if (succ->next) succ->next->prev = succ->prev;
		}

		// Replace successor
		// If we had only one subtree then left may be non-null
		if (succ->parent != nullptr)
			(succ->parent->left == succ)
				// Set*Child also checks if repl is null
				? succ->parent->setLeftChild(repl)
				: succ->parent->setRightChild(repl);
		else if (repl != nullptr)
			repl->parent = nullptr;

		
		if (isBlack(succ))
			// Repair rb structure
			repairRemoved(repl, repl ? repl->parent : succ->parent);
		
		return succ;
	}

	/**
	 * Print subtree to string
	 * 
	 * @return new string
	 */
	String toString() const
	{
		String out;
		out <<= data;
		out.appendFormat("(%c)", isBlack(this) ? 'B' : 'R');

		out += " <";
		out += left ? left->toString() : "nil";
		out += ", ";
		out += right ? right->toString() : "nil";
		out += ">";

		return out;
	}
};