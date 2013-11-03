
#include "gtest/gtest.h"

#include "../ckpool.h"

static void CreateAndAllocPool(cinek_mempool* pool, size_t numBlocks, size_t blockSize, 
        void* blocks[])
{
    *pool = cinek_mempool_create(NULL, numBlocks, blockSize);
    ASSERT_NE(nullptr, *pool);

    size_t numAlloc = 0;
    for (numAlloc = 0; numAlloc < numBlocks; ++numAlloc)
    {
        void* block = cinek_mempool_alloc(*pool);
        ASSERT_NE(nullptr, block);
        if (blocks != nullptr)
        {       
            blocks[numAlloc] = block;
        }
    }
}

TEST(PoolBasic, CreateDestroy) {
    cinek_mempool pool = cinek_mempool_create(NULL, 1, 1);
    EXPECT_TRUE(NULL != pool);
    cinek_mempool_destroy(pool);
}

TEST(PoolAlloc, AllocDeallocRealloc) {
    cinek_mempool pool = cinek_mempool_create(NULL, 1, 16);
    EXPECT_TRUE(NULL != pool);
    void* block = cinek_mempool_alloc(pool);
    EXPECT_NE(nullptr, block);
    cinek_mempool_dealloc(pool, block);
    block = cinek_mempool_alloc(pool);
    EXPECT_NE(nullptr, block);
    cinek_mempool_dealloc(pool, block);
    cinek_mempool_destroy(pool);
}

TEST(PoolAlloc, AllocNoBlocksLeft) {
    cinek_mempool pool = cinek_mempool_create(NULL, 1, 16);
    EXPECT_TRUE(NULL != pool);
    void* block = cinek_mempool_alloc(pool);
    EXPECT_NE(nullptr, block);
    EXPECT_EQ(nullptr, cinek_mempool_alloc(pool));
    cinek_mempool_dealloc(pool, block);
    cinek_mempool_destroy(pool);   
}

TEST(PoolAlloc, AllocMultiNoBlocksLeft) {
    const size_t MAX_BLOCKS = 40;
    const size_t BLOCK_SIZE = 24;
    void* blocks[MAX_BLOCKS];
    cinek_mempool pool;
    CreateAndAllocPool(&pool, MAX_BLOCKS, BLOCK_SIZE, blocks);

    //  twp tries just to be safe.
    ASSERT_EQ(nullptr, cinek_mempool_alloc(pool));
    ASSERT_EQ(nullptr, cinek_mempool_alloc(pool));

    cinek_mempool_destroy(pool);      
}

TEST(PoolStressAlloc, Alloc1024x1024) {
    const size_t MAX_BLOCKS = 1024;
    const size_t BLOCK_SIZE = 1024;
    cinek_mempool pool;
    CreateAndAllocPool(&pool, MAX_BLOCKS, BLOCK_SIZE, nullptr);
    cinek_mempool_destroy(pool);      
}

TEST(PoolStressAlloc, Alloc4096x256) {
    const size_t MAX_BLOCKS = 4096;
    const size_t BLOCK_SIZE = 256;
    cinek_mempool pool;
    CreateAndAllocPool(&pool, MAX_BLOCKS, BLOCK_SIZE, nullptr);
    cinek_mempool_destroy(pool);      
}

TEST(PoolStressAlloc, Alloc8192x128) {
    const size_t MAX_BLOCKS = 8192;
    const size_t BLOCK_SIZE = 128;
    cinek_mempool pool;
    CreateAndAllocPool(&pool, MAX_BLOCKS, BLOCK_SIZE, nullptr);
    cinek_mempool_destroy(pool);      
}

