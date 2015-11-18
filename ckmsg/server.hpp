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

namespace ckmsg {

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
     *  Receives incoming messages targeted for this client (via localAddress).
     *  Clients should call this method regularly to poll for incoming messages.
     *  This method may invoke message handlers before returning.
     */
    void receive();
    /**
     *  Reply to an active request.
     *
     *  @param  seqId   The request to reply to.
     *  @param  payload (Optional) The payload included in the reply
     */
    void reply(uint32_t seqId, const Payload* payload = nullptr);
    /**
     *  Transmits any pending messages to their targets (flushes the message
     *  queue if it still has messages.)  Invoke once per receive.
     */
    void transmit();

private:
    Messenger* _messenger;
    Address _endpoint;
    
    std::vector<std::pair<ClassId, _DelegateType>> _classDelegates;
    std::unordered_map<uint32_t, ClassId> _activeRequests;
};

} /* namespace ckmsg */

#endif /* CINEK_MSG_SERVER_HPP */
