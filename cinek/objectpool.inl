//
//  objectpool.inl
//  GfxPrototype
//
//  Created by Samir Sinha on 9/26/15.
//
//

namespace cinek {

    template<typename _T, size_t _Align, typename _Allocator>
    ObjectPool<_T, _Align, _Allocator>::ObjectPool() :
        _first(nullptr),
        _last(nullptr),
        _limit(nullptr),
        _freefirst(nullptr),
        _freelast(nullptr),
        _freelimit(nullptr)
    {
    }

    template<typename _T, size_t _Align, typename _Allocator>
    ObjectPool<_T, _Align, _Allocator>::ObjectPool
    (
        size_t blockCount,
        _Allocator allocator
    ) :
        _allocator(allocator),
        _first(nullptr),
        _last(nullptr),
        _limit(nullptr),
        _freefirst(nullptr),
        _freelast(nullptr),
        _freelimit(nullptr)
    {
        size_t allocAmt = CK_ALIGN_SIZE(sizeof(_T), _Align);
        allocAmt *= blockCount;

        if (allocAmt > 0) {
            _first = reinterpret_cast<uint8_t*>(_allocator.allocAligned(allocAmt, _Align));
            _last = _first;
            _limit = _first + allocAmt;
            _freefirst = reinterpret_cast<pointer*>(_allocator.alloc(blockCount * sizeof(pointer)));
            _freelast = _freefirst;
            _freelimit = _freefirst + blockCount;
        }
    }

    template<typename _T, size_t _Align, typename _Allocator>
    ObjectPool<_T, _Align, _Allocator>::~ObjectPool()
    {
        _allocator.free(_freefirst);
        _allocator.freeAligned(_first);
    }

    template<typename _T, size_t _Align, typename _Allocator>
    ObjectPool<_T, _Align, _Allocator>::ObjectPool(ObjectPool&& other) :
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

    template<typename _T, size_t _Align, typename _Allocator>
    ObjectPool<_T, _Align, _Allocator>& ObjectPool<_T, _Align, _Allocator>::operator=(ObjectPool&& other)
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

    template<typename _T, size_t _Align, typename _Allocator>
    void ObjectPool<_T, _Align, _Allocator>::zeroVectors()
    {
        _first = nullptr;
        _last = nullptr;
        _limit = nullptr;
        _freefirst = nullptr;
        _freelast = nullptr;
        _freelimit = nullptr;
    }

    template<typename _T, size_t _Align, typename _Allocator>
    bool ObjectPool<_T, _Align, _Allocator>::verify(pointer p) const
    {
        return (p >= (pointer)_first && p < (pointer)_last);
    }

    template<typename _T, size_t _Align, typename _Allocator> template<typename... Args>
    auto ObjectPool<_T, _Align, _Allocator>::construct(Args&&... args) -> pointer
    {
        pointer p = nullptr;
        if (_freefirst != _freelast)
        {
            --_freelast;
            p = *_freelast;
        }
        else if (_last < _limit)
        {
            p = reinterpret_cast<pointer>(_last);
            _last += CK_ALIGN_SIZE(sizeof(_T), _Align);
            CK_ASSERT(_last <= _limit);
        }

        CK_ASSERT(p);
        if (p)
        {
            ::new(p) _T(std::forward<Args>(args)...);
        }

        return p;
    }

