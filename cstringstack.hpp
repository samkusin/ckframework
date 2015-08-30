/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Cinekine Media
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
 * @file    cstringstack.hpp
 * @author  Samir Sinha
 * @date    4/5/2015
 * @brief   String allocation within a pooled heap
 * @copyright Cinekine
 */

#ifndef CINEK_CSTRING_STACK_HPP
#define CINEK_CSTRING_STACK_HPP

#include "allocator.hpp"
#include "memorystack.hpp"

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
    class CStringStack
    {
        CK_CLASS_NON_COPYABLE(CStringStack);

    public:
        /**
         * Constructor initializing the memory pool.
         * @param initSize  The initial memory block count.
         * @param allocator An optional custom memory allocator.
         */
        CStringStack(size_t initSize, const Allocator& allocator = Allocator());

        /** @cond */
        CStringStack(CStringStack&& other);
        CStringStack& operator=(CStringStack&& other);
        /** @endcond */
        /**
         * @return Number of strings in the pool
         */
        size_t count() const {
            return _count;
        }
        /**
         * Allocates a block of memory
         * @param  str The string to copy
         * @return Pointer to the allocated string
         */
        const char* create(const char* str);
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

    private:
        MemoryStack _stack;
        size_t _count;
        static const char* kEmptyString;
    };

    ////////////////////////////////////////////////////////////////////////////

}   // namespace cinek

#endif
