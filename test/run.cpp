/**
 * This file contains a sandbox for
 * testing the SGL library
 */
#include "core_types.h"
#include "hal/malloc_pool.h"
#include "containers/tree.h"

template<typename T>
struct LessThan
{
	FORCE_INLINE int32 operator()(const T & a, const T & b) const
	{
		return int32(a > b) - int32(a < b);
	}
};

int main()
{
	srand(clock());

	using Node = BinaryNode<uint32, LessThan<uint32>>;
	MallocPoolInline<65536, sizeof(Node), alignof(Node)> pool;

	Node * root = new (reinterpret_cast<Node*>(pool.alloc(sizeof(Node), alignof(Node)))) Node(0u);
	root->color = Node::Color::BLACK;
	
	for (uint32 i = 0; i < 65535; ++i)
	{
		Node * node = new (reinterpret_cast<Node*>(pool.alloc(sizeof(Node), alignof(Node)))) Node((uint32)rand());
		root->insertUnique(node);
		root = root->getRoot();
	}

	return 0;
}