    template<typename _T, size_t _Align, typename _Allocator>
    void ObjectPool<_T, _Align, _Allocator>::destruct(pointer p)
    {
        if (!p)
            return;

        CK_ASSERT((uint8_t*)p >= _first && (uint8_t*)p < _limit);
        CK_ASSERT(_freelast < _freelimit);
        if (_freelast >= _freelimit)
            return;

        p->~value_type();

        *_freelast = p;
        ++_freelast;
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>::ManagedObjectPoolBase() :
        _head(nullptr),
        _ownerRef(nullptr)
    {
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>::ManagedObjectPoolBase
    (
        size_t count
    ) :
        _recordsPool(count),
        _head(nullptr),
        _ownerRef(nullptr)
    {
        Allocator allocator;
        _ownerRef = reinterpret_cast<OwnerRef*>(allocator.alloc(sizeof(OwnerRef)));
        _ownerRef->owner = static_cast<_Derived*>(this);
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>::ManagedObjectPoolBase
    (
        ManagedObjectPoolBase&& other
    )
    noexcept :
        _recordsPool(std::move(other._recordsPool)),
        _head(other._head),
        _ownerRef(other._ownerRef)
    {
        other._head = nullptr;
        other._ownerRef = nullptr;

        setOwnerRef(static_cast<_Derived*>(this));
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>::~ManagedObjectPoolBase()
    {
        //  note - this destructor does not cleanup the records themselves.
        //  derived destructors take care of this since each derived impl
        //  can have its own destruction strategy
        if (_ownerRef) {
            Allocator allocator;
            allocator.free(_ownerRef);
            _ownerRef = nullptr;
        }
        _head = nullptr;    // memory invalidated by recordspool cleanup
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    auto ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>::operator=
    (
        ManagedObjectPoolBase&& other
    )
    noexcept -> ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>&
    {
        _recordsPool = std::move(other._recordsPool);
        _head = other._head;
        _ownerRef = other._ownerRef;

        other._head = nullptr;
        other._ownerRef = nullptr;

        setOwnerRef(static_cast<_Derived*>(this));

        return *this;
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    auto ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>::add(Value&& obj) -> Record*
    {
        Record* r = add();
        if (r) {
            r->object = std::move(obj);
        }
        return r;
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    auto ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>::add() -> Record*
    {
        Record* record = _recordsPool.construct();
        if (record) {
            record->ownerRef = _ownerRef;
            record->refcnt = 0;

            if (_head) {
                Record* tail = _head->prev;
                _head->prev = record;
                tail->next = record;
                record->prev = tail;
            }
            else {
                record->prev = record;
                _head = record;
            }

            record->next = nullptr;

        }
        return record;
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    void ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>::releaseRecordInternal(Record *record)
    {
        if (record->next) {
            record->next->prev = record->prev;
        }
        else {
            // record is tail, change head prev link accordingly.  next lines
            // will fixup the new tail's next ptr to null (tail->next)
            _head->prev = record->prev;
        }
        if (record->prev->next) {
            record->prev->next = record->next;
        }
        else {  //  record must be head
            _head = record->next;
        }

        record->prev = nullptr;
        record->next = nullptr;

        _recordsPool.destruct(record);
    }

    ////////////////////////////////////////////////////////////////////////////

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::ManagedObjectPool() :
        _delegate()
    {
    }

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::ManagedObjectPool
    (
        size_t count
    ) :
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>, _PoolAlign>(count),
        _delegate()
    {
    }

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::ManagedObjectPool
    (
        ManagedObjectPool&& other
    )
    noexcept :
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>, _PoolAlign>(std::move(other)),
        _delegate(std::move(other._delegate))
    {
        other._delegate = nullptr;
    }

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::~ManagedObjectPool()
    {
        destructAll();
    }

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    auto ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::operator=
    (
        ManagedObjectPool&& other
    )
    noexcept -> ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>&
    {
        ManagedObjectPoolBase<_Object, ThisType, _PoolAlign, _Allocator>::operator=(std::move(other));
        _delegate = std::move(other._delegate);
        other._delegate = nullptr;
        return *this;
    }

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    void ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::releaseRecord
    (
        typename ManagedObjectPoolBase<_Object, ThisType, _PoolAlign, _Allocator>::Record *record
    )
    {
        if (_delegate) {
            _delegate->onReleaseManagedObject(record->object);
        }

        BaseType::releaseRecordInternal(record);
    }

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    void ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::setDelegate(const _Delegate& del)
    {
        _delegate = del;
    }

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    void ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::clearDelegate()
    {
        _delegate = nullptr;
    }

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    auto ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::add(Value&& obj) -> Handle
    {
        return Handle(&BaseType::add(std::forward<Value>(obj))->object);
    }

    template<typename _Object, typename _Delegate, size_t _PoolAlign, typename _Allocator>
    auto ManagedObjectPool<_Object, _Delegate, _PoolAlign, _Allocator>::add() -> Handle
    {
        return Handle(&BaseType::add()->object);
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    void ManagedObjectPool<_Object, _Derived, _PoolAlign, _Allocator>::destructAll()
    {
        //  prevent handle releases from affecting our object teardown
        //  mainly in cases where our Objects contain handles to other Objects
        //  within the pool
        BaseType::setOwnerRef(nullptr);

        //  destroy our entire list.
        //  releaseRecord must call releaseRecordInternal for this to work.
        while (BaseType::_head) {
            releaseRecord(BaseType::_head->prev);
        }

        BaseType::setOwnerRef(this);
    }

    template<typename _Object, typename _Derived, size_t _PoolAlign, typename _Allocator>
    void ManagedObjectPoolBase<_Object, _Derived, _PoolAlign, _Allocator>::setOwnerRef(_Derived* owner)
    {
        if (_ownerRef) {
            _ownerRef->owner = owner;
        }
    }


    ////////////////////////////////////////////////////////////////////////////

    template<typename _Object, size_t _PoolAlign, _typename _Allocator>
    ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>::ManagedObjectPool
    (
        size_t count
    ) :
        ManagedObjectPoolBase<_Object,
            ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>,
            _PoolAlign
            _Allocator>(count)
    {
    }

    template<typename _Object, size_t _PoolAlign, _typename _Allocator>
    ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>::ManagedObjectPool
    (
        ManagedObjectPool&& other
    )
    noexcept :
        ManagedObjectPoolBase<_Object,
            ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>,
            _PoolAlign,
            _Allocator>(std::move(other))
    {
    }

    template<typename _Object, size_t _PoolAlign, _typename _Allocator>
    ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>::~ManagedObjectPool()
    {
        destructAll();
    }

    template<typename _Object, size_t _PoolAlign, _typename _Allocator>
    auto ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>::operator=
    (
        ManagedObjectPool&& other
    )
    noexcept -> ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>&
    {

        ManagedObjectPoolBase<_Object, ThisType, _PoolAlign, _Allocator>::operator=(std::move(other));
        return *this;
    }

    template<typename _Object, size_t _PoolAlign, _typename _Allocator>
    void ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>::releaseRecord
    (
        typename ManagedObjectPoolBase<_Object, ThisType, _PoolAlign, _Allocator>::Record *record
    )
    {
        BaseType::releaseRecordInternal(record);
    }

    template<typename _Object, size_t _PoolAlign, _typename _Allocator>
    auto ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>::add(Value&& obj) -> Handle
    {
        return Handle(&BaseType::add(std::forward<Value>(obj))->object);
    }

    template<typename _Object, size_t _PoolAlign, _typename _Allocator>
    auto ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>::add() -> Handle
    {
        return Handle(&BaseType::add()->object);
    }

    template<typename _Object, size_t _PoolAlign, _typename _Allocator>
    void ManagedObjectPool<_Object, void, _PoolAlign, _Allocator>::destructAll()
    {
        //  prevent handle releases from affecting our object teardown
        //  mainly in cases where our Objects contain handles to other Objects
        //  within the pool
        BaseType::setOwnerRef(nullptr);

        //  destroy our entire list.
        //  releaseRecord must call releaseRecordInternal for this to work.
        while (BaseType::_head) {
            releaseRecord(BaseType::_head->prev);
        }

        BaseType::setOwnerRef(this);
    }

}

