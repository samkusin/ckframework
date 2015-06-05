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
 * @file    cinek/memorystack.cpp
 * @author  Samir Sinha
 * @date    4/14/2013
 * @brief   Object allocation within a pooled heap
 * @copyright Cinekine
 */

#include "cinek/memorystack.hpp"

namespace cinek {

    MemoryStack::MemoryStack() :
        _tail(nullptr),
        _current(nullptr)
    {
    }

    bool MemoryStack::node::alloc(size_t cnt, Allocator& allocator)
    {
        first = last = (uint8_t*)allocator.alloc(cnt);
        if (!first)
            return false;
        limit = first + cnt;
        return true;
    }

    void MemoryStack::node::free(Allocator& allocator)
    {
        allocator.free(first);
        first = last = limit = nullptr;
    }

    MemoryStack::MemoryStack(size_t initSize, const Allocator& allocator) :
        _allocator(allocator),
        _tail(_allocator.newItem<node>()),
        _current(_tail)
    {
        _tail->alloc(initSize, _allocator);
    }

    MemoryStack::~MemoryStack()
    {
        freeAll();
    }

    MemoryStack::MemoryStack(MemoryStack&& other) :
        _allocator(std::move(other._allocator)),
        _tail(std::move(other._tail)),
        _current(std::move(other._current))
    {
        other._tail = nullptr;
        other._current = nullptr;
    }

    MemoryStack& MemoryStack::operator=(MemoryStack&& other)
    {
        freeAll();
        _allocator = std::move(other._allocator);
        _tail = std::move(other._tail);
        _current = std::move(other._current);
        return *this;
    }

    void MemoryStack::freeAll()
    {
        while(_tail)
        {
            node* prev = _tail->prev;
            if (prev)
            {
                prev->next = nullptr;
            }
            _tail->free(_allocator);
            _tail = prev;
        }
        _current = nullptr;
    }

    size_t MemoryStack::limit() const
    {
        size_t total = 0;
        node* cur = _tail;
        while (cur)
        {
            total += cur->byteLimit();
            cur = cur->prev;
        }
        return total;
    }

    size_t MemoryStack::count() const
    {
        size_t total = 0;
        node* cur = _current;
        while (cur)
        {
            total += cur->byteCount();
            cur = cur->prev;
        }
        return total;
    }

    uint8_t* MemoryStack::allocate(size_t memSize)
    {
        while (_current->bytesAvailable() < memSize)
        {
            node* next = _current->next;

            if (!next)
            {
                //  create a new pool
                //  we'll take the size of the last chunk, and request another pool
                //  of the same size.
                if (!growBy(_tail->byteLimit()))
                {
                //    TODO("Support exception handling for auto-grow failure (CK_CPP_EXCEPTIONS).");
                    return nullptr;
                }
                next = _tail;
            }
            _current = next;
        }
        uint8_t* p = _current->last;
        _current->last += memSize;
        return p;
    }

    bool MemoryStack::growBy(size_t cnt)
    {
        node *next = _allocator.newItem<node>();
        if (next)
        {
            if (next->alloc(cnt, _allocator))
            {
                next->prev = _tail;
                _tail->next = next;
                _tail = next;
                return true;
            }
            else
            {
                _allocator.deleteItem<node>(next);
            }
        }
        return false;
    }

    void MemoryStack::reset()
    {
        node *n = _tail;
        if (!n)
            return;
        while (n->prev)
        {
            n->last = n->first;
            n = n->prev;
        }
        n->last = n->first;
        _current = n;
    }

} /* namespace cinek */
