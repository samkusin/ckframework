/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Cinekine Media
 *
 * See License for details
 *
 * @file    client.hpp
 * @author  Samir Sinha
 * @date    11/16/15
 * @brief   API for sending messages to a Messenger
 * @copyright Cinekine
 */

#ifndef CINEK_MSG_CLIENT_HPP
#define CINEK_MSG_CLIENT_HPP

#include "message.hpp"
#include "endpoint.hpp"

#include <vector>

namespace ckmsg {

/**
 *  @class Client
 *  @brief An interface for sending and receiving Message objects to a Server
 *
 *  The Client implements sending and receiving of Message objects using a
 *  Messenger.
 *
 *  The Client's _Delegate must be a callable object that conforms to the
 *  following signature:
 *
 *  void callback(uint32_t seqId, ClassId classId, const Payload* payload);
 *
 *  operator bool()
 *  support move
 */
template<typename _DelegateType, typename _Allocator>
class Client
{
public:
    Client(Messenger<_Allocator>& messenger, Endpoint<_Allocator> endpoint);
    ~Client();

    /**
     *  Sends a message to the target with an optional callback on response
     *  from the server.
     *
     *  @param  target          The destination for the message
     *  @param  classId         The message class
     *  @param  tag             The message tag (0 if not used for filtering)
     *  @param  delegate        The callback invoked upon response from the
     *                          server of the sent message
     *  @return A sequence ID or 0 (failed)
     */
    uint32_t send(Address target, ClassId classId,
                  TagId tag,
                  _DelegateType delegate=_DelegateType());

    /**
     *  Sends a message with payload to the target with an optional callback
     *  on response from the server.
     *
     *  @param  target          The destination for the message
     *  @param  classId         The message class
     *  @param  tag             The message tag (0 if not used for filtering)
     *  @param  payload         The message payload
     *  @param  delegate        The callback invoked upon response from the
     *                          server of the sent message
     *  @return A sequence ID or 0 (failed)
     */
    uint32_t send(Address target, ClassId classId,
                  TagId tag,
                  const Payload& payload,
                  _DelegateType delegate=_DelegateType());
    /**
     *  Cancels a delegate specified in the send() call.
     *
     *  @param seqId    The requiest/response sequence to cancel
     */
    void cancel(uint32_t seqId);
    /**
     *  Transmits any pending messages to their targets (flushes the message
     *  queue if it still has messages.)  Invoke once per receive.
     */
    void transmit();
    /**
     *  Receives a single message targeted for this client (via localAddress).
     *  Clients should call this method regularly to poll for incoming messages.
     *  This method may invoke message handlers before returning.
     *
     *  @param tag   Filter messages via tag.  Messages not matching the tag
     *               filter are thrown out.  If zero, tag is ignored.
     *  @return      False is the receive buffer is empty
     */
    bool receiveOne(TagId tag=0);
    /**
     *  Receives all messages targeted for this endpoint (via localAddress).
     *  This call will run until all messages have been processed.  It's offered
     *  as a convenience method if an application's expected to process its
     *  entire receive queue in one call.
     *
     *  @param tag   Filter messages via tag.  Messages not matching the tag
     *               filter are thrown out.  If zero, tag is ignored.
     */
    void receive(TagId tag=0);
    /**
     *  Registers a notification handler for the specified class.  Only one
     *  handler is allowed per notifcation.  Calls that specify a class with
     *  an existing handler will replace the old handler with the specified
     *  delegate.  Use remove() to clear the delegate attached to the specified
     *  class.
     *
     *  @param classId  The Notification class.  It's possible to specify a
     *                  response message type instead of a notification.  In
     *                  such cases, both the response delegate (via send) and
     *                  the class delegate are called.
     *
     *  @param delegate The delegate called upon receipt of the incoming
     *                  message with the specified class.
     */
    void on(ClassId classId, _DelegateType delegate);
    /**
     *  @return returns the address for this client.
     */
    Address address() const { return _endpoint; }

private:
    Messenger<_Allocator>* _messenger;
    Address _endpoint;

    std::vector<std::pair<uint32_t, _DelegateType>> _sequenceDelegates;
    std::vector<std::pair<ClassId, _DelegateType>> _classDelegates;
};

}   /* namespace ckmsg */


#endif /* CINEK_MSG_SENDER_HPP */
