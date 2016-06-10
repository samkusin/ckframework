/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 Cinekine Media
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * @file    cinek/task.cpp
 * @author  Samir Sinha
 * @date    10/29/2014
 * @brief   A Task execution object
 * @copyright Cinekine
 */

/*
 * An extension of Mike McShaffry's example of Process based execution,
 * substituting 'Task' for 'Process' in a cooperative multitasking system.
 * For details, refer to his book "Game Coding Complete (4th edition)",
 * Chapter 7 ('Controlling the Main Loop')
 */

#include "task.hpp"

namespace cinek {

template<typename Allocator>
Task<Allocator>::Task(EndCallback cb) :
    _state(State::kIdle),
    _schedulerHandle(0),
    _endCb(cb),
    _schedulerContext(nullptr)
{
}

template<typename Allocator>
void Task<Allocator>::setNextTask(unique_ptr<Task>&& task)
{
    _nextTask = std::move(task);
}

template<typename Allocator>
void Task<Allocator>::cancel()
{
    _state = State::kCanceled;
}

template<typename Allocator>
void Task<Allocator>::end()
{
    _state = State::kEnded;
}

template<typename Allocator>
void Task<Allocator>::fail()
{
    _state = State::kFailed;
}

template<typename Allocator>
void Task<Allocator>::onEnd()
{
    if (_endCb) {
        _endCb(State::kEnded, *this, _schedulerContext);
    }
}

template<typename Allocator>
void Task<Allocator>::onFail()
{
    if (_endCb) {
        _endCb(State::kFailed, *this, _schedulerContext);
    }
}

} /* namespace cinek */
