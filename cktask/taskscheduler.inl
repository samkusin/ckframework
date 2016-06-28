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
 * @file    cinek/taskscheduler.cpp
 * @author  Samir Sinha
 * @date    10/29/2014
 * @brief   A "schedule-only" interface for concurrent tasks running on the
 *          same thread.
 * @copyright Cinekine
 */

#include "taskscheduler.hpp"
#include <cinek/debug.h>
#include <algorithm>

namespace cinek {

template<typename Allocator>
bool operator<(const unique_ptr<Task<Allocator>, Allocator>& l, TaskId r)
{
    return l.get()->id() < r;
}

template<typename Allocator>
TaskScheduler<Allocator>::TaskScheduler(uint32_t taskLimit, const Allocator& allocator) :
    _tasks(allocator),
    _currentHandle(0)
{
    _tasks.reserve(taskLimit);
}

template<typename Allocator>
TaskId TaskScheduler<Allocator>::schedule(TaskPtr&& task, void* context)
{
    ++_currentHandle;
    if (!_currentHandle)
        _currentHandle = 1;

    auto it = std::lower_bound(_tasks.begin(), _tasks.end(), _currentHandle);
    if (it != _tasks.end())
    {
        if ((*it).get()->id() == _currentHandle)
        {
            CK_LOG_ERROR("TaskScheduler", "Handle %u already exists in task list!",
                         _currentHandle);
            return 0;
        }
    }
    //  add to our processing list
    _runList.push_back(task.get());
    task->_state = TaskType::State::kStaged;
    task->_schedulerHandle = _currentHandle;
    task->_schedulerContext = context;
    //  place owned reference onto our searchable list
    _tasks.emplace(it, std::move(task));

    return _currentHandle;
}

template<typename Allocator>
void TaskScheduler<Allocator>::cancel(TaskId taskHandle)
{
    if (!taskHandle)
        return;
    auto it = std::lower_bound(_tasks.begin(), _tasks.end(), taskHandle);
    if (it == _tasks.end() || (*it).get()->id() != taskHandle)
        return;

    (*it).get()->cancel();
}

template<typename Allocator>
bool TaskScheduler<Allocator>::isActive(TaskId taskHandle)
{
    if (!taskHandle)
        return false;
    auto it = std::lower_bound(_tasks.begin(), _tasks.end(), taskHandle);
    if (it == _tasks.end() || (*it).get()->id() != taskHandle)
        return false;

    return true;
}

template<typename Allocator>
void TaskScheduler<Allocator>::cancelAll(void* context)
{
    for (auto& tp : _tasks)
    {
        if (!context || tp->_schedulerContext == context) {
            tp->cancel();
        }
    }
}

template<typename Allocator>
void TaskScheduler<Allocator>::update(uint32_t deltaTimeMs)
{
    auto taskIt = _runList.begin();

    while (taskIt != _runList.end())
    {
        TaskType* task = static_cast<TaskType*>(taskIt.ptr());

        CK_ASSERT(task->_state != TaskType::State::kIdle);

        // remember, task actions (begin, end, etc) can alter the task's state
        // so set our default state prior to executing the action when necessary
        // for example, onBegin may call fail(), which should set the task state
        // to fail, overwriting our default 'active' state
        if (task->_state == TaskType::State::kStaged)
        {
            task->_state = TaskType::State::kActive;
            task->onBegin();
        }
        if (task->_state == TaskType::State::kActive)
        {
            task->onUpdate(deltaTimeMs);
        }

        bool killState = true;
        switch (task->_state)
        {
        case TaskType::State::kEnded:
            {
                //  advance to next task in the chain
                task->onEnd();
                auto nextTask = std::move(task->_nextTask);
                if (nextTask)
                {
                    schedule(std::move(nextTask));
                }
            }
            break;
        case TaskType::State::kFailed:
            {
                task->onFail();
            }
            break;
        case TaskType::State::kCanceled:
            {
                task->onCancel();
            }
            break;
        default:
            killState = false;
            break;
        }

        if (killState)
        {
            auto thisTaskIt = taskIt;
            ++taskIt;
            //  remove task from the run list FIRST and then the task store,
            //  where removing from the task store destroys the task itself
            auto handle = task->_schedulerHandle;
            _runList.erase(thisTaskIt);
            //  the task must be within the task list at this point.   if not,
            //  something very wrong has happened with our task lifecycle
            //  assumptions.
            auto it = std::lower_bound(_tasks.begin(), _tasks.end(), handle);
            CK_ASSERT(it != _tasks.end() && (*it).get()->id() == handle);
            _tasks.erase(it);
        }
        else
        {
            ++taskIt;
        }
    }
}


} /* namespace cinek */
