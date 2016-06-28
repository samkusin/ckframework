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

template<typename _Delegate, typename _Allocator>
Client<_Delegate, _Allocator>::Client
(
    Messenger<_Allocator>& messenger,
    Endpoint<_Allocator> endpoint
) :
    _messenger(&messenger),
    _endpoint(_messenger->attachEndpoint(std::move(endpoint)))
{
    _sequenceDelegates.reserve(32);
    _classDelegates.reserve(32);
}

template<typename _Delegate, typename _Allocator>
Client<_Delegate, _Allocator>::~Client()
{
    _messenger->detachEndpoint(_endpoint);
}

template<typename _Delegate, typename _Allocator>
uint32_t Client<_Delegate, _Allocator>::send
(
    Address target,
    ClassId classId,
    TagId tag,
    _Delegate delegate
)
{
    return send(target, classId, tag, Payload(), delegate);
}

template<typename _Delegate, typename _Allocator>
uint32_t Client<_Delegate, _Allocator>::send
(
    Address target,
    ClassId classId,
    TagId tag,
    const Payload& payload,
    _Delegate delegate
)
{
    Message msg(_endpoint, classId);
    msg.setTag(tag);
    uint32_t seqId = _messenger->send(std::move(msg), target, &payload, kAssignSequenceId);
    if (seqId && delegate) {
        auto it = std::lower_bound(_sequenceDelegates.begin(), _sequenceDelegates.end(),
            seqId,
            [](const typename decltype(_sequenceDelegates)::value_type& p, uint32_t seqId) -> bool {
                return p.first < seqId;
            });
        //  sanity check, but this assertion should never fail in a real app
        assert(it == _sequenceDelegates.end() || it->first != seqId);
        _sequenceDelegates.emplace(it, seqId, std::move(delegate));
    }
    return seqId;
}

template<typename _DelegateType, typename _Allocator>
void Client<_DelegateType, _Allocator>::on(ClassId classId, _DelegateType delegate)
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

template<typename _Delegate, typename _Allocator>
void Client<_Delegate, _Allocator>::transmit()
{
    _messenger->transmit(_endpoint);
}

template<typename _Delegate, typename _Allocator>
bool Client<_Delegate, _Allocator>::receiveOne(TagId tag)
{
    Payload payload;
    Message msg = _messenger->pollReceive(_endpoint, payload);
    //  filter by tag if a tag is specified.  
    if (msg)
    {
        bool runDelegate = (!tag || (msg.tagId() && tag == msg.tagId()));
        //  if a reply, check sequence delegates
        if (msg.queryFlag(Message::kIsReply)) {
            auto it = std::lower_bound(_sequenceDelegates.begin(), _sequenceDelegates.end(),
                msg.sequenceId(),
                [](const typename decltype(_sequenceDelegates)::value_type& p, uint32_t seqId) -> bool {
                    return p.first < seqId;
                });
            if (it != _sequenceDelegates.end() && it->first == msg.sequenceId()) {
                if (runDelegate) {
                    it->second(msg, payload);
                }
                _sequenceDelegates.erase(it);
            }
        }
        else {
            auto it = std::lower_bound(_classDelegates.begin(), _classDelegates.end(),
                msg.type(),
                [](const typename decltype(_classDelegates)::value_type& p, ClassId cid) -> bool {
                    return p.first < cid;
                });
            if (it != _classDelegates.end() && it->first == msg.type()) {
                //  pass incoming messages to their respective class delegates
                //  register the incoming sequence for replying
                if (runDelegate) {
                    it->second(msg, payload);
                }
            }
        }
    }
    _messenger->pollEnd(_endpoint, true);

    return (bool)msg;
}


template<typename _DelegateType, typename _Allocator>
void Client<_DelegateType, _Allocator>::receive(TagId tag)
{
    while (receiveOne(tag)) {}
}

}   /* namespace ckmsg */

