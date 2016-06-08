/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2013 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @file    cinek/memory.h
 * @author  Samir Sinha
 * @date    1/6/2013
 * @brief   std library allocator using custom allocation hooks
 * @copyright Cinekine
 */

#ifndef CINEK_MEMORY_H
#define CINEK_MEMORY_H

#include "ckdefs.h"

#include <stddef.h>

/** Callbacks used for memory allocation/deallocation */
struct cinek_memory_callbacks
{
    /** Invoked when a subsystem allocates memory. */
    void*   (*alloc)(void* ctx, size_t numBytes);
    /** Invoked when a subsystem allocates aligned memory. */
    void*   (*alloc_aligned)(void* ctx, size_t numBytes, size_t align);
    /** Invoked when a subsystem frees memory. */
    void    (*free)(void* ctx, void* ptr);
    /** Invoked when a subsystem frees aligned memory. */
    void    (*free_aligned)(void* ctx, void* ptr);
    /** Invoked when a subsystem reallocates memory given a block of memory
     * previously allocated by alloc. */
    void*   (*realloc)(void* ctx, void* ptr, size_t numBytes);
    /** An application specific context pointer passed to the callback function
     * pointers in callbacks. */
    void*   context;
};

#ifdef __cplusplus
extern "C" {
#endif

/** Specify a custom memory allocator with an application specific context for
 *  CineK systems.
 *
 *  All applications must call this function with its own set of memory callbacks
 *  before using any CineK systems.
 *
 *  @param  heap            The heap index mapped to the specified callback set
 *  @param  callbacks       The custom allocator defined by a series of application
 *                          defined callbacks.
 */
void cinek_alloc_set_callbacks(
        int heap,
        const struct cinek_memory_callbacks* callbacks
    );

/** Returns the allocation callbacks and context supplied by
 *  cinek_alloc_set_callbacks.
 *
 *  @param  heap      Retrieves callbacks for the specified heap index
 *  @param  callbacks Pointer to a struct to hold the memory allocation
 *                    callbacks.
 */
void cinek_get_alloc_callbacks(int heap, struct cinek_memory_callbacks* callbacks);

/**
 *  Allocate from the specified heap
 *  @param  heap    The heap to allocate from
 *  @param  sz      The amount to allocate
 *  @return A pointer to the allocated block of memory (or NULL)
 */
void* cinek_alloc(int heap, size_t sz);

/**
 *  Allocate an aligned block from the specified heap
 *  @param  heap    The heap to allocate from
 *  @param  sz      The amount to allocate
 *  @param  align   The alignment of the allocated block
 *  @return A pointer to the allocated block of memory (or NULL)
 */
void* cinek_alloc_aligned(int heap, size_t sz, size_t align);

/**
 *  Allocate from the specified heap using an existing allocate block.  Used
 *  for efficient management of a heap when resizing a block.
 *  @param  heap    The heap to allocate from
 *  @param  ptr     The allocated block to resize
 *  @param  sz      The amount to allocate
 *  @return A pointer to the allocated block of memory (or NULL)
 */
void* cinek_realloc(int heap, void* ptr, size_t sz);

/**
 *  Frees a block allocated via cinek_alloc
 *  @param  heap    The heap to free from
 *  @param  ptr     The allocated block to free
 */
void cinek_free(int heap, void* ptr);

/**
*  Frees a block allocated via cinek_alloc_aligned
*  @param  heap    The heap to free from
*  @param  ptr     The allocated block to free
*/
void cinek_free_aligned(int heap, void* ptr);

#ifdef __cplusplus
}
#endif

#endif
