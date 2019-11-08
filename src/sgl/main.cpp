#include <core_types.h>
#include <containers/tree.h>
#include <containers/string.h>
#include <containers/tuple.h>
#include <hal/malloc_binned.h>
#include <stdio.h>

#include <string>

int32 main()
{
	srand(clock());
	
	MallocBinned * malloc = new MallocBinned;

	return 0;

	String name;

	struct Cmp
	{
		FORCE_INLINE int32 operator()(uint32 a, uint32 b) const
		{
			return int32(a > b) - int32(a < b);
		}
	};

	BinaryNode<uint32, Cmp> * root = new BinaryNode<uint32, Cmp>(2u);
	root->color = BinaryNodeColor::BLACK;

	const uint32 vals[] = {4, 6, 12, 3, 7, 10};
	
	for (uint32 i = 0; i < sizeof(vals) / sizeof(*vals); ++i)
	{
		BinaryNode<uint32, Cmp> * node = new BinaryNode<uint32, Cmp>(vals[i]);
		root->insert(node);
		root = root->getRoot();
	}

	name <<= *root;
	printf("%s\n", *name);

	{
		auto node = root->find(7u);
		if (node)
		{
			auto deleted = node->remove();
			if (node == root)
				root = node->getRoot();
		}
	}

	{
		auto node = root->find(10u);
		if (node)
		{
			auto deleted = node->remove();
			if (node == root)
				root = node->getRoot();
		}
	}

	printf("%s\n", *(name << *root));

	return 0;
}
