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
 * @file    cinek/cstringstack.hpp
 * @author  Samir Sinha
 * @date    4/5/2015
 * @brief   String allocation within a pooled heap
 * @copyright Cinekine
 */

#ifndef CINEK_CSTRING_STACK_HPP
#define CINEK_CSTRING_STACK_HPP

#include "allocator.hpp"
#include "memorystack.hpp"
#include <cstring>

namespace cinek {
    /**
     * @class CStringStack
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
    template<typename _Allocator>
    class CStringStack
    {
        CK_CLASS_NON_COPYABLE(CStringStack);

    public:
        CStringStack();

        /**
         * Constructor initializing the memory pool.
         * @param initSize  The initial memory block count.
         * @param allocator An optional custom memory allocator.
         */
        CStringStack(size_t initSize, const _Allocator& allocator = Allocator());

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
        /** @return The byte capacity of the string stack */
        size_t capacity() const {
            return _stack.capacity();
        }
        /** @return The byte size (used bytes) of the string stack */
        size_t size() const {
            return _stack.size();
        }
    private:
        MemoryStack<_Allocator> _stack;
        size_t _count;
        static const char* kEmptyString;
    };

    ////////////////////////////////////////////////////////////////////////////
    template<typename _Allocator>
    CStringStack<_Allocator>::CStringStack() :
        _count(0)
    {
    }

    template<typename _Allocator>
    CStringStack<_Allocator>::CStringStack(size_t initSize, const _Allocator& allocator) :
        _stack(initSize, allocator),
        _count(0)
    {
    }

    template<typename _Allocator>
    CStringStack<_Allocator>::CStringStack(CStringStack&& other) :
        _stack(std::move(other._stack)),
        _count(other._count)
    {
        other._count = 0;
    }

    template<typename _Allocator>
    CStringStack<_Allocator>& CStringStack<_Allocator>::operator=(CStringStack&& other)
    {
        _stack = std::move(other._stack);
        _count = other._count;
        other._count = 0;
        return *this;
    }

    template<typename _Allocator>
    const char* CStringStack<_Allocator>::create(const char* str)
    {
        char* buf;
        if (str && str[0]) {
            size_t len = strlen(str);
            buf = reinterpret_cast<char*>(_stack.allocate(len+1));
            if (!buf)
                return NULL;
            strncpy(buf, str, len);
            buf[len] = 0;
        }
        else {
            buf = reinterpret_cast<char*>(_stack.allocate(1));
            if (!buf)
                return NULL;
            *buf = 0;
        }
        ++_count;
        return buf;
    }

    template<typename _Allocator>
    bool CStringStack<_Allocator>::growBy(size_t cnt)
    {
        return _stack.growBy(cnt);
    }

    template<typename _Allocator>
    void CStringStack<_Allocator>::reset()
    {
        _stack.reset();
        _count = 0;
    }


}   // namespace cinek

#endif
