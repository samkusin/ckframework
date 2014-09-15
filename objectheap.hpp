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
 * @file    cinek/objectheap.hpp
 * @author  Samir Sinha
 * @date    4/14/2013
 * @brief   Object allocation within a pooled heap
 * @copyright Cinekine
 */

#ifndef CINEK_OBJECT_HEAP_HPP
#define CINEK_OBJECT_HEAP_HPP

#include "cinek/allocator.hpp"

#include <memory>

namespace cinekine {

    /**
     * @class ObjectHeap
     * @brief Implements a simple stack-based memory allocation pool.
     *
     * The ObjectHeap grabs memory from the supplied allocator in chunks.  Each
     * chunk contains a number of blocks of specified type T.
     *
     * When the pool runs out of blocks, it will attempt to allocate from the
     * supplied allocator up to double the block count of the last chunk -
     * basically meaning that the pool will exponentially grow as blocks are
     * used up from the current chunk.
     *
     * To prevent this exponential growth, one can call growBy to manually
     * increase the size of the pool by a fixed amount.  It's recommended that
     * callers try to use growBy, preempting the default ObjectHeap growth
     * behavior described above.
     */
    template<typename T>
    class ObjectHeap
    {
        CK_CLASS_NON_COPYABLE(ObjectHeap);

    public:
        /**
         * Constructor initializing the memory pool.
         * @param initBlockCount The initial memory block count.
         * @param allocator      An optional custom memory allocator.
         */
        ObjectHeap(size_t initBlockCount, const Allocator& allocator = Allocator());
        /**
         * Destructor.  This also destroys any initialized objects in the pool.
         */
        ~ObjectHeap();
        /** @cond */
        ObjectHeap(ObjectHeap&& other);
        ObjectHeap& operator=(ObjectHeap&& other);
        /** @endcond */
        /**
         * Calculates the number of total number of blocks in the pool.
         * @return The total number of blocks in the pool.
         */
        size_t blockLimit() const;
        /**
         * Calculates the number of blocks allocated from the pool.
         * @return The total number of allocated blocks from the pool.
         */
        size_t blockCount() const;
        /**
         * Allocates a block of type T.
         * @return Pointer to the allocated block or nullptr if out of memory.
         */
        template<class... Args> T* allocateAndConstruct(Args&&... args);
        /**
         * Allocates a block of type T (shorter version of allocateAndConstruct)
         * @return Pointer to the allocated block or nullptr if out of memory.
         */
        template<class... Args> T* allocate(Args&&... args);
        /**
         * Attempts to grow the pool by the specified block count.
         * @param blockCount Block count to grow pool by.
         * @return           False if growth fails (out of memory.)
         */
        bool growBy(size_t blockCount);
        /**
         * Deallocates all blocks, invalidating all prior allocated blocks.
         */
        void destructAll();

    private:
        Allocator _allocator;

        struct node
        {
            node* prev;
            node* next;
            T* first;
            T* last;
            T* limit;
            node() = default;
            size_t availCount() const { return limit - last; }
            size_t blockLimit() const { return limit - first; }
            size_t blockCount() const { return last - first; }
            bool alloc(size_t blockCount, Allocator& allocator);
            void free(Allocator& allocator);
            void destruct(Allocator& allocator);
        };
        node* _tail;
        node* _current;

        void freeAll();
    };

    ///////////////////////////////////////////////////////////////////////////

    template<typename T>
    bool ObjectHeap<T>::node::alloc(size_t blockCount, Allocator& allocator)
    {
        first = last = allocator.allocItems<T>(blockCount);
        if (!first)
            return false;
        limit = first + blockCount;
        return true;
    }

    template<typename T>
    void ObjectHeap<T>::node::destruct(Allocator& allocator)
    {
        while( last != first )
        {
            --last;
            last->~T();
        }
    }

    template<typename T>
    void ObjectHeap<T>::node::free(Allocator& allocator)
    {
        destruct(allocator);
        allocator.free(first);
        first = last = limit = nullptr;
    }

    template<typename T>
    ObjectHeap<T>::ObjectHeap(size_t initBlockCount, const Allocator& allocator) :
        _allocator(allocator),
        _tail(_allocator.newItem<node>()),
        _current(_tail)
    {
        _tail->alloc(initBlockCount, _allocator);
    }

    template<typename T>
    ObjectHeap<T>::~ObjectHeap()
    {
        freeAll();
    }

    template<typename T>
    ObjectHeap<T>::ObjectHeap(ObjectHeap<T>&& other) :
        _allocator(std::move(other._allocator)),
        _tail(std::move(other._tail)),
        _current(std::move(other._current))
    {
        other._tail = nullptr;
        other._current = nullptr;
    }

    template<typename T>
    ObjectHeap<T>& ObjectHeap<T>::operator=(ObjectHeap<T>&& other)
    {
        freeAll();
        _allocator = std::move(other._allocator);
        _tail = std::move(other._tail);
        _current = std::move(other._current);
        return *this;
    }

    template<typename T>
    void ObjectHeap<T>::freeAll()
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

    template<typename T>
    void ObjectHeap<T>::destructAll()
    {
        node* cur = _tail;
        while(cur)
        {
            cur->destruct(_allocator);
            cur = cur->prev;
            if (cur)
                _current = cur;
        }
    }

    template<typename T>
    size_t ObjectHeap<T>::blockLimit() const
    {
        size_t total = 0;
        node* cur = _tail->blockLimit();
        while (cur)
        {
            total += cur->blockLimit();
            cur = cur->_prev;
        }
        return total;
    }

    template<typename T>
    size_t ObjectHeap<T>::blockCount() const
    {
        size_t total = 0;
        node* cur = _current;
        while (cur)
        {
            total += cur->blockCount();
            cur = cur->_prev;
        }
        return total;
    }

    template<typename T> template<class... Args>
    T* ObjectHeap<T>::allocateAndConstruct(Args&&... args)
    {
        if (!_current->availCount())
        {
            node* next = _current->next;

            if (!next)
            {
                //  create a new pool
                //  we'll take the size of the last chunk, and request another pool
                //  of the same size.
                if (!growBy(_tail->blockLimit()))
                {
                //    TODO("Support exception handling for auto-grow failure (CK_CPP_EXCEPTIONS).");
                    return nullptr;
                }
                next = _tail;
            }
            _current = next;
        }
        T* p = _current->last;
         ::new((void *)p) T(std::forward<Args>(args)...);
        ++_current->last;
        return p;
    }

    template<typename T> template<class... Args>
    T* ObjectHeap<T>::allocate(Args&&... args)
    {
        return allocateAndConstruct(std::forward<Args>(args)...);
    }

    template<typename T>
    bool ObjectHeap<T>::growBy(size_t blockCount)
    {
        node *next = _allocator.newItem<node>();
        if (next)
        {
            if (next->alloc(blockCount, _allocator))
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


    ////////////////////////////////////////////////////////////////////////////

}   // namespace cinekine

#endif
