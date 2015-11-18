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
                  const uint8_t* payload, uint32_t payloadSize,
                  uint32_t seqId=0);
    
    void transmit(Address sender);
    
    Message receive(Address receiver, Payload& payload);
    
private:
    struct Endpoint
    {
        Buffer<Allocator> sendBuffer;
        Buffer<Allocator> recvBuffer;
        
        uint32_t thisSeqId;
    };
    
    std::unordered_map<uint32_t, Endpoint> _endpoints;
    uint32_t _thisEndpointId;
};
  
}   /* namespace ckmsg */

#endif /* messenger_hpp */
