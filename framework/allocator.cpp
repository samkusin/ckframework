/**
 * \file    ckalloc.c
 *
 *
 * \note    Created by Samir Sinha on 1/6/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include <cinek/framework/allocator.hpp>
#include <string.h>
#include <stdlib.h>

namespace cinekine {

/*****************************************************************************/
static void* DefaultAlloc(void* ctx, size_t numBytes)
{
    return malloc(numBytes);
}

static void DefaultFree(void* ctx, void* ptr)
{
    free(ptr);
}

static void* DefaultRealloc(void* ctx, void* ptr, size_t numBytes)
{
    return realloc(ctx, numBytes);
}

/*  the global memory provider */
struct
{
    cinek_memory_callbacks cbs;
}
g_cinek_memoryProvider = 
{
    {
       &DefaultAlloc,
       &DefaultFree,
       &DefaultRealloc, 
       NULL    
    }
};

/*****************************************************************************/

void cinek_alloc_set_callbacks(
        const cinek_memory_callbacks* callbacks
    )
{
    if (callbacks == NULL)
    {
        g_cinek_memoryProvider.cbs.alloc = &DefaultAlloc;
        g_cinek_memoryProvider.cbs.free = &DefaultFree;
        g_cinek_memoryProvider.cbs.realloc = &DefaultRealloc;
        g_cinek_memoryProvider.cbs.context = NULL;
        return;
    }
    memcpy(&g_cinek_memoryProvider.cbs, callbacks, sizeof(g_cinek_memoryProvider.cbs));
}


void cinek_get_alloc_callbacks(cinek_memory_callbacks* callbacks)
{
    memcpy(callbacks, &g_cinek_memoryProvider.cbs, sizeof(g_cinek_memoryProvider.cbs));
}

void* cinek_alloc_heap_alloc(size_t sz)
{
    return (*g_cinek_memoryProvider.cbs.alloc)(g_cinek_memoryProvider.cbs.context, sz);
}


void cinek_alloc_heap_dealloc(void* ptr)
{
    if (ptr == NULL)
        return;
    (*g_cinek_memoryProvider.cbs.free)(g_cinek_memoryProvider.cbs.context, ptr);
}

} /* namespace cinekine */

