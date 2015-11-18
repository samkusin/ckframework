//
//  messenger.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 11/16/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#ifndef CINEK_MSG_MESSENGER_HPP
#define CINEK_MSG_MESSENGER_HPP

#include "message.hpp"
#include "buffer.hpp"

#include <vector>
#include <unordered_map>

namespace ckmsg {
  
struct Allocator
{
    static uint8_t* allocate(size_t sz);
    static void free(uint8_t* p);
};


class Messenger
{
public:
    Messenger();
    Messenger(const Messenger& ) = delete;
    Messenger& operator=(const Messenger& ) = delete;
    
    Address createEndpoint(EndpointInitParams params);
    void destroyEndpoint(Address endp);
    
    uint32_t send(Message&& msg, Address receiver,
                  const Payload* payload,
                  uint32_t seqId=kNullSequenceId);
    
    void transmit(Address sender);
    
    /**
     *  Poll to receive a message packet.  An empty message is returned if
     *  no message is available.  Callers typically call pollReceive in a loop
     *  until a null message is returned.
     *
     *  @param  receiver    The receiver to poll
     *  @param  payload     If there's a payload, this object will be populated.
     *                      The payload remains valid until calling pollEnd.
     *  @return The incoming message.  Can be empty, indicating that the
     *          receiver should stop polling and release the receiver queue via
     *          a pollEnd.
     */
    Message pollReceive(Address receiver, Payload& payload);
    /**
     *  Following a poll loop, call pollEnd to flush to release the receiver
     *  so that it can continue to receive messages.  Also any payloads returned
     *  through pollReceive become undefined.
     *
     *  @param  receiver    End polling for the receiver.
     */
    void pollEnd(Address receiver);

private:
    struct Endpoint
    {
        Buffer<Allocator> sendBuffer;
        Buffer<Allocator> recvBuffer;
        
        uint32_t thisSeqId;
        
        //  if a sender can't send a packet to a receiver, prevent that sender
        //  from sending other queued packets to that receiver until there's
        //  room.
        //  this list is cleared at the end of a transmit.
        std::vector<uint32_t> senderReceiveHoldList;
    };
    
    std::unordered_map<uint32_t, Endpoint> _endpoints;
    uint32_t _thisEndpointId;
};
  
}   /* namespace ckmsg */

#endif /* messenger_hpp */
