//
//  MessageQueue.hpp
//  Overview
//
//  Created by Samir Sinha on 11/23/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#ifndef CK_Core_MessageQueue_hpp
#define CK_Core_MessageQueue_hpp

#include "message.hpp"

#include "cinek/vector.hpp"
#include "cinek/allocator.hpp"

namespace cinek {

class MessageQueue
{
public:
    MessageQueue() = default;
    MessageQueue(size_t msgCount, Allocator msgAllocator);
    MessageQueue(MessageQueue&& other);
    MessageQueue& operator=(MessageQueue&& other);

    void push(const Message::ClassId& classId,
              const SDO& data,
              Message::SequenceId seqId=0);
    bool empty() const { return _messages.size() == _head; }

    Message& front() { return _messages[_head]; }
    const Message& front() const { return _messages[_head]; }
    void pop();

private:
    Allocator _allocator;
    vector<Message> _messages;
    uint32_t _head;
};

} /* namespace cinek */

#endif
