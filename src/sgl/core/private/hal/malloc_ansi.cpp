#include "hal/malloc_ansi.h"
#include "hal/platform_math.h"

#if PLATFORM_UNIX
	#include <malloc.h>
#endif

void * MallocAnsi::alloc(sizet size, sizet alignment)
{
	// Compute actual alignment
	alignment = PlatformMath::max(alignment, MIN_ALIGNMENT);

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
	// Compute actual alignment
	alignment = PlatformMath::max(alignment, MIN_ALIGNMENT);

	void * out;

#if PLATFORM_UNIX
	if (orig && size > 0)
	{
		sizet usableSize = malloc_usable_size(orig);
		if (UNLIKELY(::posix_memalign(&out, alignment, size) != 0))
			return nullptr;
		
		if (LIKELY(usableSize))
			Memory::memcpy(out, orig, PlatformMath::min(size, usableSize));
		
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