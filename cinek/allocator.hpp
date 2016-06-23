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
 * @file    cinek/allocator.hpp
 * @author  Samir Sinha
 * @date    1/6/2013
 * @brief   std library allocator using custom allocation hooks
 * @copyright Cinekine
 */

#ifndef CINEK_ALLOC_HPP
#define CINEK_ALLOC_HPP

#include "ckdefs.h"
#include "memory.h"
#include "std_allocator.hpp"

#include <cstdint>
#include <memory>

namespace cinek {

/**
 * @class Allocator
 * @brief Wraps a cinek_alloc based memory allocator for use in
 * compliant C++ objects.
 *
 */
class Allocator
{
public:
	Allocator(): _heap(0) {}
    Allocator(Allocator&& other) :
        _heap(other._heap)
    {
    }
    Allocator& operator=(Allocator&& other)
    {
        _heap = other._heap;
        other._heap = 0;
        return *this;
    }
    Allocator(const Allocator& other) :
        _heap(other._heap) {}
    Allocator& operator=(const Allocator& other) {
        _heap = other._heap;
        return *this;
    }
	/**
	 * Constructor.
	 * @param heap           Memory allocation heap
	 * @param context 		 Context passed to the callbacks supplied via
	 *                   	 allocCallbacks.
	 */
	explicit Allocator(int heap)
		: _heap(heap) {}
	/**
	 * Allocates a block of memory of the supplied size.
	 * @param  size Size of the memory block to allocate.
	 * @return A pointer to the allocated block or nullptr.
	 */
	void* alloc(size_t size) {
		return cinek_alloc(_heap, size);
	}
	/**
	 * Allocates an aligned block of memory of the supplied size.
	 * @param  size Size of the memory block to allocate.
     * @param  align The alignment of the returned memory block.
	 * @return A pointer to the allocated block or nullptr.
	 */
    void* allocAligned(size_t size, size_t align) {
        return cinek_alloc_aligned(_heap, size, align);
    }
    /**
	 * Attempts to grow or reduce an existing buffer, or allocates a new buffer
     * if necessary.
     * @param  p  Pointer to the memory block to resize.  If resizing in place
     *            is not possible, then will attempt to allocate a new block,
     *            and in-effect frees the existing block.
	 * @param  sz Size of the memory block to allocate.
	 * @return    A pointer to the allocated block or nullptr.
     */
    void* realloc(void* p, size_t sz) {
        return cinek_realloc(_heap, p, sz);
    }
	/**
	 * Allocates and constucts instance of T.
     * @param  args Initialization arguments to the constructor for T.
	 * @return      A pointer to T.
	 */
	template<typename T, class... Args> T* newItem(Args&&... args) {
        T* p = reinterpret_cast<T*>(alloc(sizeof(T)));
        ::new(p) T(std::forward<Args>(args)...);
        return p;
	}
    /**
     * Destroys an instance of T allocated via newItem.
     * @param item The item to delete.
     */
    template<typename T> void deleteItem(T* item) {
        item->~T();
        free(item);
    }
	/**
	 * Allocates an array of T.
	 * @param  count Number of items in array.
	 * @return       A pointer to the array
	 */
	template<typename T> T* allocItems(size_t count) {
        T* p = reinterpret_cast<T*>(alloc(sizeof(T)*count));
		return p;
	}
	/**
	 * Frees memory allocated by alloc.
	 * @param ptr Pointer to the memory block to free.
	 */
	void free(void* ptr) {
        cinek_free(_heap, ptr);
	}

    /**
    * Frees aligned memory allocated by allocAlign.
    * @param ptr Pointer to the memory block to free
    */
    void freeAligned(void* ptr) {
        cinek_free_aligned(_heap, ptr);
    }

private:
    friend bool operator==(const Allocator& lha, const Allocator& rha);
    friend bool operator!=(const Allocator& lha, const Allocator& rha);
    int _heap;
};
/** @cond */
inline bool operator==(const Allocator& lha, const Allocator& rha)
{
    return lha._heap == rha._heap;
}
inline bool operator!=(const Allocator& lha, const Allocator& rha)
{
    return lha._heap != rha._heap;
}
/** @endcond */

}	// namespace cinek


#endif