//
//  messenger.cpp
//  SampleCommon
//
//  Created by Samir Sinha on 11/16/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#include "messenger.hpp"

#include <cassert>

namespace ckmsg {

struct MessagePacket
{
    Message msg;
    Address receiver;
};

/*
    The Messenger collects messages into various send queues and dispatches
    them to their target receiver queues.  Helper classes such as 'Client' and
    'Server' specialize messaging behavior through the Messenger Interface.
    
    Some implementation notes:
 
 */
Messenger::Messenger() :
    _thisEndpointId(0)
{
}

Address Messenger::createEndpoint(EndpointInitParams initParams)
{
    Endpoint endpoint {
        Buffer<Allocator>(initParams.sendSize),
        Buffer<Allocator>(initParams.recvSize),
        0
    };
    
    Address result { 0 };
    {
        ++_thisEndpointId;
        if (!_thisEndpointId)
            _thisEndpointId = 1;         // unlikely wraparound!
        
        auto it = _endpoints.emplace(_thisEndpointId, std::move(endpoint)).first;
        if (it != _endpoints.end()) {
            result = { it->first };
        }
    }

    return result;
}

void Messenger::destroyEndpoint(Address endp)
{
    _endpoints.erase(endp.id);
}

uint32_t Messenger::send
(
    Message&& msg,
    Address receiver,
    const Payload* payload,
    uint32_t seqId
)
{
    // create message and add it to the sender queue
    auto it = _endpoints.find(msg.sender().id);
    if (it == _endpoints.end())
        return 0;
    
    auto& sendPoint = it->second;
    auto& sendBuffer = sendPoint.sendBuffer;
    
    //  verify we can send out this payload
    MessagePacket* outMsg = reinterpret_cast<MessagePacket*>(sendBuffer.writep(sizeof(MessagePacket)));
    if (!outMsg)
        return 0;
    
    uint8_t* outPayload = nullptr;
    if (payload && payload->size()>0) {
        outPayload = sendBuffer.writep(payload->size() + sizeof(uint32_t));
        if (!outPayload) {
            sendBuffer.revertWrite();
            return 0;
        }
    }
    
    outMsg->msg = std::move(msg);
    outMsg->receiver = receiver;
    
    if (outPayload) {
        *reinterpret_cast<uint32_t*>(outPayload) = payload->size();
        memcpy(outPayload+sizeof(uint32_t), payload->data(), payload->size());
        outMsg->msg.setFlags(Message::kHasPayload);
    }
    
    if (seqId == 0) {
        ++sendPoint.thisSeqId;
        if (!sendPoint.thisSeqId)
            sendPoint.thisSeqId = 1;
        seqId = sendPoint.thisSeqId;
    }
    else {
        outMsg->msg.setFlags(Message::kIsReply);
    }
    outMsg->msg.setSequenceId(seqId);
    
    sendBuffer.updateWrite();
    
    return seqId;
}

void Messenger::transmit(Address sender)
{
    auto endpIt = _endpoints.find(sender.id);
    if (endpIt == _endpoints.end())
        return;
    
    //  iterate through all messages on the sender's send list and dispatch
    //  to every message's receiver
    auto& sendPoint = endpIt->second;
    auto& sendBuffer = sendPoint.sendBuffer;
    
    while (sendBuffer.readSizeContiguous() >= sizeof(Message)) {
        //  read all message and attached data.  if there's a valid message
        //  there will be valid attachement data, so checking for message
        //  objects by itself should be enough to indicate whether there
        //  are messages on the send buffer
        
        const MessagePacket* inMsg = reinterpret_cast<const MessagePacket*>(
            sendBuffer.readp(sizeof(MessagePacket))
        );
        const uint8_t* payloadData = nullptr;
        uint32_t payloadSize = 0;
        if (inMsg->msg.queryFlag(Message::kHasPayload)) {
            payloadSize =  *reinterpret_cast<const uint32_t*>(
                                sendBuffer.readp(sizeof(uint32_t))
                            );
            payloadData = sendBuffer.readp(payloadSize);
        }
        
        sendBuffer.updateRead();
        
        //  output to endpoint
        auto endpRecvIt = _endpoints.find(inMsg->receiver.id);
        if (endpRecvIt != _endpoints.end()) {
            auto& recvPoint = endpRecvIt->second;
            
            Message* outMsg = reinterpret_cast<Message*>(
                recvPoint.recvBuffer.writep(sizeof(Message))
            );
            if (outMsg && payloadData) {
                uint8_t* outPayload =
                    recvPoint.recvBuffer.writep(payloadSize + sizeof(uint32_t));
                if (outPayload) {
                    *reinterpret_cast<uint32_t*>(outPayload) = payloadSize;
                    memcpy(outPayload + sizeof(uint32_t), payloadData, payloadSize - sizeof(uint32_t));
                }
                else {
                    outMsg = nullptr;   // no room, dump message
                }
            }
            if (outMsg) {
                *outMsg = inMsg->msg;
                recvPoint.recvBuffer.updateWrite();
            }
            else {
                assert(outMsg);
                recvPoint.recvBuffer.revertWrite();
            }
        }
        
    }
}

Message Messenger::receive
(
    Address receiver,
    Payload& payload
)
{
    Message msg;
    
    
    
    return msg;
}

} /* namespace ckmsg */
