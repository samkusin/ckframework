/**
 * \file    allocator.cpp
 *
 *
 * \note    Created by Samir Sinha on 1/6/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#include "allocator.hpp"
#include <string.h>
#include <stdlib.h>

namespace cinek {

/*****************************************************************************/
static void* DefaultAlloc(void* ctx, size_t numBytes)
{
    return malloc(numBytes);
}

static void* DefaultAllocAlign(void* ctx, size_t numBytes, size_t align)
{
    void* ptr;
#if defined(CK_TARGET_WINDOWS)
    ptr = _aligned_malloc(numBytes, align);
#else
    if (posix_memalign(&ptr, align, numBytes) != 0)
        return nullptr;
#endif

    return ptr;
}

static void DefaultFree(void* ctx, void* ptr)
{
    free(ptr);
}

static void DefaultFreeAlign(void* ctx, void* ptr)
{
#if defined(CK_TARGET_WINDOWS)
    _aligned_free(ptr);
#else
    free(ptr);
#endif
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
g_cinek_memoryProvider[16] =
{
    {
        {
            &DefaultAlloc,
            &DefaultAllocAlign,
            &DefaultFree,
            &DefaultFreeAlign,
            &DefaultRealloc,
            NULL
        }
    },
};

/*****************************************************************************/

void cinek_alloc_set_callbacks
(
    int heap,
    const cinek_memory_callbacks* callbacks
)
{
    if (callbacks == NULL)
    {
        g_cinek_memoryProvider[heap].cbs.alloc = &DefaultAlloc;
        g_cinek_memoryProvider[heap].cbs.alloc_aligned = &DefaultAllocAlign;
        g_cinek_memoryProvider[heap].cbs.free = &DefaultFree;
        g_cinek_memoryProvider[heap].cbs.free_aligned = &DefaultFreeAlign;
        g_cinek_memoryProvider[heap].cbs.realloc = &DefaultRealloc;
        g_cinek_memoryProvider[heap].cbs.context = NULL;
        return;
    }
    memcpy(&g_cinek_memoryProvider[heap].cbs, callbacks,
           sizeof(g_cinek_memoryProvider[heap].cbs));
}


void cinek_get_alloc_callbacks(int heap, cinek_memory_callbacks* callbacks)
{
    memcpy(callbacks, &g_cinek_memoryProvider[heap].cbs,
           sizeof(g_cinek_memoryProvider[heap].cbs));
}

void* cinek_alloc(int heap, size_t sz)
{
    return (*g_cinek_memoryProvider[heap].cbs.alloc)
            (
                g_cinek_memoryProvider[heap].cbs.context,
                sz
            );
}

void* cinek_alloc_aligned(int heap, size_t sz, size_t align)
{
    return (*g_cinek_memoryProvider[heap].cbs.alloc_aligned)
            (
                g_cinek_memoryProvider[heap].cbs.context,
                sz,
                align
            );
}

void* cinek_realloc(int heap, void* ptr, size_t sz)
{
    return (*g_cinek_memoryProvider[heap].cbs.realloc)
            (
                g_cinek_memoryProvider[heap].cbs.context,
                ptr,
                sz
            );
}


void cinek_free(int heap, void* ptr)
{
    if (ptr == NULL)
        return;
    (*g_cinek_memoryProvider[heap].cbs.free)
    (
        g_cinek_memoryProvider[heap].cbs.context,
        ptr
    );
}

void cinek_free_aligned(int heap, void* ptr)
{
    if (ptr == NULL)
        return;
    (*g_cinek_memoryProvider[heap].cbs.free_aligned)
        (
            g_cinek_memoryProvider[heap].cbs.context,
            ptr
            );
}


} /* namespace cinek */

