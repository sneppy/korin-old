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
	FORCE_INLINE int32 operator()(uint32 a, uint32 b) const
	{
		return int32(a > b) - int32(a < b);
	}
};

int32 main()
{
	List<int> l;
	l.pushBack(1, 5);
	l.pushFront(5, 6, 1, 1, 10, 9, 4);

	for (auto v : l) printf("%d\n", v);

	Sort::quicksort(l.begin(), l.end(), LessThan{});

	for (auto v : l) printf("%d\n", v);

	return 0;
}
