#pragma once

#include "gtest/gtest.h"

#include "containers/array.h"
#include "containers/string.h"
#include "containers/list.h"
#include "containers/tuple.h"
#include "containers/tree.h"
#include "containers/pair.h"
#include "containers/map.h"

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

	Array<int32> h = T(1, 5, 1, 3, 4);

	ASSERT_EQ(h.getCount(), 5);

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

	/// General test

	List<uint32> a;

	for (uint32 i = 0; i < 0x1000; ++i)
		a.pushBack(i);
	
	List<uint32> b{a};

	for (uint32 i = 0; i < 0x800; ++i)
		a.removeBack();
	
	List<uint32> c{a}, d{List<uint32>{}};
	a = b;
	c = d;

	SUCCEED();
}

TEST(containers, tuple)
{
	auto tup0 = Tuple<int, 5>{4, 5, 1, 8, 8};

	ASSERT_EQ(tup0.len, 5);
	ASSERT_EQ(tup0[0], 4);
	ASSERT_EQ(tup0[1], 5);
	ASSERT_EQ(tup0[2], 1);
	ASSERT_EQ(tup0[3], 8);
	ASSERT_EQ(tup0[4], 8);
	ASSERT_EQ(tup0[0], tup0[-5]);
	ASSERT_EQ(tup0[1], tup0[-4]);
	ASSERT_EQ(tup0[2], tup0[-3]);
	ASSERT_EQ(tup0[3], tup0[-2]);
	ASSERT_EQ(tup0[4], tup0[-1]);

	auto tup1 = T(4, 5, 1);

	ASSERT_TRUE((IsSameType<decltype(tup1[0]), int&>::value));
	ASSERT_EQ(tup1.len, 3);
	ASSERT_EQ(tup1[0], 4);
	ASSERT_EQ(tup1[1], 5);
	ASSERT_EQ(tup1[2], 1);

	auto tup2 = T(T(1.f, 2.f), T(4.f, 2.f));

	ASSERT_TRUE((IsSameType<decltype(tup2[0]), Tuple<float32, 2>&>::value));

	auto tup3 = T(tup1[1], tup1[0], 0);

	ASSERT_EQ(tup3.len, 3);
	ASSERT_EQ(tup3[0], tup1[1]);
	ASSERT_EQ(tup3[1], tup1[0]);
	ASSERT_EQ(tup3[2], 0);

	auto tup4 = tup3 + 1;

	ASSERT_EQ(tup4.len, 4);
	ASSERT_EQ(tup3[0], tup3[0]);
	ASSERT_EQ(tup3[1], tup3[1]);
	ASSERT_EQ(tup3[2], tup3[2]);
	ASSERT_EQ(tup4[3], 1);

	auto tup5 = tup3 + tup4;

	ASSERT_EQ(tup5.len, tup3.len + tup4.len);
	ASSERT_EQ(tup5[0], tup3[0]);
	ASSERT_EQ(tup5[1], tup3[1]);
	ASSERT_EQ(tup5[2], tup3[2]);
	ASSERT_EQ(tup5[3], tup4[0]);
	ASSERT_EQ(tup5[4], tup4[1]);
	ASSERT_EQ(tup5[5], tup4[2]);
	ASSERT_EQ(tup5[6], tup4[3]);

	auto tup6 = tup5.slice<4, -1>();
	ASSERT_EQ(tup6.len, 2);
	ASSERT_EQ(tup6[0], tup5[4]);
	ASSERT_EQ(tup6[1], tup5[5]);
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

	Node * root = new Node{3u}, * node = nullptr;
	root->color = BinaryNodeColor::BLACK;
	
	ASSERT_EQ(root->find(3u), root);
	ASSERT_TRUE(root->find(2u) == nullptr);

	node = new Node{2u};
	root->insert(node);

	ASSERT_EQ(root->left, node);
	ASSERT_EQ(root->find(2), node);
	ASSERT_EQ(node->parent, root);

	node = new Node{4u};
	root->insert(node);
	root = root->getRoot();

	ASSERT_EQ(root->right, node);
	ASSERT_EQ(root->find(4), node);
	ASSERT_EQ(node->parent, root);

	node = root->insert(new Node{5u});
	root = root->getRoot();
	node = root->insert(new Node{6u});
	root = root->getRoot();
	node = root->insert(new Node{7u});
	root = root->getRoot();

	ASSERT_TRUE(root->parent == nullptr);
	ASSERT_EQ(root->left->data, 2u);
	ASSERT_EQ(root->right->data, 5u);
	ASSERT_EQ(root->right->left->data, 4u);
	ASSERT_EQ(root->right->right->data, 6u);
	ASSERT_EQ(root->right->right->right->data, 7u);
	ASSERT_TRUE(root->right->right->left == nullptr);

	node = root->insert(new Node{1u});
	root = root->getRoot();

	ASSERT_EQ(root->getMin()->data, 1u);
	ASSERT_EQ(root->getMax()->data, 7u);

	node = root->getMin();

	for (uint32 i = 1; i <= 7; ++i, node = node->next)
		ASSERT_EQ(node->data, i);
	
	ASSERT_TRUE(node == nullptr);

	node = root->getMax();

	for (uint32 i = 7; i >= 1; --i, node = node->prev)
		ASSERT_EQ(node->data, i);
	
	ASSERT_TRUE(node == nullptr);

	node = new Node{2u};

	ASSERT_NE(root->insertUnique(node), node);
	ASSERT_EQ(root->insertUnique(node), root->left);

	node = new Node{0u};

	ASSERT_EQ(root->insertUnique(node), node);

	root = root->getRoot();
	root->getMax()->remove();
	
	ASSERT_EQ(root->getMax()->data, 6u);

	root->getMin()->remove();

	ASSERT_EQ(root->getMin()->data, 1u);

	SUCCEED();
}

