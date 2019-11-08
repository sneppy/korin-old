#include <hal/malloc_binned.h>

//////////////////////////////////////////////////
// MallocBinned
//////////////////////////////////////////////////

MallocBinned::MallocBinned(const BucketInfo & inInfo, sizet inPoolAlign)
	: numBuckets{(uint32)((PlatformMath::log2(inInfo.allocUnitMaxSize) - PlatformMath::log2(inInfo.allocUnitMinSize)) / PlatformMath::log2(inInfo.allocUnitIncrement))}
	, bucketInfo{inInfo}
	, poolAlign{PlatformMath::max(inPoolAlign, MIN_ALIGNMENT)}
	, buckets{numBuckets}
{
	CHECK(numBuckets != 0);

	for (
		uint32 i = 0, blockSize = bucketInfo.allocUnitMinSize, numBlocks = bucketInfo.poolSize / blockSize;
		i < numBuckets;
		++i, numBlocks = bucketInfo.poolSize / (blockSize *= bucketInfo.allocUnitIncrement)
	)
	{
		// Create bucket
		buckets.add(new MallocPooled{numBlocks, blockSize, poolAlign, 1});
	}
}

void * MallocBinned::alloc(sizet size, sizet alignment)
{
	// Find bucket
	const uint32 bucketIdx = getBucketIdx(size);
	if (bucketIdx < numBuckets)
	{
		MallocPooled * bucket = buckets[bucketIdx];
		return bucket->alloc(size, alignment);
	}
	else
	{
		CHECKF(false, "requested size exceed max bucket alloc unit size, backup allocator will be used (max bucket alloc unit size is 0x%llx, requested size is 0x%llx)", bucketInfo.allocUnitMaxSize, size)

		// TODO: Use backup allocator
		return nullptr;
	}
}

void * MallocBinned::realloc(void * orig, sizet size, sizet alignment)
{
	// TODO
	return nullptr;
}

void MallocBinned::free(void * orig)
{
	// The only way is to ask each bucket
	// whether it allocated the block
	for (uint32 i = 0; i < numBuckets; ++i)
	{
		MallocPooled * bucket = buckets[i];
		if (bucket->hasBlock(orig))
		{
			// Free and return
			bucket->free(orig);
			return;
		}
	}
}