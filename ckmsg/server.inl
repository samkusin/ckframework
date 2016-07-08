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

template<typename _DelegateType, typename _Allocator>
Server<_DelegateType, _Allocator>::Server
(
    Messenger<_Allocator>& messenger,
    Endpoint<_Allocator> endpoint
) :
    _messenger(&messenger),
    _endpoint(_messenger->attachEndpoint(std::move(endpoint)))
{
}

template<typename _DelegateType, typename _Allocator>
Server<_DelegateType, _Allocator>::~Server()
{
    _messenger->detachEndpoint(_endpoint);
}


template<typename _DelegateType, typename _Allocator>
void Server<_DelegateType, _Allocator>::on
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

template<typename _DelegateType, typename _Allocator>
Address Server<_DelegateType, _Allocator>::querySenderAddressFromRequestId
(
    ServerRequestId reqId
)
const
{
    auto it = _activeRequests.find(reqId);
    if (it == _activeRequests.end()) {
        return { 0 };
    }
    
    return it->second.adr;
}

template<typename _DelegateType, typename _Allocator>
bool Server<_DelegateType, _Allocator>::receiveOne()
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
            ActiveRequest activeReq { msg.sender(), msg.tagId() };
            _activeRequests.emplace(reqId, activeReq);
            it->second(reqId, payload);
        }
    }
    _messenger->pollEnd(_endpoint, true);

    return (bool)(msg);
}

template<typename _DelegateType, typename _Allocator>
void Server<_DelegateType, _Allocator>::receive()
{
    while (receiveOne())
    {
    }
}

template<typename _DelegateType, typename _Allocator>
void Server<_DelegateType, _Allocator>::reply
(
    ServerRequestId reqId,
    ReplyType replyType,
    const Payload& payload
)
{
    auto it = _activeRequests.find(reqId);
    if (it != _activeRequests.end()) {
        Message msg(_endpoint, reqId.classId);
        if (replyType == ReplyType::kFail) {
            msg.setError();
        }
        msg.setTag(it->second.tag);
        _messenger->send(std::move(msg), it->second.adr, &payload, reqId.seqId);
        _activeRequests.erase(it);
    }
}

template<typename _DelegateType, typename _Allocator>
void Server<_DelegateType, _Allocator>::notify
(
    Address target,
    ClassId classId,
    const Payload& payload
)
{
    Message msg(_endpoint, classId);
    _messenger->send(std::move(msg), target, &payload, kNullSequenceId);
}

template<typename _DelegateType, typename _Allocator>
void Server<_DelegateType, _Allocator>::transmit()
{
    _messenger->transmit(_endpoint);
}


} /* namespace ckmsg */

