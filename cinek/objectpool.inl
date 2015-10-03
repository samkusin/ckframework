//
//  objectpool.inl
//  GfxPrototype
//
//  Created by Samir Sinha on 9/26/15.
//
//

namespace cinek {

    template<typename _Object, typename _Derived>
    ManagedObjectPoolBase<_Object, _Derived>::ManagedObjectPoolBase(size_t count) :
        _recordsPool(count),
        _head(nullptr),
        _ownerRef(nullptr)
    {
        Allocator allocator;
        _ownerRef = reinterpret_cast<OwnerRef*>(allocator.alloc(sizeof(OwnerRef)));
        _ownerRef->owner = static_cast<_Derived*>(this);
    }

    template<typename _Object, typename _Derived>
    ManagedObjectPoolBase<_Object, _Derived>::ManagedObjectPoolBase
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
    
    template<typename _Object, typename _Derived>
    ManagedObjectPoolBase<_Object, _Derived>::~ManagedObjectPoolBase()
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
    
    template<typename _Object, typename _Derived>
    auto ManagedObjectPoolBase<_Object, _Derived>::operator=
    (
        ManagedObjectPoolBase&& other
    )
    noexcept -> ManagedObjectPoolBase<_Object, _Derived>&
    {
        _recordsPool = std::move(other._recordsPool);
        _head = other._head;
        _ownerRef = other._ownerRef;
        
        other._head = nullptr;
        other._ownerRef = nullptr;
    
        setOwnerRef(static_cast<_Derived*>(this));
        
        return *this;
    }

    template<typename _Object, typename _Derived>
    auto ManagedObjectPoolBase<_Object, _Derived>::add(Value&& obj) -> Record*
    {
        Record* r = add();
        if (r) {
            r->object = std::move(obj);
        }
        return r;
    }
    
    template<typename _Object, typename _Derived>
    auto ManagedObjectPoolBase<_Object, _Derived>::add() -> Record*
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
  
    template<typename _Object, typename _Derived>
    void ManagedObjectPoolBase<_Object, _Derived>::releaseRecordInternal(Record *record)
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
    
    template<typename _Object, typename _Delegate>
    ManagedObjectPool<_Object, _Delegate>::ManagedObjectPool() :
        _delegate()
    {
    }
    
    template<typename _Object, typename _Delegate>
    ManagedObjectPool<_Object, _Delegate>::ManagedObjectPool
    (
        size_t count
    ) :
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, _Delegate>>(count),
        _delegate()
    {
    }
    
    template<typename _Object, typename _Delegate>
    ManagedObjectPool<_Object, _Delegate>::ManagedObjectPool
    (
        ManagedObjectPool&& other
    )
    noexcept :
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, _Delegate>>(std::move(other)),
        _delegate(std::move(other._delegate))
    {
        other._delegate = nullptr;
    }
    
    template<typename _Object, typename _Delegate>
    ManagedObjectPool<_Object, _Delegate>::~ManagedObjectPool()
    {
        destructAll();
    }
    
    template<typename _Object, typename _Delegate>
    auto ManagedObjectPool<_Object, _Delegate>::operator=
    (
        ManagedObjectPool&& other
    )
    noexcept -> ManagedObjectPool<_Object, _Delegate>&
    {
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, _Delegate>>::operator=(std::move(other));
        _delegate = std::move(other._delegate);
        other._delegate = nullptr;
        return *this;
    }
    
    template<typename _Object, typename _Delegate>
    void ManagedObjectPool<_Object, _Delegate>::releaseRecord
    (
        typename BaseType::Record *record
    )
    {
        if (_delegate) {
            _delegate->onReleaseManagedObject(record->object);
        }
        
        BaseType::releaseRecordInternal(record);
    }
    
    template<typename _Object, typename _Delegate>
    void ManagedObjectPool<_Object, _Delegate>::setDelegate(const _Delegate& del)
    {
        _delegate = del;
    }
    
    template<typename _Object, typename _Delegate>
    void ManagedObjectPool<_Object, _Delegate>::clearDelegate()
    {
        _delegate = nullptr;
    }
    
    template<typename _Object, typename _Delegate>
    auto ManagedObjectPool<_Object, _Delegate>::add(Value&& obj) -> Handle
    {
        return Handle(&BaseType::add(std::forward<Value>(obj))->object);
    }
    
    template<typename _Object, typename _Delegate>
    auto ManagedObjectPool<_Object, _Delegate>::add() -> Handle
    {
        return Handle(&BaseType::add()->object);
    }
    
    template<typename _Object, typename _Derived>
    void ManagedObjectPool<_Object, _Derived>::destructAll()
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
    
    template<typename _Object, typename _Derived>
    void ManagedObjectPoolBase<_Object, _Derived>::setOwnerRef(_Derived* owner)
    {
        if (_ownerRef) {
            _ownerRef->owner = owner;
        }
    }

    
    ////////////////////////////////////////////////////////////////////////////
    
    template<typename _Object>
    ManagedObjectPool<_Object, void>::ManagedObjectPool
    (
        size_t count
    ) :
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, void>>(count)    {
    }
    
    template<typename _Object>
    ManagedObjectPool<_Object, void>::ManagedObjectPool
    (
        ManagedObjectPool&& other
    )
    noexcept :
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, void>>(std::move(other))
    {
    }
    
    template<typename _Object>
    ManagedObjectPool<_Object, void>::~ManagedObjectPool()
    {
        destructAll();
    }
    
    template<typename _Object>
    auto ManagedObjectPool<_Object, void>::operator=
    (
        ManagedObjectPool&& other
    )
    noexcept -> ManagedObjectPool<_Object, void>&
    {
     
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, void>>::operator=(std::move(other));
        return *this;
    }
    
    template<typename _Object>
    void ManagedObjectPool<_Object, void>::releaseRecord
    (
        typename BaseType::Record *record
    )
    {
        BaseType::releaseRecordInternal(record);
    }
    
    template<typename _Object>
    auto ManagedObjectPool<_Object, void>::add(Value&& obj) -> Handle
    {
        return Handle(&BaseType::add(std::forward<Value>(obj))->object);
    }
    
    template<typename _Object>
    auto ManagedObjectPool<_Object, void>::add() -> Handle
    {
        return Handle(&BaseType::add()->object);
    }
    
    template<typename _Object>
    void ManagedObjectPool<_Object, void>::destructAll()
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

