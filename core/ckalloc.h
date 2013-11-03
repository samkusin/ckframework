/**
 * \file    ckalloc.h
 *
 *
 * \note    Created by Samir Sinha on 1/5/13.
 *          Copyright (c) 2013 Cinekine. All rights reserved.
 */

#ifndef CINEK_CKALLOC_H
#define CINEK_CKALLOC_H

/* PORTABILITY NOTE: Not including cktypes.h if possible to minimize dependencies. 
   Try to mimic ckdefs.h if we need preprocessor ifdef checks for including one 
   header vs. another.
*/
#include <stddef.h>
#include <stdint.h>


#ifdef __cplusplus
extern "C" {
#endif

/** Callbacks used for memory allocation/deallocation */
typedef struct cinek_memory_callbacks_t
{
    /** Invoked when a subsystem allocates memory. */
    void*   (*alloc)(void* ctx, size_t numBytes);
    /** Invoked when a subsystem frees memory. */
    void    (*free)(void* ctx, void* ptr);
    /** Invoked when a subsystem reallocates memory given a block of memory 
     * previously allocated by alloc. */
    void*   (*realloc)(void* ctx, void* ptr, size_t numBytes);
    /** An application specific context pointer passed to the callback function
     * pointers in callbacks. */
    void*   context;
}
cinek_memory_callbacks;

/** Specify a custom memory allocator with an application specific context for 
 *  CineK systems.
 *
 *  All applications must call this function with its own set of memory callbacks
 *  before using any CineK systems.
 *  
 *  @param  callbacks       The custom allocator defined by a series of application
 *                          defined callbacks.
 */
void cinek_alloc_set_callbacks(
        const cinek_memory_callbacks* callbacks
    );

/** Returns the allocation callbacks and context supplied by 
 *  cinek_alloc_set_callbacks.
 *
 *  @param  callbacks Pointer to a struct to hold the memory allocation 
 *                    callbacks.
 */
void cinek_get_alloc_callbacks(cinek_memory_callbacks* callbacks);

/** Allocates memory (redirects from the callback passed to 
 *  cinek_alloc_set_callbacks.)
 *
 *  @param  sz              The amount (in bytes) to allocate.
 *  @return                 Pointer to the allocated memory.
 */
void* cinek_alloc_heap_alloc(size_t sz);

/** Frees a block of memory allocated via cinek_alloc_heap_alloc, redirecting
 *  the request to the application callback (see cinek_alloc_set_callbacks.)
 * 
 *  @param ptr              The memory block to deallocate. 
 */
void cinek_alloc_heap_dealloc(void* ptr);

#ifdef __cplusplus
}
#endif

#endif
