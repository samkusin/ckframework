//
//  endpoint.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 6/17/16.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#ifndef CINEK_MSG_ENDPOINT_HPP
#define CINEK_MSG_ENDPOINT_HPP

#include "message.hpp"
#include "buffer.hpp"

namespace ckmsg {

    struct EndpointBase
    {
        static const uint8_t kEncodedMessageHeader[4];

        void encodeHeader(uint8_t* target, const uint8_t hdr[]);
        bool checkHeader(const uint8_t* input, const uint8_t hdr[]);

        void setFlags(Message& msg, uint16_t mask) { msg.setFlags(mask); }
        void clearFlags(Message& msg, uint16_t mask) { msg.clearFlags(mask); }
        void setSequenceId(Message& msg,uint32_t id) { msg.setSequenceId(id); }
        uint16_t getFlags(Message& msg) const { return msg.getFlags(); }
        uint16_t messageSize() const { return Message::serializeSize(); }
        void serialize(Message& msg, uint8_t* out) { msg.serialize(out); }
        void unserialize(Message& msg, const uint8_t* in, uint16_t sz) {
            msg.unserialize(in, sz);
        }
    };

    template<typename Allocator>
    struct Endpoint : EndpointBase
    {
        Buffer<Allocator> sendBuffer;
        Buffer<Allocator> recvBuffer;

        uint32_t thisSeqId = 0;

        Endpoint() = default;
        Endpoint(uint32_t bufferSize, Allocator allocator);

        uint32_t send(Message&& msg, Address receiver, const Payload* payload,
                      uint32_t seqId);
        Message receive(Payload& payload);
        void receiveEnd(bool consume);
    };

}   /* namespace ckmsg */

#endif