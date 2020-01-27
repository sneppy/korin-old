#include "core_types.h"
#include "containers/tree.h"
#include "containers/string.h"
#include "containers/tuple.h"
#include "containers/map.h"
#include "math/mat4.h"
#include "math/vec3.h"
#include "algorithm/sort.h"

#include "stdio.h"
#include <string>

struct LessThan
{
	template<typename T>
	FORCE_INLINE int32 operator()(const T & a, const T & b) const
	{
		return int32(a > b) - int32(a < b);
	}
};

template<>
FORCE_INLINE int32 LessThan::operator()(const String & a, const String & b) const
{
	return a.cmp(b);
}

int32 main()
{
	BinaryTree<uint32, LessThan> tree;
	tree.insert(3u);
	tree.insert(4u);
	tree.insert(2u);

	tree.insertUnique(2u);
	tree.replace(3u);
	tree.insert(4u);

	for (auto v : tree) printf("%d\n", v);

	for (auto begin = tree.begin(3u), end = tree.end(4u); begin != end; ++begin)
	{
		printf("%d\n", *begin);
	}

	return 0;
}
