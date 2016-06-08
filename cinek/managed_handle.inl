//
//  managed_handle.inl
//  GfxPrototype
//
//  Created by Samir Sinha on 9/28/15.
//
//


namespace cinek {

//  These ManagedHandle methods require the ManagedPool definition, so
//  they are placed here to prevent defining ManagedPool prior to
//  ManagedHandle
//
template<typename _HandleValue, typename _HandleOwner>
void ManagedHandle<_HandleValue,_HandleOwner>::acquire()
{
    if (!_resource)
        return;

    auto record = reinterpret_cast<typename _HandleOwner::Record*>(_resource);
    ++record->refcnt;
//    printf("%p refcnt %d\n", _resource, record->refcnt);
    CK_ASSERT(record->refcnt > 0);
}

template<typename _HandleValue, typename _HandleOwner>
void ManagedHandle<_HandleValue, _HandleOwner>::release()
{
    if (!_resource)
        return;

    auto record = reinterpret_cast<typename _HandleOwner::Record*>(_resource);

    CK_ASSERT_RETURN(record->refcnt > 0);
    --record->refcnt;
//    printf("%p refcnt %d\n", _resource, record->refcnt);
    if (!record->refcnt && record->ownerRef && record->ownerRef->owner) {
        record->ownerRef->owner->releaseRecord(record);
    }
}

}