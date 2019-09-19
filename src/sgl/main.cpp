#include <core_types.h>
#include <containers/string.h>
#include <stdio.h>

#include <string>

int32 main()
{
	String name, test = String::format("%f", 3.14);

	Array<Array<Array<uint32>>> array;
	for (uint32 i = 0; i < 4; ++i)
	{
		array.add(Array<Array<uint32>>{});

		for (uint32 j = 0; j < 4; ++j)
		{
			array[i].add(Array<uint32>{});

			for (uint32 k = 0; k < 4; ++k)
			{
				array[i][j].add((i * 4 + j) * 4 + k);
			}
		}
	}

	name <<= array;

	printf("%s\n", *name.substr(4, 3));

	return 0;
}
