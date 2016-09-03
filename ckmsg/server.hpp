//
//  server.hpp
//  SampleCommon
//
//  Created by Samir Sinha on 11/16/15.
//  Copyright © 2015 Cinekine. All rights reserved.
//

#ifndef CINEK_MSG_SERVER_HPP
#define CINEK_MSG_SERVER_HPP

#include "message.hpp"
#include "endpoint.hpp"

#include <vector>
#include <unordered_map>

namespace std {

template<> struct hash<ckmsg::ServerRequestId>
{
    size_t operator()(const ckmsg::ServerRequestId& r) const
    {
        //  collisions with classId are extremely unlikely, so just use
        //  our seqId as the hash
        return std::hash<uint32_t>()(r.seqId);
    }
};

}

namespace ckmsg {

enum class ServerReplyType
{
    kSuccess,
    kFail
};

/**
 *  @class Server
 *  @brief An interface for receiving and replying to Message objects, the
 *         function of a server.
 *
 *  The Server receives requests from a Client, sends responses and events to
 *  subscribers.
 *
 *  The Server's _Delegate must be a callable object that conforms to the
 *  following signature:
 *
 *  void cb(ServerRequestId reqId, const Payload& payload);
 */
template<typename _DelegateType, typename _Allocator>
class Server
{
public:
    using ReplyType = ServerReplyType;
    
    Server(Messenger<_Allocator>& messenger, Endpoint<_Allocator> endpoint);
    ~Server();
    
    /**
     *  Registers a command handler for the specified class.  Only one
     *  handler is allowed per notifcation.  Calls that specify a class with
     *  an existing handler will replace the old handler with the specified
     *  delegate.  Use remove() to clear the delegate attached to the specified
     *  class.
     *
     *  @param classId  The Request class to subscribe to
     *
     *  @param delegate The delegate called upon receipt of the incoming
     *                  message with the specified class
     */
    void on(ClassId classId, _DelegateType delegate);
    /**
     *  Clears the message handler for the specified class.
     *
     *  @param classId  The Request class to unsubscribe from
     */
    void clear(ClassId classId);
    /**
     *  Receives a single message targeted for this client (via localAddress).
     *  Servers should call this method regularly to poll for incoming messages.
     *  This method may invoke message handlers before returning.
     *
     *  @return False if there are no more messages on the receive buffer
     */
    bool receiveOne();
    /**
     *  Receives all messages targeted for this endpoint (via localAddress).
     *  This call will run until all messages have been processed.  It's offered
     *  as a convenience method if an application's expected to process its
     *  entire receive queue in one call.
     */
    void receive();
    /**
     *  Reply to an active request.
     *
     *  @param  reqId   The request to reply to.
     *  @param  type    Type of reply (kSuccess or kFail)
     *  @param  payload The payload included in the reply
     */
    void reply(ServerRequestId reqId, ReplyType type, const Payload& payload);
    /**
     *  Notify a target of an event.
     *
     *  @param  target          The destination for the message
     *  @param  classId         The message class
     *  @param  payload         The message payload
     */
    void notify(Address target, ClassId classId, const Payload& payload);
    /**
     *  Transmits any pending messages to their targets (flushes the message
     *  queue if it still has messages.)  Invoke once per receive.
     */
    void transmit();
    /**
     *  @return returns the address for this server.
     */
    Address address() const { return _endpoint; }
    /**
     *  @param  reqId  The ServerRequestId acquired from an incoming message
     *                 handler.
     *  @return The sender address
     */
    Address querySenderAddressFromRequestId(ServerRequestId reqId) const;

private:
    Messenger<_Allocator>* _messenger;
    Address _endpoint;

    std::vector<std::pair<ClassId, _DelegateType>> _classDelegates;
    struct ActiveRequest {
        Address adr;
        TagId tag;
    };
    std::unordered_map<ServerRequestId, ActiveRequest> _activeRequests;
};

} /* namespace ckmsg */

#endif /* CINEK_MSG_SERVER_HPP */
