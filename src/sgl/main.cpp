#include <core_types.h>
#include <containers/tree.h>
#include <containers/string.h>
#include <containers/tuple.h>
#include <containers/map.h>
#include <math/mat4.h>
#include <stdio.h>

#include <string>

/**
 * SGL ordered map implementation
 */
void sglMap()
{
	struct LessThan
	{
		FORCE_INLINE int32 operator()(uint32 a, uint32 b) const
		{
			return int32(a > b) - int32(a < b);
		}
	};

	const uint32 numNodes = 1u << 16;
	Map<uint32, uint32, LessThan> map;

	for (uint32 i = 0; i < numNodes; ++i)
		map[i] = i << 1;
	
	for (uint32 i = 0; i < numNodes; ++i)
		map[i] = map[(numNodes - i) - 1];
	
	for (uint32 i = 0; i < numNodes; ++i)
		map.remove(i);
}

int32 main()
{
	struct FindString
	{
		FORCE_INLINE int32 operator()(const String & a, const String & b) const
		{
			return a.cmp(b);
		}
	};

	Map<String, String, FindString> user;
	user["email"] = "sneppy13@gmail.com";
	user["username"] = "sneppy";

	printf("count: %llu\n", user.getNumNodes());

	user.remove("email");

	sglMap();

	return 0;
}
