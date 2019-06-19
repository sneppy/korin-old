/**
 * This file contains a sandbox for
 * testing the SGL library
 */
#include "core_types.h"
#include "hal/malloc_pool.h"
#include "containers/tree.h"

template<typename T>
struct LessThan
{
	FORCE_INLINE int32 operator()(const T & a, const T & b) const
	{
		return int32(a > b) - int32(a < b);
	}
};

int main()
{
	return 0;
}