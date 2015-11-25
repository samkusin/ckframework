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
    if (it != _classDelegates.end() && it->first == classId) {
        it->second = std::move(delegate);
    }
    else {
        _classDelegates.emplace(it, classId, std::move(delegate));
    }
}

template<typename _DelegateType>
bool Server<_DelegateType>::receive()
{
    Payload payload;
    Message msg = _messenger->pollReceive(_endpoint, payload);
    if (msg)
    {
        auto it = std::lower_bound(_classDelegates.begin(), _classDelegates.end(),
            msg.type(),
            [](const typename decltype(_classDelegates)::value_type& p, ClassId cid) -> bool {
                return p.first < cid;
            });
        if (it != _classDelegates.end() && it->first == msg.type()) {
            //  pass incoming messages to their respective class delegates
            //  register the incoming sequence for replying
            ServerRequestId reqId { msg.sequenceId(), msg.type() };
            _activeRequests.emplace(reqId, msg.sender());
            it->second(reqId, &payload);
        }
    }
    _messenger->pollEnd(_endpoint, true);
    
    return (msg)==true;
}

template<typename _DelegateType>
void Server<_DelegateType>::reply
(
    ServerRequestId reqId,
    const Payload* payload
)
{
    auto it = _activeRequests.find(reqId);
    if (it != _activeRequests.end()) {
        Message msg(_endpoint, reqId.classId);
    
        _messenger->send(std::move(msg), it->second, payload, reqId.seqId);
        _activeRequests.erase(it);
    }
}

template<typename _DelegateType>
void Server<_DelegateType>::transmit()
{
    _messenger->transmit(_endpoint);
}


} /* namespace ckmsg */

