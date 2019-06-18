#pragma once

#include <gtest/gtest.h>

#include "containers/array.h"
#include "containers/string.h"
#include "containers/list.h"
#include "containers/pair.h"
#include "containers/tree.h"

#include "hal/malloc_ansi.h"
#include "hal/malloc_pool.h"

TEST(containers, array)
{
	Array<uint64, MallocAnsi> a(0, 0, nullptr);
	Array<uint64, MallocAnsi> b(32, 32, nullptr);
	
	ASSERT_EQ(a.getCount(), 0);
	ASSERT_EQ(b.getCount(), 32);

	uint64 s, r, t;
	r = rand();
	t = a.add(r);

	ASSERT_EQ(a.getCount(), 1);
	ASSERT_EQ(t, r);
	ASSERT_EQ(a[0], t);

	a.add(t);

	Array<uint64, MallocAnsi> c(a);

	ASSERT_EQ(c[0], a[0]);
	ASSERT_EQ(c[1], a[1]);
	ASSERT_EQ(c.getCount(), a.getCount());

	Array<Array<Array<uint64>>> d;

	d.add(Array<Array<uint64>>(16, 8, nullptr));
	d.add(Array<Array<uint64>>(8, 8, nullptr));

	ASSERT_EQ(d.getCount(), 2);
	ASSERT_EQ(d[0].getCount(), 8);
	ASSERT_EQ(d[1].getCount(), 8);

	SUCCEED();
}

TEST(containers, string)
{
	String a = "sneppy";
	String b = "rulez";

	ASSERT_EQ(a.getLength(), 6);
	ASSERT_EQ(b.getLength(), 5);
	ASSERT_EQ(a[6], '\0');
	ASSERT_EQ(b[5], '\0');
	ASSERT_TRUE(strcmp(a.getData(), "sneppy") == 0);
	ASSERT_TRUE(strcmp(b.getData(), "rulez") == 0);

	const String c = a + ' ' + b;
	
	ASSERT_EQ(c.getLength(), 12);
	ASSERT_EQ(c[12], '\0');
	ASSERT_TRUE(strcmp(c.getData(), "sneppy rulez") == 0);

	SUCCEED();
}

TEST(containers, list)
{
	List<uint32> list;
	uint32 n;
	bool bNotEmpty;

	ASSERT_EQ(list.getLength(), 0);
	ASSERT_TRUE(list.getHead() == nullptr);
	ASSERT_TRUE(list.getTail() == nullptr);

	list.pushFront(4u);
	list.pushFront(3u);

	ASSERT_EQ(list.getLength(), 2);
	ASSERT_EQ(list.getHead()->data, 3);
	ASSERT_EQ(list.getTail()->data, 4);

	list.pushBack(5u);
	list.pushBack(6u);

	ASSERT_EQ(list.getLength(), 4);
	ASSERT_EQ(list.getHead()->data, 3);
	ASSERT_EQ(list.getTail()->data, 6);

	auto link = list.getHead()->next;

	ASSERT_EQ(link->data, 4);

	list.remove(link);

	ASSERT_EQ(list.getLength(), 3);
	ASSERT_EQ(list.getHead()->data, 3);
	ASSERT_EQ(list.getTail()->data, 6);

	list.remove(list.getHead());
	
	ASSERT_EQ(list.getLength(), 2);
	ASSERT_EQ(list.getHead()->data, 5);
	ASSERT_EQ(list.getTail()->data, 6);

	bNotEmpty = list.popBack(n);

	ASSERT_EQ(list.getLength(), 1);
	ASSERT_EQ(list.getHead(), list.getTail());
	ASSERT_EQ(n, 6);

	bNotEmpty = list.popFront(n);

	ASSERT_EQ(list.getLength(), 0);
	ASSERT_EQ(list.getHead(), nullptr);
	ASSERT_EQ(list.getTail(), nullptr);
	ASSERT_EQ(n, 5);

	bNotEmpty = list.popFront(n);

	ASSERT_FALSE(bNotEmpty);

	SUCCEED();
}

template<typename T>
struct LessThan
{
	FORCE_INLINE int32 operator()(const T & a, const T & b) const
	{
		return int32(a > b) - int32(a < b);
	}
};

TEST(containers, tree)
{
	using Node = BinaryNode<uint32, LessThan<float32>>;

	Node * root = new Node(3u), * node = nullptr;
	root->color = Node::Color::BLACK;
	
	ASSERT_EQ(root->find(3), root);
	ASSERT_TRUE(root->find(2) == nullptr);

	node = new Node(2u);
	root->insert(node);

	ASSERT_EQ(root->left, node);
	ASSERT_EQ(root->find(2), node);
	ASSERT_EQ(node->parent, root);

	node = new Node(4u);
	root->insert(node);

	ASSERT_EQ(root->right, node);
	ASSERT_EQ(root->find(4), node);
	ASSERT_EQ(node->parent, root);

	node = root->insert(new Node(5u));
	node = root->insert(new Node(6u));
	node = root->insert(new Node(7u));

	ASSERT_TRUE(root->parent == nullptr);
	ASSERT_EQ(root->left->data, 2);
	ASSERT_EQ(root->right->data, 5);
	ASSERT_EQ(root->right->left->data, 4);
	ASSERT_EQ(root->right->right->data, 6);
	ASSERT_EQ(root->right->right->right->data, 7);
	ASSERT_TRUE(root->right->right->left == nullptr);

	node = root->insert(new Node(1));

	ASSERT_EQ(root->getMin()->data, 1);
	ASSERT_EQ(root->getMax()->data, 7);

	node = root->getMin();

	for (uint32 i = 1; i <= 7; ++i, node = node->next)
		ASSERT_EQ(node->data, i);
	
	ASSERT_TRUE(node == nullptr);

	node = root->getMax();

	for (uint32 i = 7; i >= 1; --i, node = node->prev)
		ASSERT_EQ(node->data, i);
	
	ASSERT_TRUE(node == nullptr);
}

TEST(containers, map)
{
	Pair<uint32, uint32> pair(8u, 3u);
	
	ASSERT_EQ(pair.getKey(), 8);
	ASSERT_EQ(pair.getVal(), 3);

	pair = Pair<uint32, uint32>(4u);

	ASSERT_EQ(pair.getKey(), 4);
	ASSERT_EQ(pair.getVal(), 0);

	SUCCEED();
}