/*
TEST(PoolIntegrity, AllocVerifySizes) {
    const size_t MAX_BLOCKS = 10;
    const size_t BLOCK_SIZE = 24;
    uint8_t* blocks[MAX_BLOCKS];
    cinek_mempool pool = cinek_mempool_create(MAX_BLOCKS, BLOCK_SIZE, 0);
    ASSERT_TRUE(NULL != pool);
    size_t numAlloc = 0;
    uint8_t* lastBlock = NULL;
    for (numAlloc = 0; numAlloc < MAX_BLOCKS; ++numAlloc)
    {
        blocks[numAlloc] = (uint8_t*)cinek_mempool_alloc(pool);
        ASSERT_NE(nullptr, blocks[numAlloc]);
        if (lastBlock != NULL)
        {
            ASSERT_EQ(BLOCK_SIZE, blocks[numAlloc] - lastBlock);
        }
        lastBlock = blocks[numAlloc];
    }
    cinek_mempool_destroy(pool);      
}
*/

TEST(PoolDeallocAll, DeallocAllNoAlloc) {
    const size_t MAX_BLOCKS = 40;
    const size_t BLOCK_SIZE = 24;
    void* blocks[MAX_BLOCKS];
    cinek_mempool pool;
    CreateAndAllocPool(&pool, MAX_BLOCKS, BLOCK_SIZE, blocks);
    cinek_mempool_dealloc_all(pool);
    cinek_mempool_destroy(pool);         
}

TEST(PoolDeallocAll, DeallocAndReallocAll) {
    const size_t MAX_BLOCKS = 40;
    const size_t BLOCK_SIZE = 24;
    void* blocks[MAX_BLOCKS];
    cinek_mempool pool;
    CreateAndAllocPool(&pool, MAX_BLOCKS, BLOCK_SIZE, blocks);
    //  verify we have a full pool
    ASSERT_EQ(nullptr, cinek_mempool_alloc(pool));
    //  deallocate all.
    cinek_mempool_dealloc_all(pool);
    //  reallocate
    size_t numAlloc = 0;
    while (numAlloc < MAX_BLOCKS)
    {
        blocks[numAlloc] = cinek_mempool_alloc(pool);
        EXPECT_NE(nullptr, blocks[numAlloc]);
        ++numAlloc;
    }
    //  verify we're full.
    EXPECT_EQ(nullptr, cinek_mempool_alloc(pool));
    cinek_mempool_destroy(pool);         
}

TEST(PoolIntegrity, DeallocAndRealloc) {
    const size_t MAX_BLOCKS = 40;
    const size_t BLOCK_SIZE = 24;
    void* blocks[MAX_BLOCKS];
    cinek_mempool pool;
    CreateAndAllocPool(&pool, MAX_BLOCKS, BLOCK_SIZE, blocks);
    size_t numAlloc = MAX_BLOCKS;
    for (; numAlloc >= MAX_BLOCKS/2; --numAlloc)
    {
        cinek_mempool_dealloc(pool, blocks[numAlloc-1]);
        blocks[numAlloc-1] = nullptr;
    }
    for (; numAlloc < MAX_BLOCKS; ++numAlloc)
    {
        blocks[numAlloc] = cinek_mempool_alloc(pool);
        ASSERT_NE(nullptr, blocks[numAlloc]);
    }
    cinek_mempool_destroy(pool);      
}

TEST(PoolIntegrity, DeallocRandomAndRealloc) {
    const size_t MAX_BLOCKS = 117;   /* non 8-bit aligned for worst case scenario */
    const size_t BLOCK_SIZE = 26;
    void* blocks[MAX_BLOCKS];
    cinek_mempool pool;
    CreateAndAllocPool(&pool, MAX_BLOCKS, BLOCK_SIZE, blocks);
    size_t numAlloc = MAX_BLOCKS;
    for (size_t i = 0; i < numAlloc; ++i)
    {
        if (!(rand() % 3))
        {
            --numAlloc;
            cinek_mempool_dealloc(pool, blocks[i]);
            blocks[i] = nullptr;
        }
    }
    while (numAlloc < MAX_BLOCKS)
    {
        EXPECT_NE(nullptr, cinek_mempool_alloc(pool));
        ++numAlloc;
    }
    EXPECT_EQ(nullptr, cinek_mempool_alloc(pool));

    cinek_mempool_destroy(pool);
}
