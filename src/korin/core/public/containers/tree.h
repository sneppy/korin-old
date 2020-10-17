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
	template<typename, typename> friend class BinaryTree;
	template<typename, typename, typename> friend class Map;

	/// Data type
	using DataT = T;

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
	 * Returns tree size (number of nodes)
	 * 
	 * @param root tree root node
	 * @return number of nodes in tree
	 * 	spawning from root node
	 * @{
	 */
	static uint64 getTreeSize(BinaryNode * root)
	{
		return root ? 1 + getTreeSize(root->right) + getTreeSize(root->left) : 0;
	}

	uint64 getNumNodes()
	{
		return getTreeSize(this);
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
	template<typename U>
	const BinaryNode * find(const U & key) const
	{
		const int32 cmp = CompareT()(key, this->data);

		if (cmp < 0)
			return left ? left->find(key) : nullptr;
		else if (cmp > 0)
			return right ? right->find(key) : nullptr;
		else
			return this;
	}
	
	template<typename U>
	FORCE_INLINE BinaryNode * find(const U & key)
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
	template<typename U>
	const BinaryNode * findMin(const U & key) const
	{
		// Find matching node
		const BinaryNode * it = find(key), * jt;
		if (!it) return nullptr;

		// Now find leftmost
		do
		{
			jt = it;
			it = it->prev;
		} while (it && CompareT{}(key, it->data) == 0);	

		return jt;	
	}

	template<typename U>
	FORCE_INLINE BinaryNode * findMin(const U & key)
	{
		return const_cast<BinaryNode*>(static_cast<const BinaryNode&>(*this).findMin(key));
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
	template<typename U>
	const BinaryNode * findMax(const U & key) const
	{
		// Find matching node
		const BinaryNode * it = find(key), * jt;
		if (!it) return nullptr;

		// Now find rightmost
		do
		{
			jt = it;
			it = it->next;
		} while (it && CompareT{}(key, it->data) == 0);		

		return jt;	
	}

	template<typename U>
	FORCE_INLINE BinaryNode * findMax(const U & key)
	{
		return const_cast<BinaryNode*>(static_cast<const BinaryNode&>(*this).findMax(key));
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
		if ((left = node)) left->parent = this;
		return left;
	}

	FORCE_INLINE BinaryNode * setRightChild(BinaryNode * node)
	{
		if ((right = node)) right->parent = this;
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
	 * Replace node with left[right] subtree
	 * @{
	 */
	FORCE_INLINE void collapseRight()
	{
		// Replace parent child
		if (parent) (parent->left == this)
			? parent->setLeftChild(right)
			: parent->setRightChild(right)
			;
		else if (right) right->parent = nullptr;

		// Replace next and prev
		if (prev) prev->next = next;
		if (next) next->prev = prev;
	}

	FORCE_INLINE void collapseLeft()
	{
		// Replace parent child
		if (parent) (parent->left == this)
			? parent->setLeftChild(left)
			: parent->setRightChild(left)
			;
		else if (left) left->parent = nullptr;

		// Replace next and prev
		if (prev) prev->next = next;
		if (next) next->prev = prev;
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
		else if (node && (node->color == BinaryNodeColor::RED || parent == nullptr))
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
			if (isBlack(sibling) && isBlack(sibling->left) && isBlack(sibling->right))
			{
				sibling->color = BinaryNodeColor::RED;
				// Recursive call
				repairRemoved(parent, parent->parent);
			}
			else
			{
				// Case 3: sibling is black and inner child is red
				if (isRed(sibling->left))
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
			
			if (isBlack(sibling->left) &&isBlack(sibling->right))
			{
				sibling->color = BinaryNodeColor::RED;
				// Recursive call
				repairRemoved(parent, parent->parent);
			}
			else
			{
				// Case 3: sibling is black and inner child is red
				if (isRed(sibling->right))
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
	 * Insert node in the subtree.
	 * 
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
	 * @{
	 */
	BinaryNode * remove(BinaryNode* & outValidNode)
	{
		// Proceed with normal bt deletion, then repair
		// @ref http://www.mathcs.emory.edu/~cheung/Courses/171/Syllabus/9-BinTree/BST-delete2.html
		// @ref https://www.programiz.com/deletion-from-a-red-black-tree
		
		BinaryNode * u = this;
		BinaryNode * v = nullptr;
		BinaryNode * w = next;

		// Get actual successor
		if (left != nullptr && right != nullptr)
		{
			swap((w = this)->data, (u = next)->data);
		}
		
		// Remove left or right child of successor
		if (u->left != nullptr)
		{
			if (((v = u->left)->next = u->next) != nullptr)
				v->next->prev = v;
		}
		else if (u->right != nullptr)
		{
			if (((v = u->right)->prev = u->prev) != nullptr)
				v->prev->next = v;
		}
		else
		{
			if (u->prev) u->prev->next = u->next;
			if (u->next) u->next->prev = u->prev;
		}

		// Replace successor
		// If we had only one subtree then left may be non-null
		if (u->parent != nullptr) (u->parent->left == u)
				// Set*Child also checks if repl is null
				? u->parent->setLeftChild(v)
				: u->parent->setRightChild(v);
		else if (v != nullptr)
			v->parent = nullptr;

		// Repair rb structure
		if (isBlack(u)) repairRemoved(v, v ? v->parent : u->parent);
		
		return (outValidNode = w), u;
	}

	BinaryNode * remove()
	{
		BinaryNode * _;
		return remove(_);
	}
	/** @} */

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

/**
 * 
 */
template<typename NodeT>
struct NodeIterator
{
	template<typename> friend struct NodeConstIterator;

	//////////////////////////////////////////////////
	// Iterator types
	//////////////////////////////////////////////////
	
	using DataT = typename NodeT::DataT;
	using RefT = DataT&;
	using PtrT = DataT*;

	/**
	 * Default constructor
	 */
	FORCE_INLINE NodeIterator()
		: node{nullptr}
	{
		//
	}

	/**
	 * Initialize current node
	 */
	FORCE_INLINE NodeIterator(NodeT * inNode)
		: node{inNode}
	{
		//
	}

	//////////////////////////////////////////////////
	// BaseIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE RefT operator*() const
	{
		return node->data;
	}

	FORCE_INLINE PtrT operator->() const
	{
		return &operator*();
	}

	FORCE_INLINE bool operator==(const NodeIterator & other) const
	{
		return node == other.node;
	}

	FORCE_INLINE bool operator!=(const NodeIterator & other) const
	{
		return !(*this == other);
	}

	//////////////////////////////////////////////////
	// ForwardIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE NodeIterator & operator++()
	{
		node = node->next;
		return *this;
	}

	FORCE_INLINE NodeIterator operator++(int)
	{
		auto other = NodeIterator{node->next};
		node = node->next;
		return other;
	}

	//////////////////////////////////////////////////
	// BidirectionalIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE NodeIterator & operator--()
	{
		node = node->prev;
		return *this;
	}

	FORCE_INLINE NodeIterator operator--(int) const
	{
		return NodeIterator{node->prev};
	}

protected:
	/// Current node
	NodeT * node;
};

/**
 * 
 */
template<typename NodeT>
struct NodeConstIterator
{
	template<typename, typename> friend class BinaryTree;
	template<typename, typename, typename> friend class Map;

	//////////////////////////////////////////////////
	// Iterator types
	//////////////////////////////////////////////////
	
	using DataT = typename NodeT::DataT;
	using RefT = const DataT&;
	using PtrT = const DataT*;

	/**
	 * Default constructor
	 */
	FORCE_INLINE NodeConstIterator()
		: node{nullptr}
	{
		//
	}

	/**
	 * Initialize current node
	 */
	FORCE_INLINE NodeConstIterator(const NodeT * inNode)
		: node{inNode}
	{
		//
	}

	/**
	 * Cast non-const to const iterator
	 */
	FORCE_INLINE NodeConstIterator(const NodeIterator<NodeT> & other)
		: node{other.node}
	{
		//
	}

	//////////////////////////////////////////////////
	// BaseIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE RefT operator*() const
	{
		return node->data;
	}

	FORCE_INLINE PtrT operator->() const
	{
		return &operator*();
	}

	FORCE_INLINE bool operator==(const NodeConstIterator & other) const
	{
		return node == other.node;
	}

	FORCE_INLINE bool operator!=(const NodeConstIterator & other) const
	{
		return !(*this == other);
	}

	//////////////////////////////////////////////////
	// ForwardIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE NodeConstIterator & operator++()
	{
		node = node->next;
		return *this;
	}

	FORCE_INLINE NodeConstIterator operator++(int)
	{
		auto other = NodeConstIterator{node->next};
		node = node->next;
		return other;
	}

	//////////////////////////////////////////////////
	// BidirectionalIterator interface
	//////////////////////////////////////////////////
	
	FORCE_INLINE NodeConstIterator & operator--()
	{
		node = node->prev;
		return *this;
	}

	FORCE_INLINE NodeConstIterator operator--(int) const
	{
		return NodeConstIterator{node->prev};
	}

protected:
	/// Current node
	NodeT * node;
};

/**
 * Red and black binary tree
 * implementation, built on top
 * of BinaryNode class
 */
template<typename T, typename CompareT>
class BinaryTree
{
	template<typename, typename, typename>	friend class Map;
	template<typename, typename>			friend class Set;

public:
	/// Expose data type
	using DataT = T;

	/// Induced node type
	using NodeT = BinaryNode<T, CompareT>;

	/// Iterator type
	using Iterator = NodeIterator<NodeT>;

	/// Const node iterator
	using ConstIterator = NodeConstIterator<NodeT>;

protected:
	/// Used allocator
	mutable MallocBase * malloc;

	/// Has own allocator flag
	bool bHasOwnMalloc;

	/// Tree root node
	NodeT * root;

	/// Number of nodes
	uint64 numNodes;

public:
	/**
	 * Default constructor
	 */
	FORCE_INLINE BinaryTree()
		: malloc{nullptr}
		, bHasOwnMalloc{true}
		, root{nullptr}
		, numNodes{0ull}
	{
		// Create allocator
		malloc = new MallocAnsi;
	}

	/**
	 * Initialize allocator
	 */
	FORCE_INLINE BinaryTree(MallocBase * inMalloc)
		: malloc{inMalloc}
		, bHasOwnMalloc{false}
		, root{nullptr}
		, numNodes{0ull}
	{
		//
	}

protected:
	/**
	 * Create node using allocation policy
	 * 
	 * @param data node data
	 * @return pointer to created node
	 */
	template<typename TT>
	FORCE_INLINE NodeT * createNode(TT && data) const
	{
		return new (malloc->alloc(sizeof(NodeT), alignof(NodeT))) NodeT{forward<TT>(data)};
	}

	/**
	 * Destroy node
	 * 
	 * @param node pointer to node
	 */
	FORCE_INLINE void destroyNode(NodeT * node) const
	{
		node->~NodeT();
		malloc->free(node);
	}

	/**
	 * Delete subtree spawning from
	 * node. Root cannot be null
	 * 
	 * @param root subtree root
	 */
	void destroySubtree(NodeT * root)
	{
		if (root->left) destroySubtree(root->left);
		if (root->right) destroySubtree(root->right);

		destroyNode(root);
	}

	/**
	 * Copy construct subtree and
	 * return root
	 * 
	 * @param src source subtree root
	 * @return new subtree root
	 */
	NodeT * cloneSubtree(const NodeT * src)
	{
		// Copy data and color
		NodeT * dst = createNode(src->data);
		dst->color = src->color;
		
		// Copy left subtree
		if (src->left)
		{
			NodeT * left = cloneSubtree(src->left);
			NodeT * prev = left->getMax();

			(dst->left = left)->parent = dst;
			(dst->prev = prev)->next = dst;
		}

		// Copy right subtree
		if (src->right)
		{
			NodeT * right = cloneSubtree(src->right);
			NodeT * next = right->getMin();
			
			(dst->right = right)->parent = dst;
			(dst->next = next)->prev = dst;
		}

		return dst;
	}

	/**
	 * Copy subtree and return root.
	 * If dst is null a new subtree
	 * is created
	 * 
	 * @param dst existing subtree root
	 * @param src source subtree root
	 * @return subtree root
	 */
	NodeT * cloneSubtree(NodeT * dst, const NodeT * src)
	{
		// Copy data and color
		dst->data = src->data;
		dst->color = src->color;

		if (src->left)
		{
			NodeT * left = dst->left ? cloneSubtree(dst->left, src->left) : cloneSubtree(src->left);
			NodeT * prev = left->getMax();

			(dst->left = left)->parent = dst;
			(dst->prev = prev)->next = dst;
		}
		else if (dst->left)
		{
			destroySubtree(dst->left);
			dst->left = nullptr;
			dst->prev = nullptr;
		}

		if (src->right)
		{
			NodeT * right = dst->right ? cloneSubtree(dst->right, src->right) : cloneSubtree(src->right);
			NodeT * next = right->getMin();

			(dst->right = right)->parent = dst;
			(dst->next = next)->prev = dst;
		}
		else if (dst->right)
		{
			destroySubtree(dst->right);
			dst->right = nullptr;
			dst->next = nullptr;
		}

		return dst;
	}

public:
	/**
	 * Copy constructor
	 */
	FORCE_INLINE BinaryTree(const BinaryTree & other)
		: BinaryTree{}
	{
		root = cloneSubtree(other.root);
		numNodes = other.numNodes;
	}	
	
	/**
	 * Move constructor
	 */
	FORCE_INLINE BinaryTree(BinaryTree && other)
		: malloc{other.malloc}
		, bHasOwnMalloc{other.bHasOwnMalloc}
		, root{other.root}
		, numNodes{other.numNodes}
	{
		other.malloc = nullptr;
		other.bHasOwnMalloc = false;
		other.root = nullptr;
		other.numNodes = 0ull;
	}

	/**
	 * Copy assignment
	 */
	FORCE_INLINE BinaryTree & operator=(const BinaryTree & other)
	{
		// Empty tree
		if (!other.root) empty();
		// Clone subtree starting from existing root
		else if (root) cloneSubtree(root, other.root);
		// Clòne subtree from zero
		else root = cloneSubtree(other.root);

		numNodes = other.numNodes;

		return *this;
	}

	/**
	 * Move assignment
	 */
	FORCE_INLINE BinaryTree & operator=(BinaryTree && other)
	{
		// Destroy subtree
		if (root) destroySubtree(root);

		// Delete allocator
		if (bHasOwnMalloc) delete malloc;

		malloc = other.malloc;
		bHasOwnMalloc = other.bHasOwnMalloc;
		root = other.root;
		numNodes = other.numNodes;

		other.malloc = nullptr;
		other.bHasOwnMalloc = false;
		other.root = nullptr;
		other.numNodes = 0ull;

		return *this;
	}

	/**
	 * Delete all nodes
	 */
	FORCE_INLINE void empty()
	{
		if (root) destroySubtree(root);

		// Reset tree
		root = nullptr;
		numNodes = 0;
	}

	/**
	 * Tree destructor
	 */
	FORCE_INLINE ~BinaryTree()
	{
		empty();

		if (bHasOwnMalloc)
		{
			delete malloc;

			malloc = nullptr;
			bHasOwnMalloc = false;
		}
	}

	/**
	 * Returns tree size (i.e. number
	 * of nodes)
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
	 * Return pointer to root node
	 * @{
	 */
	FORCE_INLINE const NodeT * getRoot() const
	{
		return root;
	}

	FORCE_INLINE NodeT * getRoot()
	{
		return root;
	}
	/** @} */

	/**
	 * Return pointer to leftmost node,
	 * which holds the min value.
	 * @{
	 */
	FORCE_INLINE const NodeT * getMin() const
	{
		return root ? root->getMin() : nullptr;
	}

	FORCE_INLINE NodeT * getMin()
	{
		return root ? root->getMin() : nullptr;
	}
	/** @} */

	/**
	 * Return pointer to rightmost node,
	 * which holds the max value.
	 * @{
	 */
	FORCE_INLINE const NodeT * getMax() const
	{
		return root ? root->getMax() : nullptr;
	}

	FORCE_INLINE NodeT * getMax()
	{
		return root ? root->getMax() : nullptr;
	}
	/** @} */

	/**
	 * Returns a new iterator pointing
	 * to the min element of the tree
	 * (i.e. leftmost element) or the
	 * first element that matches the
	 * provided argument (if any)
	 * 
	 * @param arg search criteria
	 * @return Iterator pointing to the
	 * 	beginning of the search
	 * @{
	 */
	FORCE_INLINE ConstIterator begin() const
	{
		return ConstIterator{root ? root->getMin() : nullptr};
	}

	FORCE_INLINE Iterator begin()
	{
		return Iterator{root ? root->getMin() : nullptr};
	}

	template<typename U>
	FORCE_INLINE ConstIterator begin(const U & arg) const
	{
		return ConstIterator{root ? root->findMin(arg) : nullptr};
	}

	template<typename U>
	FORCE_INLINE Iterator begin(const U & arg)
	{
		return Iterator{root ? root->findMin(arg) : nullptr};
	}
	/// @}

	/**
	 * Returns a new iterator pointing
	 * to the element right before the
	 * last element matching the datum
	 * or to the end of the tree if no
	 * argument is provided
	 * 
	 * @param arg search criteria
	 * @return Iterator pointing to the
	 * 	end of the search
	 */
	FORCE_INLINE ConstIterator end() const
	{
		return ConstIterator{nullptr};
	}

	FORCE_INLINE Iterator end()
	{
		return Iterator{nullptr};
	}

	template<typename U>
	ConstIterator end(const U & arg) const
	{
		if (root)
		{
			NodeT * last = root->findMax(arg);
			return ConstIterator{last ? last->next : nullptr};
		}
		else return nullptr;
	}

	template<typename U>
	Iterator end(const U & arg)
	{
		if (root)
		{
			NodeT * last = root->findMax(arg);
			return Iterator{last ? last->next : nullptr};
		}
		else return nullptr;
	}
	/// @}

	/**
	 * Returns pointer to node that
	 * matches search criteria or
	 * null if no such node exists
	 * 
	 * @param arg search criteria
	 * @return Pointer to node
	 */
	/// @{
	template<typename U>
	FORCE_INLINE const NodeT * findNode(const U & arg) const
	{
		return root ? root->find(arg) : nullptr;
	}

	template<typename U>
	FORCE_INLINE NodeT * findNode(const U & arg)
	{
		return root ? root->find(arg) : nullptr;
	}
	/// @}

	/**
	 * Returns a new iterator that
	 * points to the first node
	 * that matches criteria
	 * 
	 * @param arg search criteria
	 * @return iterator that points
	 * 	matching node (if found)
	 * @{
	 */
	template<typename U>
	FORCE_INLINE ConstIterator find(const U & arg) const
	{
		return ConstIterator{findNode(arg)};
	}
	
	template<typename U>
	FORCE_INLINE Iterator find(const U & arg)
	{
		return Iterator{findNode(arg)};
	}
	/// @}

	/**
	 * Insert node, possible duplicate.
	 * If multiple datums are provided
	 * they are inserted in the order
	 * in which they are presented (left
	 * to right)
	 * 
	 * @param arg,args data to insert
	 * @return reference to inserted data
	 * @{
	 */
	template<typename TT>
	T & insert(TT && arg)
	{
		if (root)
		{
			// Insert node
			NodeT * node = createNode(forward<TT>(arg));
			root->insert(node);
			root = root->getRoot();

			++numNodes;

			return node->data;
		}
		else
		{
			// Insert as root
			root = createNode(forward<TT>(arg));
			root->color = BinaryNodeColor::BLACK;

			numNodes = 1;

			return root->data;
		}
	}

	template<typename TT, typename ... TTT>
	void insert(TT && arg, TTT && ... args)
	{
		insert(forward<TT>(arg));
		insert(forward<TTT>(args)...);
	}
	/// @}

	/**
	 * Insert node, without overriding
	 * existing duplicate node. If such
	 * a node exists, the new node is
	 * not inserted and a reference to
	 * the existing data is returned
	 * 
	 * @param arg,args data to insert
	 * @return reference to matching data
	 * @{
	 */
	template<typename TT>
	T & insertUnique(TT && arg)
	{
		if (root)
		{
			// Find node
			NodeT * it = root, * parent = nullptr;
			while (it)
			{
				parent = it;

				int32 cmp = CompareT{}(arg, it->data);
				if (cmp < 0)
					it = it->left;
				else if (cmp > 0)
					it = it->right;
				else /* cmp == 0 */
					// Return without inserting node
					return it->data;
			}
			
			// Insert node
			NodeT * node = parent->insert(createNode(forward<TT>(arg)));
			root = root->getRoot();

			++numNodes;
			return node->data;
		}
		else
		{
			// Insert as root
			root = createNode(forward<TT>(arg));
			root->color = BinaryNodeColor::BLACK;

			numNodes = 1;

			return root->data;
		}
	}

	template<typename TT, typename ... TTT>
	void insertUnique(TT && arg, TTT && ... args)
	{
		insertUnique(forward<TT>(arg));
		insertUnique(forward<TTT>(args)...);
	}
	/// @}

	/**
	 * Insert node, overriding duplicate
	 * nodes if necessary. if more duplicate
	 * node exists, the first one encountered
	 * is replaced
	 * 
	 * @param arg,args data to insert
	 * @return reference to inserted data
	 * @{
	 */
	template<typename TT>
	T & replace(TT && arg)
	{
		if (root)
		{
			// Find node
			NodeT * it = root, * parent = nullptr;
			while (it)
			{
				parent = it;

				int32 cmp = CompareT{}(arg, it->data);
				if (cmp < 0)
					it = it->left;
				else if (cmp > 0)
					it = it->right;
				else
					// Replacing existing data
					return (it->data = forward<TT>(arg));
			}
			
			// Insert new node
			NodeT * node = parent->insert(createNode(forward<TT>(arg)));

			++numNodes;
			return node->data;
		}
		else
		{
			// Insert as root
			root = createNode(forward<TT>(arg));
			root->color = BinaryNodeColor::BLACK;

			numNodes = 1;

			return root->data;
		}
	}

	template<typename TT, typename ... TTT>
	void replace(TT && arg, TTT && ... args)
	{
		replace(forward<TT>(arg));
		replace(forward<TTT>(args)...);
	}
	/// @}

	/**
	 * Remove node from tree
	 * 
	 * @param node node to remove
	 * @return ptr to valid node afte deletion,
	 * 	if any
	 */
	NodeT * removeNode(NodeT * node)
	{
		NodeT * removed = nullptr;
		NodeT * valid = nullptr;

		// Remove and ensure root is valid
		if ((removed = node->remove(valid)) == root) root = root->left ? root->left : root->right;
		if (root) root = root->getRoot();

		--numNodes;

		// Destroy node
		destroyNode(removed);

		return valid;
	}

	/**
	 * Remove node pointed by
	 * iterator
	 * 
	 * @param it iterator that points
	 * 	to the node to remove
	 */
	FORCE_INLINE bool remove(const ConstIterator & it)
	{
		if (it.node)
		{
			// Node exists, remove it
			removeNode(it.node);
			return true;
		}

		return false;
	}

	/**
	 * @brief Remove node that matches value
	 * 
	 * @param value node value
	 */
	FORCE_INLINE bool remove(const T & value)
	{
		// Find node and remove it
		if (NodeT * node = findNode(value))
		{
			// Node found, remove it
			removeNode(node);
			return true;
		}

		return false;
	}
};