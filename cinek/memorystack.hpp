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
    template<typename _Allocator>
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
        MemoryStack(size_t initSize, const _Allocator& allocator = Allocator());
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
        size_t capacity() const;
        /**
         * Calculates the bytes allocated from the pool.
         * @return The number of bytes allocated
         */
        size_t size() const;
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
        const _Allocator& allocator() const { return _allocator; }

    private:
        _Allocator _allocator;

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
            bool alloc(size_t cnt, _Allocator& allocator);
            void free(_Allocator& allocator);
        };
        node* _tail;
        node* _current;

        void freeAll();
    };

    ////////////////////////////////////////////////////////////////////////////

    template<typename _Allocator>
    template<typename T, typename... Args>
    T* MemoryStack<_Allocator>::newItem(Args&&... args)
    {
        uint8_t* p = allocate(sizeof(T));
        return ::new((void *)p) T(std::forward<Args>(args)...);
    }

    template<typename _Allocator>
    MemoryStack<_Allocator>::MemoryStack() :
        _tail(nullptr),
        _current(nullptr)
    {
    }

    template<typename _Allocator>
    bool MemoryStack<_Allocator>::node::alloc(size_t cnt, _Allocator& allocator)
    {
        first = last = (uint8_t*)allocator.alloc(cnt);
        if (!first)
            return false;
        limit = first + cnt;
        return true;
    }

    template<typename _Allocator>
    void MemoryStack<_Allocator>::node::free(Allocator& allocator)
    {
        allocator.free(first);
        first = last = limit = nullptr;
    }

    template<typename _Allocator>
    MemoryStack<_Allocator>::MemoryStack(size_t initSize, const Allocator& allocator) :
        _allocator(allocator),
        _tail(initSize > 0 ? _allocator.newItem<node>() : nullptr),
        _current(_tail)
    {
        if (_tail)
        {
            _tail->alloc(initSize, _allocator);
        }
    }

    template<typename _Allocator>
    MemoryStack<_Allocator>::~MemoryStack()
    {
        freeAll();
    }

    template<typename _Allocator>
    MemoryStack<_Allocator>::MemoryStack(MemoryStack&& other) :
        _allocator(std::move(other._allocator)),
        _tail(std::move(other._tail)),
        _current(std::move(other._current))
    {
        other._tail = nullptr;
        other._current = nullptr;
    }

    template<typename _Allocator>
    MemoryStack<_Allocator>& MemoryStack<_Allocator>::operator=(MemoryStack&& other)
    {
        freeAll();
        _allocator = std::move(other._allocator);
        _tail = std::move(other._tail);
        _current = std::move(other._current);
        return *this;
    }

    template<typename _Allocator>
    void MemoryStack<_Allocator>::freeAll()
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

    template<typename _Allocator>
    size_t MemoryStack<_Allocator>::capacity() const
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

    template<typename _Allocator>
    size_t MemoryStack<_Allocator>::size() const
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

    template<typename _Allocator>
    uint8_t* MemoryStack<_Allocator>::allocate(size_t memSize)
    {
        while (_current->bytesAvailable() < memSize)
        {
            node* next = _current->next;

            if (!next)
            {
                //  create a new pool
                //  we'll take the size of the last chunk, and request another pool
                //  of the same size.
                size_t growByAmt = _tail->byteLimit();
                if (growByAmt < memSize)
                    growByAmt = memSize * 2;
                if (!growBy(growByAmt))
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

    template<typename _Allocator>
    bool MemoryStack<_Allocator>::growBy(size_t cnt)
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

    template<typename _Allocator>
    void MemoryStack<_Allocator>::reset()
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



}   // namespace cinek

#endif
