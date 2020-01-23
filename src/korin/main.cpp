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
	struct FindInt3
	{
		FORCE_INLINE constexpr int32 operator()(const int3 & a, const int3 & b) const
		{
			return int32(a.cmpgt(b)) - int32(a.cmplt(b));
		}
	};

	Map<int3, uint32, FindInt3> chunkTriangles;
	for (uint32 i = 0; i < 40; ++i)
		chunkTriangles.insert(int3{rand() & 0xf, rand() & 0xf, rand() & 0xf}, (uint32)rand() & 0xf);
	
	

	return 0;
}
