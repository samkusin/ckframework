/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Cinekine Media
 *
 * See License for details
 *
 * @file    sender.hpp
 * @author  Samir Sinha
 * @date    11/16/15
 * @brief   API for sending messages to a Messenger
 * @copyright Cinekine
 */

#ifndef CINEK_MSG_SENDER_HPP
#define CINEK_MSG_SENDER_HPP

#include "message.hpp"

namespace ckmsg {

/**
 *  @class Sender
 *  @brief An interface for sending Message objects.
 *
 *  The Sender is a shallow wrapper for message sending via the Messenger.  It's
 *  purpose is to provide applications an interface to sending messages apart
 *  from the Messenger service.  One or more Sender instances may be obtained
 *  from a common Messenger.
 *
 *  For example, a single thread may use a Sender to send messages, a Receiver
 *  to receive sent messages, and a common Messenger that facilitates sender
 *  to receiver communciation.
 *
 *  This example could be extended to a multi-threaded application, which may
 *  have a single Messenger, but a Sender and Receiver per thread.  Each thread
 *  would queue messages with its own Sender, where some of these messages are
 *  targeted to receivers on other threads.  Then a thread uses a Receiver to
 *  handle messages.
 *
 *  As mentioned before, Sender (and Receiver) objects are shallow wrappers;
 *  they wrap Messenger functionality.
 */
class Sender
{
public:
    
};
    
}   /* namespace ckmsg */


#endif /* CINEK_MSG_SENDER_HPP */
