//
//  endpoint.inl
//  SampleCommon
//
//  Created by Samir Sinha on 6/17/16.
//  Copyright © 2016 Cinekine. All rights reserved.
//

#include "endpoint.hpp"

#include <cstdlib>

namespace ckmsg {

    template<typename Allocator>
    Endpoint<Allocator>::Endpoint(uint32_t bufsize, Allocator allocator) :
        sendBuffer(bufsize, allocator),
        recvBuffer(bufsize, allocator),
        thisSeqId(0)
    {

    }

    template<typename Allocator>
    uint32_t Endpoint<Allocator>::send
    (
        Message&& msg,
        Address receiver,
        const Payload* payload,
        uint32_t seqId
    )
    {
        constexpr size_t kEncodedHeaderSize = sizeof(kEncodedMessageHeader);
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
            ++thisSeqId;
            if (thisSeqId == kAssignSequenceId)
                thisSeqId = 1;
            seqId = thisSeqId;
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
    Message Endpoint<Allocator>::receive(Payload& payload)
    {
        Message msg;
        constexpr size_t kEncodedHeaderSize = sizeof(kEncodedMessageHeader);
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
        
        return msg;
    }

    template<typename Allocator>
    void Endpoint<Allocator>::receiveEnd(bool consume)
    {
        if (consume) {
            recvBuffer.updateRead();
        }
        else {
            recvBuffer.revertRead();
        }
    }

}   /* namespace ckmsg */
