#pragma once

#include "malloc_pool.h"
#include "../containers/array.h"

/**
 * Similar to MallocPool, except buckets
 * of pools of different sizes are managed
 * to accomodate for different allocation
 * requests.
 * 
 * This is the ultimate pool-based allocator.
 */
class MallocBinned : public MallocBase
{
public:
	/**
	 * Configuration struct
	 */
	struct BucketInfo
	{
		/// Pool fixed size
		sizet poolSize;

		/// Bucket range @{
		sizet allocUnitMinSize;
		sizet allocUnitMaxSize;
		sizet allocUnitIncrement;
		/// @}
	};

protected:
	/// Buckets descriptor
	const BucketInfo bucketInfo;

	/// Number of buckets
	const uint32 numBuckets;

	/// Pool alignment
	const sizet poolAlign;

	/// Array of MallocPooled allocators
	Array<MallocPooled*> buckets;

public:
	/**
	 * Default constructor.
	 * 
	 * @see MemoryPool::MemoryPool
	 */
	MallocBinned(const BucketInfo & inInfo = {1ull << 16ull/* 1 MB */, 1ull << 3ull, 1ull << 18ull, 1ull << 3ull}, sizet inPoolAlign = DEFAULT_ALIGNMENT);

protected:
	/**
	 * Compute bucket index from requested
	 * size in Bytes. size cannot be zero.
	 * 
	 * @param size bucket min size
	 * @return bucket index (even if greater
	 * 	than max bucket size)
	 */
	uint32 getBucketIdx(sizet size) const
	{
		CHECK(size != 0);

		// Get power of two index
		return PlatformMath::log2(size - 1) / PlatformMath::log2(bucketInfo.allocUnitIncrement);
	}

public:
	//////////////////////////////////////////////////
	// MallocBase interface
	//////////////////////////////////////////////////
	
	virtual void * alloc(sizet size, sizet alignment = DEFAULT_ALIGNMENT) override;
	virtual void * realloc(void * orig, sizet size, sizet alignment = DEFAULT_ALIGNMENT) override;
	virtual void free(void * orig) override;
};