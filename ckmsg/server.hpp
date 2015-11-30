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
 *  void cb(ServerRequestId reqId, const Payload* payload);
 */
template<typename _DelegateType>
class Server
{
public:
    Server(Messenger& messenger, EndpointInitParams params={256,256});
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
     *  Receives a single message targeted for this client (via localAddress).
     *  Servers should call this method regularly to poll for incoming messages.
     *  This method may invoke message handlers before returning.
     *
     *  @return False if there are no more messages on the receive buffer
     */
    bool receive();
    /**
     *  Reply to an active request.
     *
     *  @param  reqId   The request to reply to.
     *  @param  payload (Optional) The payload included in the reply
     */
    void reply(ServerRequestId reqId, const Payload* payload = nullptr);
    /**
     *  Transmits any pending messages to their targets (flushes the message
     *  queue if it still has messages.)  Invoke once per receive.
     */
    void transmit();
    /**
     *  @return returns the address for this server.
     */
    Address address() const { return _endpoint; }

private:
    Messenger* _messenger;
    Address _endpoint;
    
    std::vector<std::pair<ClassId, _DelegateType>> _classDelegates;
    std::unordered_map<ServerRequestId, Address> _activeRequests;
};

} /* namespace ckmsg */

#endif /* CINEK_MSG_SERVER_HPP */