struct FindString
{
	FORCE_INLINE int32 operator()(const String & a, const String & b)
	{
		return a.cmp(b);
	}
};

TEST(containers, map)
{
	Map<uint32, String, LessThan<uint32>> a;
	
	ASSERT_EQ(a.getCount(), 0);
	ASSERT_EQ(a.find(8u), nullptr);

	a.insert(4u, "sneppy");

	ASSERT_EQ(a.getCount(), 1);
	ASSERT_EQ(a.find(8u), nullptr);
	ASSERT_NE(a.find(4u), nullptr);
	ASSERT_EQ(a.find(4u)->second, "sneppy");

	a.insert(2u, "two");
	a.insert(8u, "eight");

	ASSERT_EQ(a.getCount(), 3);
	ASSERT_EQ(a.find(8u)->second, "eight");
	ASSERT_EQ(a.find(2u)->second, "two");

	String str;
	
	ASSERT_TRUE(a.find(2u, str));
	ASSERT_EQ(str, "two");
	ASSERT_TRUE(a.find(8u, str));
	printf("%s == %s\n", *str, "eight");
	printf("%u\n", str == "eight");
	ASSERT_EQ(str, "eight");
	ASSERT_TRUE(a.find(4u, str));
	ASSERT_EQ(str, "sneppy");
	ASSERT_FALSE(a.find(1u, str));
	ASSERT_EQ(str, "sneppy");

	ASSERT_TRUE(a.pop(2u, str));
	ASSERT_EQ(a.getCount(), 2);
	ASSERT_EQ(str, "two");
	ASSERT_TRUE(a.pop(4u, str));
	ASSERT_EQ(a.getCount(), 1);
	ASSERT_EQ(str, "sneppy");
	ASSERT_FALSE(a.pop(13u, str));
	ASSERT_EQ(a.getCount(), 1);
	ASSERT_EQ(str, "sneppy");

	Map<String, String, FindString> b;
	b.insert("username", "sneppy");
	b.insert("email", "sneppy13@gmail.com");
	
	ASSERT_EQ(b.getCount(), 2);
	ASSERT_EQ(b["email"], "sneppy13@gmail.com");

	ASSERT_TRUE(b.pop("username", str));
	ASSERT_EQ(b.getCount(), 1);
	ASSERT_EQ(str, "sneppy");
	ASSERT_TRUE(b.pop("email", str));
	ASSERT_EQ(b.getCount(), 0);
	ASSERT_EQ(str, "sneppy13@gmail.com");
	ASSERT_FALSE(b.pop("password", str));
	ASSERT_EQ(b.getCount(), 0);
	ASSERT_EQ(str, "sneppy13@gmail.com");

	SUCCEED();
}