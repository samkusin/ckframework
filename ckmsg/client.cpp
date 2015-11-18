//
//  client.inl
//  SampleCommon
//
//  Created by Samir Sinha on 11/16/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#include "client.hpp"
#include "messenger.hpp"

#include <functional>
#include <cassert>

namespace ckmsg {

template<typename _Delegate>
Client<_Delegate>::Client
(
    Messenger& messenger,
    EndpointInitParams initParams
) :
    _messenger(&messenger),
    _endpoint(_messenger->createEndpoint(initParams))
{
    _sequenceDelegates.reserve(64);
    _classDelegates.reserve(64);
}

template<typename _Delegate>
Client<_Delegate>::~Client()
{
    _messenger->destroyEndpoint(_endpoint);
}
    
template<typename _Delegate>
uint32_t Client<_Delegate>::send
(
    Address target,
    ClassId classId,
    _Delegate delegate
)
{
    uint32_t seqId = _messenger->send(_endpoint, target, classId);
    if (seqId && delegate) {
        auto it = std::lower_bound(_sequenceDelegates.begin(), _sequenceDelegates.end(),
            seqId,
            [](const typename decltype(_sequenceDelegates)::value_type& p, uint32_t seqId) -> bool {
                return p.first < seqId;
            });
        //  sanity check, but this assertion should never fail in a real app
        assert(it == _sequenceDelegates.end() || it->first != seqId);
        _sequenceDelegates.emplace(seqId, std::move(delegate));
    }
    return seqId;
}

template<typename _DelegateType>
void Client<_DelegateType>::on(ClassId classId, _DelegateType delegate)
{
    auto it = std::lower_bound(_classDelegates.begin(), _classDelegates.end(),
        classId,
        [](const typename decltype(_classDelegates)::value_type& p, ClassId cid) -> bool {
            return p.first < cid;
        });
    if (it != _classDelegates.end() && it->first == classId) {
        it->second = std::move(delegate);
    }
    else {
        _classDelegates.emplace(it, std::move(delegate));
    }
}

template<typename _Delegate>
void Client<_Delegate>::transmit()
{
    _messenger->transmit(_endpoint);
}


template<typename _Delegate>
void Client<_Delegate>::receive()
{
}



}   /* namespace ckmsg */

template<> class ckmsg::Client<std::function<void(uint32_t, ckmsg::ClassId, const ckmsg::Payload*)>>;