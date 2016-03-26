/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Cinekine Media
 *
 * See License for details
 *
 * @file    message.hpp
 * @author  Samir Sinha
 * @date    11/16/15
 * @brief   The basic header for messaging, containing the mnessage object.
 * @copyright Cinekine
 */

#ifndef CINEK_MSG_HPP
#define CINEK_MSG_HPP

#include <cinek/ckdefs.h>
#include <cstddef>
#include <cstdint>
#include <cassert>

namespace ckmsg {
    
class Messenger;
template<typename _DelegateType> class Client;
template<typename _DelegateType> class Server;
    
/** The target of a message, which is a registered address from the Messenger */
struct Address
{
    uint32_t id;
};

/** Identifies the message */
using ClassId = uint32_t;

/** Parameters for initializing an endpoint */
struct EndpointInitParams
{
    uint32_t sendSize;
    uint32_t recvSize;
};


const uint32_t kNullSequenceId = 0;
const uint32_t kAssignSequenceId = 0xffffffff;

/**
 *  @class Payload
 *  @brief Defines a packet of data associated with messages
 *
 *  Payload is another word for data associated with a message.  Payloads are
 *  seralized by the message sender, and are unserialized by receivers.  Method
 *  of serialization depends on the system using Messenger.
 */
class Payload
{
public:
    Payload() : _data(nullptr), _size(0) {}
    Payload(const uint8_t* data, uint32_t size) :
        _data(data), _size(size) {}
    
    const uint8_t* data() const { return _data; }
    uint32_t size() const { return _size; }
    
private:
    const uint8_t* _data;
    uint32_t _size;
};

/**
 *  @class Message
 *  @brief Messages are the basic unit of data managed by the Messenger.
 *
 *  Messages are transmitted from one endpoint to another.  Messenger ties the
 *  Message to metadata used for delivery.  Applications only need to work with
 *  Message objects and Payloads and let Messenger handle the legwork.
 */
class Message
{
public:
    enum Flags
    {
        /** This is a reply message (reusing the request sequence ID) */
        kIsReply            = (1 << 0),
        /** Includes payload */
        kHasPayload         = (1 << 1),
        /** A large payload (for Messenger handling) */
        kLargePayload       = (1 << 2) | kHasPayload,
        /** Message is an error msg */
        kErrorFlag          = (1 << 15)
    };
    
    Message() : _classId(0), _flags(0) {}
    Message(Address sender, ClassId classId) :
        _classId(classId),
        _sender(sender),
        _seqId(0),
        _customFlags(0),
        _flags(0)
    {
    }

    explicit operator bool() const { return _classId != 0; }

    ClassId type() const {  return _classId; }
    Address sender() const { return _sender; }
    uint32_t sequenceId() const { return _seqId; }
    bool queryFlag(uint16_t mask) const { return (_flags & mask) != 0; }
    bool queryCustomFlags(uint16_t mask) const { return (_customFlags & mask) != 0; }
    void setCustomFlags(uint16_t mask) { _customFlags |= mask; }
    void clearCustomFlags(uint16_t mask) { _customFlags &= ~mask; }
    
private:
    friend class Messenger;
    
    void setFlags(uint16_t mask) { _flags |= mask; }
    void clearFlags(uint16_t mask) { _flags &= ~mask; }
    void setSequenceId(uint32_t id) { _seqId = id; }
    
    ClassId _classId;
    Address _sender;
    uint32_t _seqId;
    uint16_t _customFlags;
    uint16_t _flags;
};

/**
 *  Identifies a request tracker.  
 *  
 *  A Server's main mission is to reply to client requests.  Incoming requests
 *  must always be replied to.  Because requests might take some time to finish,
 *  responses are often delayed until the request has completed processing.
 *  
 *  In an asynchronous server, we'd then need to track requests, so that 
 *  repsonses contain the correct response class and sequence ID.
 */
struct ServerRequestId
{
    uint32_t seqId;
    ClassId classId;

    bool operator==(const ServerRequestId& other) const {
        return seqId == other.seqId && classId == other.classId;
    }
};

}   /* namespace ckmsg */


#endif /* CINEK_MSG_HPP */
