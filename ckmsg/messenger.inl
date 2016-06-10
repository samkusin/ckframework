//
//  messenger.inl
//  SampleCommon
//
//  Created by Samir Sinha on 11/16/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#include "messenger.hpp"

#include <cassert>
#include <cstdlib>

namespace ckmsg {
/*
    The Messenger collects messages into various send queues and dispatches
    them to their target receiver queues.  Helper classes such as 'Client' and
    'Server' specialize messaging behavior through the Messenger Interface.

    Some implementation notes:

 */
template<typename Allocator>
Messenger<Allocator>::Messenger(Allocator allocator) :
    _allocator(allocator),
    _thisEndpointId(0)
{
}

template<typename Allocator>
Address Messenger<Allocator>::createEndpoint(EndpointInitParams initParams)
{
    Endpoint endpoint {
        Buffer<Allocator>(initParams.sendSize, _allocator),
        Buffer<Allocator>(initParams.recvSize, _allocator),
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

template<typename Allocator>
void Messenger<Allocator>::destroyEndpoint(Address endp)
{
    _endpoints.erase(endp.id);
}

template<typename Allocator>
uint32_t Messenger<Allocator>::send
(
    Message&& msg,
    Address receiver,
    const Payload* payload,
    uint32_t seqId
)
{
    constexpr size_t kEncodedHeaderSize = sizeof(kEncodedMessageHeader);
    // create message and add it to the sender queue
    auto it = _endpoints.find(msg.sender().id);
    if (it == _endpoints.end())
        return 0;

    //  todo - buffers that are full, call transmit to make space
    auto& sendPoint = it->second;
    auto& sendBuffer = sendPoint.sendBuffer;

    //  verify we can send out the message packet
    uint8_t* packet = sendBuffer.writep(kEncodedHeaderSize +
                                        sizeof(Address));
    if (!packet)
        return 0;

    encodeHeader(packet, kEncodedMessageHeader);
    memcpy(packet + kEncodedHeaderSize, &receiver, sizeof(Address));

    auto outMsg = reinterpret_cast<Message*>(sendBuffer.writep(sizeof(Message)));
    if (!outMsg) {
        sendBuffer.revertWrite();
        return 0;
    }

    *outMsg = std::move(msg);

    if (payload && payload->size()>0) {
        uint32_t payloadSize = payload->size();
        uint8_t* outPayload = sendBuffer.writep(sizeof(uint32_t));
        if (!outPayload) {
            sendBuffer.revertWrite();
            return 0;
        }
        memcpy(outPayload, &payloadSize, sizeof(uint32_t));
        outPayload = sendBuffer.writep(payloadSize);
        if (!outPayload) {
            sendBuffer.revertWrite();
            return 0;
        }
        setFlags(*outMsg, Message::kHasPayload);

        memcpy(outPayload, payload->data(), payloadSize);
    }

    if (seqId == kAssignSequenceId) {
        //  reserve 0 and (uint32_t)(-1) (see message.hpp constants.)
        ++sendPoint.thisSeqId;
        if (sendPoint.thisSeqId == kAssignSequenceId)
            sendPoint.thisSeqId = 1;
        seqId = sendPoint.thisSeqId;
    }
    else if (seqId != kNullSequenceId) {
        setFlags(*outMsg, Message::kIsReply);
    }

    setSequenceId(*outMsg, seqId);

    //  finish write (note, we could've updated two times, message and payload.)
    //  since we don't yet support transmitting during send to free send space
    //  we want to perform the write in one sequence.
    //
    //  note, this doesn't mean much yet without multithreading.
    //
    sendBuffer.updateWrite();

    return seqId;
}

template<typename Allocator>
void Messenger<Allocator>::transmit(Address sender)
{
    constexpr size_t kEncodedHeaderSize = sizeof(kEncodedMessageHeader);
    auto endpIt = _endpoints.find(sender.id);
    if (endpIt == _endpoints.end())
        return;

    //  iterate through all messages on the sender's send list and dispatch
    //  to every message's receiver

    auto& sendPoint = endpIt->second;
    auto& sendBuffer = sendPoint.sendBuffer;

    //  take send buffer input and copy it to the receiver's output buffer
    //  - confirm we have a valid send buffer packet first (the while guard)
    //  - the remaining logic ingests the send buffer packet based on type
    //      - implementation is a state machine since packet content relies on
    //        the encoded header string.
    //      - missing data results in a corrupt packet, which is thrown out
    //      - a full receive buffer results in an 'out of room' state
    //
    while (sendBuffer.readSizeContiguous(kEncodedHeaderSize + sizeof(Address))) {

        const uint8_t* hdrpacket = sendBuffer.readp(sizeof(kEncodedMessageHeader) + sizeof(Address));
        const Address& address = *reinterpret_cast<const Address*>(
            hdrpacket+kEncodedHeaderSize
        );

        auto endpRecvIt = _endpoints.find(address.id);
        if (endpRecvIt != _endpoints.end()) {
            auto& recvPoint = endpRecvIt->second;
            auto& recvBuffer = recvPoint.recvBuffer;

            enum
            {
                kStartPacket,
                kMessage,
                kMessagePayloadSize,
                kMessagePayload,
                kCompleted,
                kCorrupted,
                kOutOfRoom
            }
            inputState = kStartPacket;

            uint32_t datasize = 0;
            while (inputState < kCompleted) {
                const uint8_t* inp = nullptr;
                uint8_t* outp = nullptr;

                switch (inputState)
                {
                case kStartPacket:
                    inp = hdrpacket;
                    datasize = kEncodedHeaderSize;
                    break;
                case kMessage:
                    datasize = sizeof(Message);
                    break;
                case kMessagePayloadSize:
                    datasize = sizeof(uint32_t);
                    break;
                case kMessagePayload:
                    //  datasize already set in kMessagePayloadSize state
                    break;
                default:
                    assert(false);  // logic error
                    datasize = 0;
                    break;
                }

                if (!inp) {
                    inp = sendBuffer.readp(datasize);
                }
                outp = recvBuffer.writep(datasize);
                if (!inp) {
                    inputState = kCorrupted;
                }
                else if (!outp) {
                    inputState = kOutOfRoom;
                }
                else {
                    memcpy(outp, inp, datasize);
                }

                switch (inputState)
                {
                case kStartPacket:
                    if (checkHeader(inp, kEncodedMessageHeader)) {
                        inputState = kMessage;
                    }
                    else {
                        inputState = kCorrupted;
                    }
                    break;
                case kMessage: {
                        auto& msg = *reinterpret_cast<const Message*>(inp);
                        if (msg.queryFlag(Message::kHasPayload)) {
                            inputState = kMessagePayloadSize;
                        }
                        else {
                            inputState = kCompleted;
                        }
                    }
                    break;
                case kMessagePayloadSize: {
                        datasize = *reinterpret_cast<const uint32_t*>(inp);
                        inputState = kMessagePayload;
                    }
                    break;
                case kMessagePayload: {
                        inputState = kCompleted;
                    }
                    break;
                default:
                    break;
                }
            }   //  end packet state machine

            if (inputState == kOutOfRoom) {
                //  Full receive buffer.  since our sender must publish packets
                //  in-order, we can't transmit until there's room.
                //
                //  Buffers store packets sequentially and we can't skip packets
                //  in the buffer to continue reading.)
                recvBuffer.revertWrite();
                sendBuffer.revertRead();
                return;
            }
            else if (inputState == kCorrupted) {
                //  input packet is incomplete - likely due to implementation
                //  errors inside send().  we'll continue to ingest our send
                //  buffer, but dump any contents sent to the receiver.
                recvBuffer.revertWrite();
                sendBuffer.updateRead();
            }
            else if (inputState == kCompleted) {
                recvBuffer.updateWrite();
                sendBuffer.updateRead();
            }
            else {
                //  this shouldn't happen unless there's an implementation
                //  error above
                assert(false);
            }
        }
    }
}

template<typename Allocator>
Message Messenger<Allocator>::pollReceive
(
    Address receiver,
    Payload& payload
)
{
    Message msg;
    constexpr size_t kEncodedHeaderSize = sizeof(kEncodedMessageHeader);

    auto endpIt = _endpoints.find(receiver.id);
    if (endpIt != _endpoints.end()) {
        auto& recvBuffer = endpIt->second.recvBuffer;
        if (recvBuffer.readSizeContiguous(kEncodedHeaderSize)) {
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
                        payload = Payload(payloadData, payloadSize);
                    }
                    msg = *pkt;
                }
            }
        }
    }

    return msg;
}

template<typename Allocator>
void Messenger<Allocator>::pollEnd(Address receiver, bool consume)
{
    auto endpIt = _endpoints.find(receiver.id);
    if (endpIt != _endpoints.end()) {
        if (consume) {
            endpIt->second.recvBuffer.updateRead();
        }
        else {
            endpIt->second.recvBuffer.revertRead();
        }
    }
}

} /* namespace ckmsg */
