/**
 * This file contains a sandbox for
 * testing the SGL library
 */
#include "core_types.h"
#include "hal/malloc_ansi.h"

int main()
{
	MallocAnsi * gMalloc = new MallocAnsi;
	void * dst = gMalloc->alloc(4096, 16);
	dst = gMalloc->realloc(dst, 8192, 32);
	dst = gMalloc->realloc(dst, 1024, 32);
	dst = gMalloc->realloc(dst, 1024, 8);
	gMalloc->free(dst);

	return 0;
}