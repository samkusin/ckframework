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
 * @file    cinek/objectpool.hpp
 * @author  Samir Sinha
 * @date    9/1/2014
 * @brief   Object allocation within a pooled heap
 * @copyright Cinekine
 */

#ifndef CINEK_OBJECT_POOL_HPP
#define CINEK_OBJECT_POOL_HPP

#include "debug.h"
#include "managed_handle.hpp"

namespace cinek {

    template<typename _T, typename _Allocator, size_t _Align=CK_ARCH_ALIGN_BYTES>
    class ObjectPool
    {
        CK_CLASS_NON_COPYABLE(ObjectPool);

    public:
        typedef _T          value_type;
        typedef _T*         pointer;
        typedef const _T*   const_pointer;

        ObjectPool();
        explicit ObjectPool(size_t blockLimit,
                   _Allocator allocator=_Allocator());
        ~ObjectPool();

        ObjectPool(ObjectPool&& other);
        ObjectPool& operator=(ObjectPool&& other);

        _Allocator allocator() const { return _allocator; }
        size_t blockLimit() const { return (_limit - _first); }
        size_t blockCount() const { return (_last - _first); }

        template<typename... Args> pointer construct(Args&&... args);
        pointer construct();
        void destruct(pointer p);

        bool verify(pointer p) const;

    private:
        void zeroVectors();

        _Allocator _allocator;
        uint8_t* _first;
        uint8_t* _last;
        uint8_t* _limit;
        pointer* _freefirst;
        pointer* _freelast;
        pointer* _freelimit;
    };

    template<typename _Object,
        typename _Derived,
        typename _Allocator,
        size_t _PoolAlign=CK_ARCH_ALIGN_BYTES>
    class ManagedObjectPoolBase
    {
        CK_CLASS_NON_COPYABLE(ManagedObjectPoolBase);

    public:
        using Value = _Object;
        using Handle = ManagedHandle<_Object, _Derived>;
        using Derived = _Derived;

        struct OwnerRef
        {
            Derived* owner;
        };

        struct Record
        {
            // must be first member of the Record to support ManagedHandle
            Value object;
            int refcnt;
            // links in the Record list, used for cleanup and traversal
            Record* next;
            Record* prev;
            // points to a persistant reference object for the owner.  This is
            // mainly for move operations
            OwnerRef* ownerRef;
        };

        ManagedObjectPoolBase();
        explicit ManagedObjectPoolBase(size_t count, _Allocator=_Allocator());
        ManagedObjectPoolBase(ManagedObjectPoolBase&& other) noexcept;
        ManagedObjectPoolBase& operator=(ManagedObjectPoolBase&& other) noexcept;

        ~ManagedObjectPoolBase();

    protected:
        Record* add(Value&& obj);
        Record* add();

        void releaseRecordInternal(Record* record);

        ObjectPool<Record, _Allocator, _PoolAlign> _recordsPool;
        Record* _head;

        void setOwnerRef(_Derived* owner);

    private:
        OwnerRef* _ownerRef;
    };


    /**
     *  @class ManagedObjectPool
     *
     *  _Delegate must implement the following concept:
     *
     *      void onReleaseManagedObject(Node& node);
     */
    template<typename _Object, typename _Delegate, typename _Allocator,
             size_t _PoolAlign=CK_ARCH_ALIGN_BYTES>
    class ManagedObjectPool :
        public ManagedObjectPoolBase<_Object,
            ManagedObjectPool<_Object, _Delegate, _Allocator, _PoolAlign>,
            _Allocator,
            _PoolAlign>
    {
        CK_CLASS_NON_COPYABLE(ManagedObjectPool);

    public:
        using ThisType = ManagedObjectPool<_Object, _Delegate, _Allocator, _PoolAlign>;
        using BaseType = ManagedObjectPoolBase<_Object, ThisType, _Allocator, _PoolAlign>;
        using Handle = typename BaseType::Handle;
        using Value = typename BaseType::Value;

        ManagedObjectPool();
        ~ManagedObjectPool();
        explicit ManagedObjectPool(size_t count, _Allocator allocator=_Allocator());
        ManagedObjectPool(ManagedObjectPool&& other) noexcept;
        ManagedObjectPool& operator=(ManagedObjectPool&& other) noexcept;

        void setDelegate(const _Delegate& del);
        void clearDelegate();

        Handle add(Value&& obj);
        Handle add();

        void destructAll();

    private:
        friend Handle;
        // MSVC 2015 does not resolve typename BaseType::Record properly during codegen
        //  - expanding BaseType seems to work
        //  - Clang (and likely GCC) do not have this problem
        void releaseRecord(typename ManagedObjectPoolBase<_Object,
                                                ThisType,
                                                _Allocator,
                                                _PoolAlign>::Record* record);

        _Delegate _delegate;
    };


    template<typename _Object, typename _Allocator, size_t _PoolAlign>
    class ManagedObjectPool<_Object, void, _Allocator, _PoolAlign> :
        public ManagedObjectPoolBase<_Object,
            ManagedObjectPool<_Object, void, _Allocator, _PoolAlign>,
            _Allocator,
            _PoolAlign>
    {
        CK_CLASS_NON_COPYABLE(ManagedObjectPool);

    public:
        using ThisType = ManagedObjectPool<_Object, void, _Allocator, _PoolAlign>;
        using BaseType = ManagedObjectPoolBase<_Object, ThisType, _Allocator, _PoolAlign>;
        using Handle = typename BaseType::Handle;
        using Value = typename BaseType::Value;

        ManagedObjectPool() = default;
        ~ManagedObjectPool();
        explicit ManagedObjectPool(size_t count, _Allocator allocator=_Allocator());
        ManagedObjectPool(ManagedObjectPool&& other) noexcept;
        ManagedObjectPool& operator=(ManagedObjectPool&& other) noexcept;

        Handle add(Value&& obj);
        Handle add();

        void destructAll();

    private:
        friend Handle;

        // MSVC 2015 does not resolve typename BaseType::Record properly during codegen
        //  - expanding BaseType seems to work
        //  - Clang (and likely GCC) do not have this problem
        void releaseRecord(typename ManagedObjectPoolBase<_Object, ThisType, _Allocator, _PoolAlign>::Record* record);
    };

} /* namespace cinek */

#endif
