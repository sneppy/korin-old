#include "hal/memory_base.h"
#include "hal/malloc_ansi.h"

// TODO: Replace with initialization function
static MallocAnsi gMallocAnsi;
MallocBase * gMalloc = &gMallocAnsi;