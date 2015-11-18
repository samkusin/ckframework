//
//  server.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 11/16/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#include "server.hpp"
#include "messenger.hpp"

namespace ckmsg {

template<typename _DelegateType>
Server<_DelegateType>::Server
(
    Messenger& messenger,
    EndpointInitParams params
) :
    _messenger(&messenger),
    _endpoint()
{
    _endpoint = messenger.createEndpoint(params);
}

template<typename _DelegateType>
Server<_DelegateType>::~Server()
{
    _messenger->destroyEndpoint(_endpoint);
}


template<typename _DelegateType>
void Server<_DelegateType>::on
(
    ClassId classId,
    _DelegateType delegate
)
{
    auto it = std::lower_bound(_classDelegates.begin(), _classDelegates.end(),
        classId,
        [](const typename decltype(_classDelegates)::value_type& p, ClassId cid) -> bool {
            return p.first < cid;
        });
    _classDelegates.emplace(classId, std::move(delegate));
}

template<typename _DelegateType>
void Server<_DelegateType>::receive()
{
    Message msg;
    Payload payload;
    while ((msg = _messenger->receive(_endpoint, payload))) {
    
    }
}

template<typename _DelegateType>
void Server<_DelegateType>::reply
(
    uint32_t seqId,
    const Payload* payload
)
{
}

template<typename _DelegateType>
void Server<_DelegateType>::transmit()
{
}


} /* namespace ckmsg */

template<> class ckmsg::Server<std::function<void(ckmsg::ServerRequestId, const ckmsg::Payload*)>>;
