/** 
 * \file    ckpool.h
 * An API for fixed sized block pool allocation.
 * 
 * \note    Created by Samir Sinha on 4/10/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_CKPOOL_H
#define CINEK_CKPOOL_H

#include "cktypes.h"
#include "ckalloc.h"

#ifdef __cplusplus
extern "C" {
#endif

/** An opaque memory pool handle. */
typedef struct cinek_mempool_t {} *cinek_mempool;

/**
 * Creates a memory pool with fixed sized blocks.
 * @param  callbacks      Memory callbacks used for allocation of heap memory.
 *                        If NULL, then memory is allocated from the default
 *                        heap.
 * @param  numBlocks      Maximum number of blocks to reserve for allocation.
 * @param  blockSize      Byte size of an individual block.
 * @return           	  A memory pool handle.
 */
cinek_mempool cinek_mempool_create(const cinek_memory_callbacks* callbacks,
						size_t numBlocks, 
                        size_t blockSize);

/** 
 * Destroys a memory pool, releasing memory back to the system allocator.
 * @param  pool The memory pool to destroy.
 */
void cinek_mempool_destroy(cinek_mempool pool);

/**
 * Deallocates all blocks at once.  All allocated blocks are freed, but the
 * pool's system memory remains allocated.
 * @param pool Pool to reset.
 */
void cinek_mempool_dealloc_all(cinek_mempool pool);

/**
 * Allocates a block from the pool.
 * @param  pool The memory pool.
 * @return      A pointer to the returned block.
 */
void* cinek_mempool_alloc(cinek_mempool pool);

/**
 * Deallocates a block, returning it to the pool.
 * @param  pool The memory pool. 
 * @param  block The block to deallocate.
 */
void cinek_mempool_dealloc(cinek_mempool pool, void* block);

#ifdef __cplusplus
}
#endif

#endif