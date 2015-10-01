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
        _recordsPool(count)
    {
    }

    template<typename _Object, typename _Derived>
    ManagedObjectPoolBase<_Object, _Derived>::ManagedObjectPoolBase
    (
        ManagedObjectPoolBase&& other
    )
    noexcept :
        _recordsPool(std::move(other._recordsPool))
    {
        fixupRecords();
    }

    template<typename _Object, typename _Derived>
    auto ManagedObjectPoolBase<_Object, _Derived>::operator=
    (
        ManagedObjectPoolBase&& other
    )
    noexcept -> ManagedObjectPoolBase<_Object, _Derived>&
    {
        _recordsPool = std::move(other._recordsPool);
        
        fixupRecords();
        
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
            record->owner = static_cast<Derived*>(this);
            record->refcnt = 0;
        }
        return record;
    }
  
    template<typename _Object, typename _Derived>
    void ManagedObjectPoolBase<_Object, _Derived>::releaseRecord(Record *record)
    {
        _recordsPool.destruct(record);
    }
    
    template<typename _Object, typename _Derived>
    void ManagedObjectPoolBase<_Object, _Derived>::fixupRecords()
    {
        _recordsPool.forEach(
            [this](Record& rec) {
                rec.owner = static_cast<Derived*>(this);
            });
    }
    
    ////////////////////////////////////////////////////////////////////////////
    
    template<typename _Object, typename _Delegate>
    ManagedObjectPool<_Object, _Delegate>::ManagedObjectPool
    (
        size_t count,
        const _Delegate& del
    ) :
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, _Delegate>>(count),
        _delegate(del)
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
    auto ManagedObjectPool<_Object, _Delegate>::operator=
    (
        ManagedObjectPool&& other
    )
    noexcept -> ManagedObjectPool<_Object, _Delegate>&
    {
        ManagedObjectPoolBase<_Object, ManagedObjectPool<_Object, _Delegate>>::operator=(std::move(other));
        _delegate = std::move(other._delegate);
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
        
        BaseType::releaseRecord(record);
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
        BaseType::releaseRecord(record);
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
    
}

