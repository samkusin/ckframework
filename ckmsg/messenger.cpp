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


static const size_t kEncodedHeaderSize = 4;
static const uint8_t kEncodedMessageHeader[kEncodedHeaderSize] = { 'm','e','s','g' };

inline void encodeHeader(uint8_t* target, const uint8_t hdr[]) {
    target[0] = hdr[0];
    target[1] = hdr[1];
    target[2] = hdr[2];
    target[3] = hdr[3];
}

inline bool checkHeader(const uint8_t* input, const uint8_t hdr[]) {
    return input[0]==hdr[0] &&
           input[1]==hdr[1] &&
           input[2]==hdr[2] &&
           input[3]==hdr[3];
}

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
    
    //  receiver holds for this endpoint.  perhaps we need to make this
    //  capacity configurable?
    endpoint.senderReceiveHoldList.reserve(32);
    
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
 
    //  todo - buffers that are full, call transmit to make space
    auto& sendPoint = it->second;
    auto& sendBuffer = sendPoint.sendBuffer;
    
    //  verify we can send out the message packet
    uint8_t* packet = sendBuffer.writep(kEncodedHeaderSize +
                                        sizeof(Address) +
                                        sizeof(Message));
    if (!packet)
        return 0;
    
    encodeHeader(packet, kEncodedMessageHeader);
    memcpy(packet + kEncodedHeaderSize, &receiver, sizeof(Address));
    auto outMsg = reinterpret_cast<Message*>(packet + kEncodedHeaderSize + sizeof(Address));
    *outMsg = std::move(msg);
    
    uint8_t* outPayload = nullptr;
    uint32_t payloadSize = payload->size();
    if (payload && payloadSize>0) {
        outPayload = sendBuffer.writep(sizeof(uint32_t) +
                                       payload->size());
        if (!outPayload) {
            sendBuffer.revertWrite();
            return 0;
        }
        outMsg->setFlags(Message::kHasPayload);
        
        memcpy(outPayload, &payloadSize, sizeof(uint32_t));
        memcpy(outPayload+sizeof(uint32_t), payload, payload->size());
    }
    
    if (seqId == kAssignSequenceId) {
        //  reserve 0 and (uint32_t)(-1) (see message.hpp constants.)
        ++sendPoint.thisSeqId;
        if (sendPoint.thisSeqId == kAssignSequenceId)
            sendPoint.thisSeqId = 1;
        seqId = sendPoint.thisSeqId;
    }
    else if (seqId != kNullSequenceId) {
        outMsg->setFlags(Message::kIsReply);
    }
    
    outMsg->setSequenceId(seqId);
    
    //  finish write (note, we could've updated two times, message and payload.)
    //  since we don't yet support transmitting during send to free send space
    //  we want to perform the write in one sequence.
    //
    //  note, this doesn't mean much yet without multithreading.
    //
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
    
    //  clear out receive holds for our sender left over from last frame
    //  receive holds will be populated below if needed
    sendPoint.senderReceiveHoldList.clear();
    
    //  use the encoded header to determine current send/transmit state
    //  our goal is to fill the receive buffer with whole objects
    //  a 'whole' object is defined as a 4-byte encoded header and its
    //  attached data.
    //  this way, we can divide transmitted data into chunks and maximize
    //  memory efficiency - keep all buffers as full as possible.
    //
    //  the receiver will handle incoming data accordingly by packets.
    //
    while (sendBuffer.readSizeContiguous() >= (kEncodedHeaderSize + sizeof(Address))) {
      
        const uint8_t* hdrpacket = sendBuffer.readp(kEncodedHeaderSize);
        const Address& address = *reinterpret_cast<const Address*>(
            sendBuffer.readp(sizeof(Address))
        );
        
        const uint8_t* datapacket = nullptr;
        uint32_t datasize = 0;
        
        if (checkHeader(hdrpacket, kEncodedMessageHeader)) {
            datapacket = sendBuffer.readp(sizeof(Message));
            if (datapacket) {
                const Message& inMsg = *reinterpret_cast<const Message*>(datapacket);
                datasize = sizeof(Message);
            
                if (inMsg.queryFlag(Message::kHasPayload)) {
                    const uint8_t* p = sendBuffer.readp(sizeof(uint32_t));
                    if (p) {
                        uint32_t payloadSize = *reinterpret_cast<const uint32_t*>(p);
                        datasize += sizeof(uint32_t);
                        if (payloadSize) {
                            const uint8_t* payloadData = sendBuffer.readp(payloadSize);
                            if (payloadData) {
                                datasize += payloadSize;
                            }
                            else {
                                datapacket = nullptr;
                            }
                        }
                    }
                    else {
                        datapacket = nullptr;
                    }
                }
            }
        }
        
        //  the while condition verifies that we have a valid header + address
        //  but check edge cases where we don't have any data associated with
        //  the packet.  in such cases, we must drop the packet as our system
        //  requires that packets are submitted in whole to the send buffer.
        //  non-data packets in this case indicate that we're not sending
        //  packets properly.
        
        //  valid datapackets are transmitted to the target address's endpoint
        //  receive buffer.
        bool consumePacket = true;
        if (datapacket) {
            //  we have a whole packet to transmit.  if there's room on the
            //  target's receiver queue, then copy it over.  otherwise we hold
            //  further transmission to this receiver for this remainder of this
            //  transmission pass
            
            if (std::find(sendPoint.senderReceiveHoldList.begin(),
                      sendPoint.senderReceiveHoldList.end(),
                      address.id) == sendPoint.senderReceiveHoldList.end()) {
            
                auto endpRecvIt = _endpoints.find(address.id);
                if (endpRecvIt != _endpoints.end()) {
                    auto& recvPoint = endpRecvIt->second;
                    auto& recvBuffer = recvPoint.recvBuffer;
                    if (recvBuffer.writeSizeContiguous() >= kEncodedHeaderSize + datasize) {
                        uint8_t* hdrout = recvBuffer.writep(kEncodedHeaderSize);
                        encodeHeader(hdrout, hdrpacket);
                        uint8_t* dataout = recvBuffer.writep(datasize);
                        memcpy(dataout, datapacket, datasize);
                        recvBuffer.updateWrite();
                    }
                    else {
                        sendPoint.senderReceiveHoldList.push_back(endpRecvIt->first);
                        consumePacket = false;
                    }
                }
                /*
                else {
                    //  dropped - no endpoint found.
                }
                */
            }
            else {
                consumePacket = false;
            }
        }
        /*
        else {
            //  corrupt data - remove.
        }
        */
        
        if (consumePacket) {
            sendBuffer.updateRead();
        }
    }
}

