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

#ifndef CINEK_MEMORY_POOL_HPP
#define CINEK_MEMORY_POOL_HPP

#include "cinek/allocator.hpp"

#include <memory>

namespace cinekine {
    /**
     * @class ObjectPool
     * @brief Implements a simple stack-based memory allocation pool.
     *
     * The ObjectPool grabs memory from the supplied allocator in chunks.  Each
     * chunk contains a number of blocks of specified type T.
     *
     * When the pool runs out of blocks, it will attempt to allocate from the
     * supplied allocator up to double the block count of the last chunk -
     * basically meaning that the pool will exponentially grow as blocks are
     * used up from the current chunk.
     *
     * To prevent this exponential growth, one can call growBy to manually
     * increase the size of the pool by a fixed amount.  It's recommended that
     * callers try to use growBy, preempting the default ObjectPool growth
     * behavior described above.
     */
    template<typename T>
    class ObjectPool
    {
    public:
        /**
         * Constructor initializing the memory pool.
         * @param initBlockCount The initial memory block count.
         * @param allocator      An optional custom memory allocator.
         */
        ObjectPool(size_t initBlockCount, const Allocator& allocator = Allocator());
        /**
         * Destructor.  This also destroys any initialized objects in the pool.
         */
        ~ObjectPool();
        /** @cond */
        ObjectPool(ObjectPool&& other);
        ObjectPool& operator=(ObjectPool&& other);
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

        void freeAll();
    };

    ///////////////////////////////////////////////////////////////////////////

    template<typename T>
    bool ObjectPool<T>::node::alloc(size_t blockCount, Allocator& allocator)
    {
        first = last = allocator.allocItems<T>(blockCount);
        if (!first)
            return false;
        limit = first + blockCount;
        return true;
    }

    template<typename T>
    void ObjectPool<T>::node::destruct(Allocator& allocator)
    {
        while( last != first )
        {
            --last;
            last->~T();
        }
    }

    template<typename T>
    void ObjectPool<T>::node::free(Allocator& allocator)
    {
        destruct(allocator);
        allocator.free(first);
        first = last = limit = nullptr;
    }

    template<typename T>
    ObjectPool<T>::ObjectPool(size_t initBlockCount, const Allocator& allocator) :
        _allocator(allocator),
        _tail{ _allocator.newItem<node>() }
    {
        _tail->alloc(initBlockCount, _allocator);
    }

    template<typename T>
    ObjectPool<T>::~ObjectPool()
    {
        freeAll();
    }

    template<typename T>
    ObjectPool<T>::ObjectPool(ObjectPool<T>&& other) :
        _allocator(std::move(other._allocator)),
        _tail(std::move(other._tail))
    {
        other._tail = nullptr;
    }

    template<typename T>
    ObjectPool<T>& ObjectPool<T>::operator=(ObjectPool<T>&& other)
    {
        freeAll();
        _allocator = std::move(other._allocator);
        _tail = std::move(other._tail);
        return *this;
    }

    template<typename T>
    void ObjectPool<T>::freeAll()
    {
        while(_tail)
        {
            node* prev = _tail->prev;
            _tail->free(_allocator);
            _tail = prev;
        }
    }

    template<typename T>
    void ObjectPool<T>::destructAll()
    {
        node* cur = _tail;
        while(cur)
        {
            cur->destruct(_allocator);
            cur = cur->_prev;
        }
    }

    template<typename T>
    size_t ObjectPool<T>::blockLimit() const
    {
        size_t total = 0;
        node* cur = _tail;
        while (cur)
        {
            total += cur->blockLimit();
        }
        return total;
    }

    template<typename T>
    size_t ObjectPool<T>::blockCount() const
    {
        size_t total = 0;
        node* cur = _tail;
        while (cur)
        {
            total += cur->blockCount();
        }
        return total;
    }

    template<typename T> template<class... Args>
    T* ObjectPool<T>::allocateAndConstruct(Args&&... args)
    {
        if (!_tail->availCount())
        {
            //  create a new pool
            //  we'll take the size of the last chunk, and request another pool
            //  of the same size.
            if (!growBy(_tail->blockLimit()))
            {
            //    TODO("Support exception handling for auto-grow failure (CK_CPP_EXCEPTIONS).");
                return nullptr;
            }
        }
        T* p = _tail->last;
         ::new((void *)p) T(std::forward<Args>(args)...);
        ++_tail->last;
        return p;
    }

    template<typename T>
    bool ObjectPool<T>::growBy(size_t blockCount)
    {
        node *next = _allocator.newItem<node>();
        if (next)
        {
            if (next->alloc(blockCount, _allocator))
            {
                next->prev = _tail;
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
