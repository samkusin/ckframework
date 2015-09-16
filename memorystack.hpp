/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Cinekine Media
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
 * @file    cinek/memorystack.hpp
 * @author  Samir Sinha
 * @date    4/14/2013
 * @brief   Object allocation within a pooled heap
 * @copyright Cinekine
 */

#ifndef CINEK_MEMORY_STACK_HPP
#define CINEK_MEMORY_STACK_HPP

#include "allocator.hpp"

namespace cinek {

    /**
     * @class MemoryStack
     * @brief Implements a simple stack-based memory allocation pool.
     *
     * The MemoryStack grabs memory from the supplied allocator in chunks.  The
     * stack allocator grabs memory sequentially from a chunk, until there is
     * no room left.
     *
     * When the pool runs out of bytes, it will attempt to allocate a new chunk
     * of equal size to the preceding chunk from the supplied allocator.
     *
     * One can manually increase the size of the available stack memory by
     * calling growBy.
     */
    class MemoryStack
    {
        CK_CLASS_NON_COPYABLE(MemoryStack);

    public:
        MemoryStack();
        /**
         * Constructor initializing the memory pool.
         * @param initSize  The initial memory block count.
         * @param allocator An optional custom memory allocator.
         */
        MemoryStack(size_t initSize, const Allocator& allocator = Allocator());
        /**
         * Destructor.
         */
        ~MemoryStack();
        /** @cond */
        MemoryStack(MemoryStack&& other);
        MemoryStack& operator=(MemoryStack&& other);
        /** @endcond */
        /**
         * @return The size of the stack.
         */
        size_t limit() const;
        /**
         * Calculates the bytes allocated from the pool.
         * @return The number of bytes allocated
         */
        size_t count() const;
        /**
         * Allocates a block of memory
         * @param  memSize The number of bytes to allocate
         * @return Pointer to the allocated block or nullptr if out of memory
         */
        uint8_t* allocate(size_t memSize);
        /**
         * Allocate and construct a block of type T
         * @return Pointer to the constructed object or nullptr if out of memory
         */
        template<typename T, typename... Args> T* newItem(Args&&... args);
        /**
         * Attempts to grow the pool by the specified block count.
         * @param cnt Byte count to grow pool by.
         * @return    False if growth fails (out of memory.)
         */
        bool growBy(size_t cnt);
        /**
         * Resets the stack to the head
         */
        void reset();
        /**
         * @return The MemoryStack object's allocator
         */
        const Allocator& allocator() const { return _allocator; }

    private:
        Allocator _allocator;

        struct node
        {
            node* prev;
            node* next;
            uint8_t* first;
            uint8_t* last;
            uint8_t* limit;
            node() = default;
            size_t bytesAvailable() const { return limit - last; }
            size_t byteLimit() const { return limit - first; }
            size_t byteCount() const { return last - first; }
            bool alloc(size_t cnt, Allocator& allocator);
            void free(Allocator& allocator);
        };
        node* _tail;
        node* _current;

        void freeAll();
    };

    ////////////////////////////////////////////////////////////////////////////
    template<typename T, typename... Args>
    T* MemoryStack::newItem(Args&&... args)
    {
        uint8_t* p = allocate(sizeof(T));
        return ::new((void *)p) T(std::forward<Args>(args)...);
    }


    ////////////////////////////////////////////////////////////////////////////

}   // namespace cinek

#endif
