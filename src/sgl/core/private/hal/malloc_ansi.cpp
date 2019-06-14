#include "hal/malloc_ansi.h"

#if PLATFORM_UNIX
	#include <malloc.h>
#endif

void * MallocAnsi::alloc(sizet size, sizet alignment)
{
	// TODO: Compute actual alignment

	void * out;

#if PLATFORM_UNIX
	if (UNLIKELY(::posix_memalign(&out, alignment, size) != 0))
		return nullptr;
#else
	// TODO: Windows aligned allocation
#endif

	return out;
}

void * MallocAnsi::realloc(void * orig, sizet size, sizet alignment)
{
	// TODO: compute actual alignment

	void * out;

#if PLATFORM_UNIX
	if (orig && size > 0)
	{
		sizet usable = malloc_usable_size(orig);
		if (UNLIKELY(::posix_memalign(&out, alignment, size) != 0))
			return nullptr;
		else if (LIKELY(usable))
			// TODO: limit by usable size
			Memory::memcpy(out, orig, size);
		
		// Free original block
		::free(orig);
	}
	else if (!orig)
	{
		if (UNLIKELY(::posix_memalign(&out, alignment, size) != 0))
			return nullptr;
	}
	else
	{
		// Basically a free
		::free(orig);
		out = nullptr;
	}
#else
	// TODO: Windows implementation
#endif

	return out;
}

void MallocAnsi::free(void * orig)
{
#if PLATFORM_UNIX
	::free(orig);
#else
	// TODO: Windows implementation
#endif
}