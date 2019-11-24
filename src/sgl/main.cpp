#include <core_types.h>
#include <containers/tree.h>
#include <containers/string.h>
#include <containers/tuple.h>
#include <containers/map.h>
#include <math/mat4.h>
#include <stdio.h>

#include <string>

int32 main()
{
	struct FindString
	{
		FORCE_INLINE int32 operator()(const String & a, const String & b) const
		{
			return a.cmp(b);
		}
	};

	Map<String, String, FindString> nameMap;
	nameMap.insert("hello", "world");
	nameMap.insert("sneppy", "Andrea Mecchia");

	printf("%s\n", *nameMap["sneppy"]);

	String name;
	if (nameMap.pop("sneppy", name))
	{
		printf("%s\n", *name);
	}

	printf("%llu", nameMap.getCount());

	return 0;
}
