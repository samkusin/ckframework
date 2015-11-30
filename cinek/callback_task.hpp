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
 * @file    cinek/task.hpp
 * @author  Samir Sinha
 * @date    11/29/2015
 * @brief   A Task execution object
 * @copyright Cinekine
 */

#ifndef CINEK_CALLBACK_TASK_HPP
#define CINEK_CALLBACK_TASK_HPP

#include "cinek/task.hpp"
#include <functional>

namespace cinek {

    /**
     * @class CallbackTask
     * @brief A unit of execution managed by the TaskScheduler
     */
    template<typename _TaskName>
    class CallbackTask : public Task
    {
    public:
        CallbackTask(std::function<void(Task::State, _TaskName&)> cb) :
            _cb(std::move(cb))
        {
        }

    protected:
        virtual void onFail() override {
            if (_cb) {
                _cb(Task::State::kFailed, *static_cast<_TaskName*>(this));
            }
            Task::onFail();
        }
        virtual void onEnd(Task* next) override {
            if (_cb) {
                _cb(Task::State::kEnded, *static_cast<_TaskName*>(this));
            }
            
            Task::onEnd(next);
        }
        
        std::function<void(Task::State, _TaskName&)> _cb;
    };

} /* namespace cinek */


#endif