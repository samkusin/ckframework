/**
 * \file    ckpool.c
 *
 * \note    Created by Samir Sinha on 4/10/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "ckpool.h"
#include "ckalloc.h"
#include "ckdebug.h"

#include <string.h>

/* Must be power of 2, indicates number of bits in a state word. This should match
 * the blockStateBits pointer type (i.e. uint8_t = 8, etc.)
 */
#define CK_POOL_STATE_BIT_COUNT     8
typedef uint8_t cinek_mempool_state_word;

/* Internal data */
struct cinek_mempool_data 
{
    uint8_t* memory;            /*  pool memory allocated from the system. */
    cinek_memory_callbacks    memcbs;
    /*  each bit represents a block, if off then it's been allocated. */
    cinek_mempool_state_word* blockStateBits;    
    size_t numBlocks;
    size_t blockSize;
};


cinek_mempool cinek_mempool_create(const cinek_memory_callbacks* callbacks,
                        size_t numBlocks, 
                        size_t blockSize)
{
    struct cinek_mempool_data* pool;
    size_t stateByteCount;
    size_t poolRawMemorySize;

    if (!blockSize || !numBlocks)
        return NULL;

    stateByteCount = (numBlocks+CK_POOL_STATE_BIT_COUNT-1)/CK_POOL_STATE_BIT_COUNT;
    poolRawMemorySize = sizeof(struct cinek_mempool_data) +
                                blockSize * numBlocks +
                                stateByteCount; 
    /* allocate all memory required for the pool in a single alloc. */
    if (callbacks) {
        pool = (struct cinek_mempool_data*)(*callbacks->alloc)(callbacks->context, poolRawMemorySize);
    }
    else {
        pool = (struct cinek_mempool_data*)cinek_alloc_heap_alloc(poolRawMemorySize);
    }
    if (pool == NULL)
        return NULL;

    if (callbacks) {
        memcpy(&pool->memcbs, callbacks, sizeof(*callbacks));
    }
    else {
        memset(&pool->memcbs, 0, sizeof(pool->memcbs));
    }
    pool->memory = (uint8_t*)pool + sizeof(struct cinek_mempool_data);
    pool->blockStateBits = (cinek_mempool_state_word *)(pool->memory + blockSize * numBlocks);
    pool->numBlocks = numBlocks;
    pool->blockSize = blockSize;
    cinek_mempool_dealloc_all((cinek_mempool)pool);
    
    return (cinek_mempool)pool;
}


void cinek_mempool_destroy(cinek_mempool pool)
{
    struct cinek_mempool_data* poolData;

    if (pool == NULL)
        return;

    poolData = (struct cinek_mempool_data* )pool;
    if (poolData->memcbs.free) {
        (*poolData->memcbs.free)(poolData->memcbs.context, poolData);
    }
    else {
        cinek_alloc_heap_dealloc(poolData);
    }
}


void cinek_mempool_dealloc_all(cinek_mempool pool)
{
    struct cinek_mempool_data* poolData;
    cinek_mempool_state_word* stateBits;
    size_t numWords, numBitsRemainder;
    if (pool == NULL)
        return;

    poolData = (struct cinek_mempool_data* )pool;
    stateBits = poolData->blockStateBits;
    numWords = poolData->numBlocks/CK_POOL_STATE_BIT_COUNT;
    numBitsRemainder = poolData->numBlocks%CK_POOL_STATE_BIT_COUNT;
    
    /* compress free/alloc list into an array of bits.  used for faster
       lookup of free/available blocks in the pool.
    */
    if (numWords)
    {
        memset(stateBits, (cinek_mempool_state_word)(-1), numWords * sizeof(cinek_mempool_state_word));
        stateBits += numWords;
    }
    *stateBits = 0;
    while (numBitsRemainder)
    {
        --numBitsRemainder;   
        *stateBits |= (1 << numBitsRemainder);
    }
}


void* cinek_mempool_alloc(cinek_mempool pool)
{
    struct cinek_mempool_data* poolData;
    size_t stateWordCount, stateWordIndex;

    if (pool == NULL)
        return NULL;

    /* Efficient for high block count pools.  resort to bit iteration once we've found
       a non-zero byte.  As more blocks are allocated, this approach should speed lookup
       of free blocks.
    */
    poolData = (struct cinek_mempool_data* )pool;
    stateWordCount = (poolData->numBlocks+7)/8;
    for (stateWordIndex = 0; stateWordIndex < stateWordCount; ++stateWordIndex)
    {
        cinek_mempool_state_word stateBits;
        stateBits = poolData->blockStateBits[stateWordIndex];
        /* group of 8 blocks, at least one is free. */
        if (stateBits)
        {
            uint8_t* memoryBlock;
            uint8_t bitIndex;

            memoryBlock = poolData->memory + (stateWordIndex*CK_POOL_STATE_BIT_COUNT * poolData->blockSize);
            for (bitIndex = 0; bitIndex < CK_POOL_STATE_BIT_COUNT; ++bitIndex)
            {
                if (stateBits & (1<<bitIndex))
                {
                    poolData->blockStateBits[stateWordIndex] = stateBits & ~(1<<bitIndex);
                    memoryBlock += (poolData->blockSize * bitIndex);
                    return memoryBlock;
                }
            }
        }
    }
    /* no free blocks found above. */
    return NULL;
}


void cinek_mempool_dealloc(cinek_mempool pool, void* block)
{
    struct cinek_mempool_data* poolData;
    uintptr_t blockOffsetFromBase;
    size_t blockIndex;
    size_t stateWordBitIndex;
    cinek_mempool_state_word* stateBits;

    if (pool == NULL || block == NULL)
        return;
    poolData = (struct cinek_mempool_data* )pool;
    blockOffsetFromBase = (uintptr_t)((uint8_t*)block - poolData->memory);
    if (blockOffsetFromBase % poolData->blockSize)
    {
        CK_ASSERT(0);
        return;
    }
    blockIndex = blockOffsetFromBase / poolData->blockSize;
    if (blockIndex >= poolData->numBlocks)
    {
        CK_ASSERT(0);
        return;
    }
    stateBits = poolData->blockStateBits + blockIndex / CK_POOL_STATE_BIT_COUNT;
    stateWordBitIndex = blockIndex % CK_POOL_STATE_BIT_COUNT;
    *stateBits = *stateBits | ( 1<<(blockIndex % CK_POOL_STATE_BIT_COUNT) );
}
