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
	
	return 0;
}
