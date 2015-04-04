//
//  MessageQueue.cpp
//  Overview
//
//  Created by Samir Sinha on 11/23/14.
//  Copyright (c) 2014 Cinekine. All rights reserved.
//

#include "messagequeue.hpp"

namespace cinek {

MessageQueue::MessageQueue(size_t msgCount, Allocator msgAllocator) :
    _allocator(msgAllocator),
    _messages(_allocator),
    _head(0)
{
    _messages.reserve(msgCount);
}

MessageQueue::MessageQueue(MessageQueue&& other) :
    _allocator(std::move(other._allocator)),
    _messages(std::move(other._messages)),
    _head(other._head)
{
    other._head = other._messages.size();
}

MessageQueue& MessageQueue::operator=(MessageQueue&& other)
{
    _allocator = std::move(other._allocator);
    _messages = std::move(other._messages);
    _head = other._head;
    other._head = other._messages.size();
    return *this;
}

void MessageQueue::push(
    const Message::ClassId& classId,
    const SDO& data,
    Message::SequenceId seqId
)
{
    if (_messages.size() == _messages.capacity() && _head > 0)
    {
        _messages.erase(_messages.begin(), _messages.begin() + _head);
        _head = 0;
    }
    auto clonedData = data.clone(_allocator);
    if (!clonedData)
        return;
    _messages.emplace_back(classId, std::move(clonedData), seqId);
}

void MessageQueue::pop()
{
    if (_head == _messages.size())
        return;
    ++_head;
}

} /* namespace cinek */