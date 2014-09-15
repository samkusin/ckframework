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
 * @file    cinek/memorypool.hpp
 * @author  Samir Sinha
 * @date    4/14/2013
 * @brief   Object allocation within a pooled heap
 * @copyright Cinekine
 */

#ifndef CINEK_MEMORY_REGION_HPP
#define CINEK_MEMORY_REGION_HPP

#include "cinek/allocator.hpp"
#include "cinek/debug.hpp"

namespace cinekine {

    template<size_t _BlockSize>
    class MemoryRegion
    {
        CK_CLASS_NON_COPYABLE(MemoryRegion);

    public:
        MemoryRegion(size_t blockCount, const Allocator& allocator);
        ~MemoryRegion();

        operator bool() const { return _first != nullptr && _freefirst != nullptr; }

        size_t blocksRemaining() const { return _limit - _last; }
        size_t blockLimit() const { return _limit - _first; }
        size_t blockCount() const { return _last - _first; }

        uint8_t* allocate();
        void deallocate(uint8_t* block);

    private:
        Allocator _allocator;
        uint8_t* _first;
        uint8_t* _last;
        uint8_t* _limit;
        uint8_t** _freefirst;
        uint8_t** _freelast;
        uint8_t** _freelimit;
    };

    template<size_t _BlockSize>
    MemoryRegion<_BlockSize>::MemoryRegion(size_t blockCount, const Allocator& allocator) :
        _allocator(allocator),
        _first(nullptr),
        _last(nullptr),
        _limit(nullptr),
        _freefirst(nullptr),
        _freelast(nullptr),
        _freelimit(nullptr)
    {
        const size_t kByteCount = _BlockSize * blockCount;
        _first = reinterpret_cast<uint8_t*>(_allocator.alloc(kByteCount));
        _last = _first;
        _limit = _first + kByteCount;
        _freefirst = reinterpret_cast<uint8_t**>(_allocator.alloc(blockCount * sizeof(uint8_t*)));
        _freelast = _freefirst;
        _freelimit = _freefirst + blockCount;
    }

    template<size_t _BlockSize>
    MemoryRegion<_BlockSize>::~MemoryRegion()
    {
        _allocator.free(_freefirst);
        _allocator.free(_first);
    }

    template<size_t _BlockSize>
    uint8_t* MemoryRegion<_BlockSize>::allocate()
    {
        uint8_t* block;
        if (_freefirst != _freelast)
        {
            --_freelast;
            block = *_freelast;
        }
        else if (_last < _limit)
        {
            block = _last;
            _last += _BlockSize;
        }
        else
        {
            block = nullptr;
        }

        CK_ASSERT(block != nullptr);

        return block;
    }

    template<size_t _BlockSize>
    void MemoryRegion<_BlockSize>::deallocate(uint8_t* block)
    {
    #if CK_DEBUG_ASSERT
        uintptr_t diff = reinterpret_cast<uintptr_t>(block) - reinterpret_cast<uintptr_t>(_first);
        CK_ASSERT(!(diff % _BlockSize) && block < _last);
        CK_ASSERT(_freelast < _freelimit);
    #endif
    #if CK_MEMORY_DEBUG_VERBOSE
        uint8_t** freeptr = _freelast;
        while (freeptr > _freefirst)
        {
            CK_DEBUG_ASSERT(*freeptr != block);
            --freeptr;
        }
    #endif
        *_freelast = block;
        ++_freelast;
    }

} /* namespace cinekine */

#endif
