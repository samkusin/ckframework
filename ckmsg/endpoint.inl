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
                                            sizeof(Address::id));
        if (!packet)
            return 0;

        encodeHeader(packet, kEncodedMessageHeader);
        *reinterpret_cast<decltype(Address::id)*>(packet + kEncodedHeaderSize) =
            htobe32(receiver.id);
        
        auto msgSize = messageSize();
        uint8_t* outMsg = sendBuffer.writep(2 * sizeof(uint16_t) + msgSize);
        if (!outMsg) {
            sendBuffer.revertWrite();
            return 0;
        }
        if (payload && payload->size()>0) {
            setFlags(msg, Message::kHasPayload);
        }
        
        if (seqId == kAssignSequenceId) {
            //  reserve 0 and (uint32_t)(-1) (see message.hpp constants.)
            ++thisSeqId;
            if (thisSeqId == kAssignSequenceId)
                thisSeqId = 1;
            seqId = thisSeqId;
        }
        else if (seqId != kNullSequenceId) {
            setFlags(msg, Message::kIsReply);
        }
        setSequenceId(msg, seqId);
        //  lower 16 bits for msg size
        //  upper 16 bits for other data
        *reinterpret_cast<uint16_t*>(outMsg) = htobe16(msgSize);
        *reinterpret_cast<uint16_t*>(outMsg + 2) = htobe16(getFlags(msg));
        serialize(msg, outMsg + 4);

        if (payload && payload->size()>0) {
            uint32_t payloadSize = payload->size();
            uint8_t* outPayload = sendBuffer.writep(sizeof(uint32_t)
                                            + sizeof(int16_t)
                                            + sizeof(int16_t));
            if (!outPayload) {
                sendBuffer.revertWrite();
                return 0;
            }
            *reinterpret_cast<uint32_t*>(outPayload) = htobe32(payloadSize);
            *reinterpret_cast<int16_t*>(outPayload + 4) =
                htobe16(static_cast<int16_t>(payload->encoding()));
            *reinterpret_cast<int16_t*>(outPayload + 6) =
                htobe16(static_cast<int16_t>(payload->format()));
            
            outPayload = sendBuffer.writep(payloadSize);
            if (!outPayload) {
                sendBuffer.revertWrite();
                return 0;
            }

            memcpy(outPayload, payload->data(), payloadSize);
        }

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
                //  read message size
                const uint8_t* input = recvBuffer.readp(2 * sizeof(uint16_t));
                if (input) {
                    uint16_t msgSize = be16toh(*reinterpret_cast<const uint16_t*>(input));
                    uint16_t msgFlags = be16toh(*reinterpret_cast<const uint16_t*>(input+2));
                    input = recvBuffer.readp(msgSize);
                    if (input) {
                        setFlags(msg, msgFlags);
                        unserialize(msg, input, msgSize);
                        
                        const uint8_t* payloadData = nullptr;
                        uint32_t payloadSize = 0;
                        PayloadEncoding payloadEnc = PayloadEncoding::kRaw;
                        PayloadFormat payloadFmt = PayloadFormat::kNone;
                        if (msg.queryFlag(Message::kHasPayload)) {
                            const uint8_t* p = recvBuffer.readp(sizeof(uint32_t) +
                                                                2*sizeof(uint16_t));
                            if (p) {
                                payloadSize =  be32toh(*reinterpret_cast<const uint32_t*>(p));
                                payloadEnc = static_cast<PayloadEncoding>(
                                                be16toh(*reinterpret_cast<const uint16_t*>(p + 4))
                                             );
                                payloadFmt = static_cast<PayloadFormat>(
                                                be16toh(*reinterpret_cast<const uint16_t*>(p + 6))
                                             );
                                payloadData = recvBuffer.readp(payloadSize);
                            }
                            payload = Payload(payloadData, payloadSize, payloadEnc, payloadFmt);
                        }
                    }
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