Message Messenger::pollReceive
(
    Address receiver,
    Payload& payload
)
{
    Message msg;
    
    auto endpIt = _endpoints.find(receiver.id);
    if (endpIt != _endpoints.end()) {
        auto& recvBuffer = endpIt->second.recvBuffer;
        while (recvBuffer.readSizeContiguous() >= kEncodedHeaderSize) {
            
            const uint8_t* hdrpacket = recvBuffer.readp(kEncodedHeaderSize);
            
            if (checkHeader(hdrpacket, kEncodedMessageHeader)) {
                const Message* pkt = reinterpret_cast<const Message*>(
                    recvBuffer.readp(sizeof(Message))
                );
                if (pkt) {
                    const uint8_t* payloadData = nullptr;
                    uint32_t payloadSize = 0;
                    if (pkt->queryFlag(Message::kHasPayload)) {
                        const uint8_t* p = recvBuffer.readp(sizeof(uint32_t));
                        if (p) {
                            payloadSize =  *reinterpret_cast<const uint32_t*>(p);
                            payloadData = recvBuffer.readp(payloadSize);
                        }
                        payload = std::move(Payload(payloadData, payloadSize));
                    }
                    msg = *pkt;
                }
            }
        }
    }
    
    return msg;
}

void Messenger::pollEnd(Address receiver)
{
    auto endpIt = _endpoints.find(receiver.id);
    if (endpIt != _endpoints.end()) {
        endpIt->second.recvBuffer.updateRead();
    }
}

} /* namespace ckmsg */
