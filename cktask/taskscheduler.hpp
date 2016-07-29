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
 * @file    cinek/taskscheduler.hpp
 * @author  Samir Sinha
 * @date    10/29/2014
 * @brief   A Task scheduler for Task objects
 * @copyright Cinekine
 */

#ifndef CINEK_TASKSCHEDULER_HPP
#define CINEK_TASKSCHEDULER_HPP

#include "task.hpp"

#include <cinek/intrusive_list.hpp>
#include <vector>

namespace cinek {

    /**
     *  @class  TaskScheduler
     *  @brief  Manages cooperative execution and the lifecycle of tasks.
     */
    template<typename Allocator>
    class TaskScheduler
    {
        CK_CLASS_NON_COPYABLE(TaskScheduler);

    public:
        using TaskPtr = typename Task<Allocator>::Ptr;
        using TaskType = Task<Allocator>;
        
        /**
         * Constructor
         *
         * @param taskLimit The expected maximum of concurrent tasks running
         * @param allocator An optional allocator
         */
        explicit TaskScheduler(uint32_t taskLimit,
                               const Allocator& allocator=Allocator());
        /**
         * Schedules a Task object for execution.  The scheduler takes ownership
         * of the Task.
         *
         * @param  task Job pointer
         * @return      Handle to the scheduled Task
         */
        TaskId schedule(TaskPtr&& task, void* context=nullptr);
        /**
         * Cancels a scheduled task.
         *
         * @param jobHandle Handle to a scheduled Task.
         */
        void cancel(TaskId taskHandle);
        /**
         * Cancels task by a context pointer.  If nullptr is specified, then
         * all tasks are cancelled.
         *
         * @param   context The context pointer specified during schedule (if
         *                  any.)
         */
        void cancelAll(void* context=nullptr);
        /**
         * Executes tasks currently scheduled
         *
         * @param timeMs Delta time in milliseconds since last update
         */
        void update(uint32_t timeMs);
        /**
         *  Queries whether a task is currently active by handle.
         *
         *  @param  taskHandle  The task handle returned from schedule()
         *  @return True if the task is still active
         */
        bool isActive(TaskId taskHandle);

    private:
        intrusive_list<TaskListNode<Allocator>> _runList;
        std::vector<TaskPtr, std_allocator<TaskPtr, Allocator>> _tasks;
        TaskId _currentHandle;
    };

} /* namespace cinek */


#endif
