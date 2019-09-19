#pragma once

#include "gtest/gtest.h"

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

	Array<uint32> e;
	e.add(4u);
	e.add(8u);
	e.add(2u);
	e.removeAt(1);

	ASSERT_EQ(e.getCount(), 2);
	ASSERT_EQ(e[0], 4u);
	ASSERT_EQ(e[1], 2u);

	Array<Array<Array<int32>>> f;
	for (int32 i = 0; i < 3; ++i)
	{
		f.add(Array<Array<int32>>{});

		for (int32 j = 0; j < 3; ++j)
		{
			f[i].add(Array<int32>{});

			for (int32 k = 0; k < 3; ++k)
			{
				f[i][j].add((i * 3 + j) * 3 + k);
			}
		}
	}

	ASSERT_EQ(f.getCount(), 3);

	for (uint32 i = 0; i < 3; ++i)
	{
		ASSERT_EQ(f[i].getCount(), 3);

		for (uint32 j = 0; j < 3; ++j)
		{
			ASSERT_EQ(f[i][j].getCount(), 3);

			for (uint32 k = 0; k < 3; ++k)
			{
				ASSERT_EQ(f[i][j][k], (i * 3 + j) * 3 + k);
			}
		}
	}

	f.removeAt(1);

	ASSERT_EQ(f.getCount(), 2);
	ASSERT_EQ(f[1][0][0], 18);
	ASSERT_EQ(f[0][1][2], 5);
	ASSERT_EQ(f[1][1][1], 22);

	f.removeLast();

	ASSERT_EQ(f.getCount(), 1);
	ASSERT_EQ(f[0][2][1], 7);
	ASSERT_EQ(f[0][1][1], 4);
	ASSERT_EQ(f[0][0][1], 1);

	f[0].removeFirst();

	ASSERT_EQ(f.getCount(), 1);
	ASSERT_EQ(f[0][1][2], 8);
	ASSERT_EQ(f[0][1][1], 7);
	ASSERT_EQ(f[0][0][1], 4);

	f.removeFirst();

	ASSERT_EQ(f.getCount(), 0);

	Array<int32> g{(const int32[]){5, 1, 4, 2, 7, 8, 3}, 7};
	
	ASSERT_EQ(g.getCount(), 7);
	ASSERT_EQ(g[0], 5);
	ASSERT_EQ(g[6], 3);
	ASSERT_EQ(g[3], 2);

	int32 gg;
	g.popAt(2, gg);

	ASSERT_EQ(g.getCount(), 6);
	ASSERT_EQ(g[2], 2);
	ASSERT_EQ(gg, 4);

	a.reset();
	b.reset();
	c.reset();
	d.reset();
	e.reset();
	f.reset();
	g.reset();

	SUCCEED();
}

TEST(containers, string)
{
	ASSERT_EQ(PlatformStrings::getLength("sneppy"), 6u);
	ASSERT_EQ(PlatformStrings::getLength("abracadabra"), 11u);

	ASSERT_EQ(PlatformStrings::cmp("sneppy", "sneppy"), 0);
	ASSERT_EQ(PlatformStrings::cmp("snappy", "sneppy"), 'a' - 'e');
	ASSERT_EQ(PlatformStrings::cmp("sneppy", "sn"), 'e');

	ASSERT_EQ(PlatformStrings::icmp("sNePpY", "SNEppy"), 0);
	ASSERT_EQ(PlatformStrings::icmp("sneppy", "SNEPPY"), 0);

	ASSERT_EQ(PlatformStrings::cmpn("snep", "sn", 2), 0);
	ASSERT_EQ(PlatformStrings::cmpn("snyp", "snap", 2), 0);
	ASSERT_EQ(PlatformStrings::cmpn("snep", "snap", 2, 2), 'e' - 'a');

	ASSERT_EQ(PlatformStrings::icmpn("sNelLY", "SNEppy", 3), 0);
	ASSERT_EQ(PlatformStrings::icmpn("sneppy", "UNEPPY", 3, 1), 0);

	String a = "sneppy";
	String b = "rulez";
	
	ASSERT_EQ(a[6], '\0');
	ASSERT_EQ(b[5], '\0');

	ASSERT_EQ(PlatformStrings::cmp(a.getData(), "sneppy"), 0);
	ASSERT_EQ(PlatformStrings::cmp(b.getData(), "rulez"), 0);

	ASSERT_EQ(a.getLength(), 6);
	ASSERT_EQ(b.getLength(), 5);

	ASSERT_EQ(a.cmp(b), 's' - 'r');
	ASSERT_EQ(a.cmp("sneppy"), 0);
	ASSERT_EQ(a.cmp("sne"), 'p');

	ASSERT_FALSE(a == b);
	ASSERT_FALSE(b == a);
	ASSERT_TRUE(a != b);
	ASSERT_TRUE(b != a);
	ASSERT_TRUE(a == "sneppy");
	ASSERT_FALSE(a != "sneppy");
	ASSERT_FALSE(a == "snep");
	ASSERT_TRUE(a != "snep");
	
	String c, d, e;
	
	a <<= 3u;
	b.printFormat("%.2f", 3.14f);

	ASSERT_EQ(a, "3");
	ASSERT_EQ(b, "3.14");

	c = a << 4u;
	d = b << 4.5f;

	ASSERT_TRUE(c == "4");
	ASSERT_TRUE(d == "4.500000");

	e = a + ", " + b + ", " + c + ", " + d;

	ASSERT_TRUE(e == "3, 3.14, 4, 4.500000");

	e += ", ";
	e += 12;

	ASSERT_TRUE(e == "3, 3.14, 4, 4.500000, 12");

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
	using Node = BinaryNode<uint32, LessThan<uint32>>;

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

	node = root->insert(new Node(1u));

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

	node = new Node(2u);

	ASSERT_NE(root->insertUnique(node), node);
	ASSERT_EQ(root->insertUnique(node), root->left);

	node = new Node(0u);

	ASSERT_EQ(root->insertUnique(node), node);
}

TEST(containers, map)
{
	using PairT = Pair<uint32, uint32, LessThan<uint32>>;
	PairT pair(8u, 3u);
	
	ASSERT_EQ(pair.getKey(), 8);
	ASSERT_EQ(pair.getVal(), 3);

	pair = PairT(4u);

	ASSERT_EQ(pair.getKey(), 4);
	ASSERT_EQ(pair.getVal(), 0);

	using Node = BinaryNode<PairT, PairT::FindPair>;

	Node * root = new Node(PairT(8u, 3u));
	root->color = Node::Color::BLACK;
	
	ASSERT_EQ(root->data.first, 8);
	ASSERT_EQ(root->data.second, 3);

	ASSERT_EQ(root->find(PairT(8u)), root);
	ASSERT_TRUE(root->find(PairT(4u)) == nullptr);

	root->insertUnique(new Node(PairT(4u, 9u)));
	root = root->getRoot();
	root->insertUnique(new Node(PairT(7u, 2u)));
	root = root->getRoot();
	root->insertUnique(new Node(PairT(8u, 11u)));
	root = root->getRoot();

	ASSERT_EQ(root->find(PairT(4u))->data.second, 9u);
	ASSERT_EQ(root->find(PairT(7u))->data.second, 2u);
	ASSERT_EQ(root->find(PairT(8u))->data.second, 3u);

	SUCCEED();
}