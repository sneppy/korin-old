#pragma once

#include "platform_memory.h"

/**
 * Standard C aligned allocator
 */
class MallocAnsi : public MallocBase
{
public:
	//////////////////////////////////////////////////
	// MallocBase interface
	//////////////////////////////////////////////////
	
	virtual void * alloc(sizet size, sizet alignment = DEFAULT_ALIGNMENT) override;
	virtual void * realloc(void * orig, sizet size, sizet alignment = DEFAULT_ALIGNMENT) override;
	virtual void free(void * orig) override;
};