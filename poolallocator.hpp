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
 * @file    cinek/poolallocator.hpp
 * @author  Samir Sinha
 * @date    9/1/2014
 * @brief   Object allocation within a pooled heap
 * @copyright Cinekine
 */

#ifndef CINEK_POOL_ALLOCATOR_HPP
#define CINEK_POOL_ALLOCATOR_HPP

#include "cinek/allocator.hpp"
#include "cinek/debug.hpp"

namespace cinek {

    template<typename _T>
    class ObjectPool
    {
        CK_CLASS_NON_COPYABLE(ObjectPool);

    public:
        typedef _T          value_type;
        typedef _T*         pointer;
        typedef const _T*   const_pointer;

        ObjectPool(size_t blockLimit, const Allocator& allocator);
        ~ObjectPool();

        ObjectPool(ObjectPool&& other);
        ObjectPool& operator=(ObjectPool&& other);

        size_t blockLimit() const { return (_limit - _first); }
        size_t blockCount() const { return (_last - _first); }

        template<typename... Args> pointer construct(Args&&... args);
        void destruct(pointer p);

    private:
        void zeroVectors();

        Allocator _allocator;
        pointer _first;
        pointer _last;
        pointer _limit;
        pointer* _freefirst;
        pointer* _freelast;
        pointer* _freelimit;
    };

    template<typename _T>
    ObjectPool<_T>::ObjectPool(size_t blockCount, const Allocator& allocator) :
        _allocator(allocator),
        _first(nullptr),
        _last(nullptr),
        _limit(nullptr),
        _freefirst(nullptr),
        _freelast(nullptr),
        _freelimit(nullptr)
    {
        _first = reinterpret_cast<pointer>(_allocator.alloc(sizeof(_T) * blockCount));
        _last = _first;
        _limit = _first + blockCount;
        _freefirst = reinterpret_cast<pointer*>(_allocator.alloc(blockCount * sizeof(pointer)));
        _freelast = _freefirst;
        _freelimit = _freefirst + blockCount;
    }

    template<typename _T>
    ObjectPool<_T>::~ObjectPool()
    {
        _allocator.free(_freefirst);
        _allocator.free(_first);
    }

    template<typename _T>
    ObjectPool<_T>::ObjectPool(ObjectPool&& other) :
        _allocator(std::move(other._allocator)),
        _first(other._first),
        _last(other._last),
        _limit(other._limit),
        _freefirst(other._freefirst),
        _freelast(other._freelast),
        _freelimit(other._freelimit)
    {
        other.zeroVectors();
    }

    template<typename _T>
    ObjectPool<_T>& ObjectPool<_T>::operator=(ObjectPool&& other)
    {
        _allocator = std::move(other._allocator);
        _first = other._first;
        _last = other._last;
        _limit = other._limit;
        _freefirst = other._freefirst;
        _freelast = other._freelast;
        _freelimit = other._freelimit;

        other.zeroVectors();

        return *this;
    }

    template<typename _T>
    void ObjectPool<_T>::zeroVectors()
    {
        _first = nullptr;
        _last = nullptr;
        _limit = nullptr;
        _freefirst = nullptr;
        _freelast = nullptr;
        _freelimit = nullptr;
    }

    template<typename _T> template<typename... Args>
    auto ObjectPool<_T>::construct(Args&&... args) -> pointer
    {
        pointer p = nullptr;
        if (_freefirst != _freelast)
        {
            --_freelast;
            p = *_freelast;
        }
        else if (_last < _limit)
        {
            p = _last;
            ++_last;
        }

        CK_ASSERT(p);
        if (p)
        {
            ::new(p) _T(std::forward<Args>(args)...);
        }

        return p;
    }

    template<typename _T>
    void ObjectPool<_T>::destruct(pointer p)
    {
        if (!p)
            return;

        CK_ASSERT(p >= _first && p < _limit);
        CK_ASSERT(_freelast < _freelimit);
        if (_freelast >= _freelimit)
            return;

        p->~value_type();

        *_freelast = p;
        ++_freelast;
    }

} /* namespace cinek */

#endif
