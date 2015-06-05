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

#include "cinek/task.hpp"
#include "cinek/vector.hpp"
#include "cinek/intrusive_list.hpp"

namespace cinek {

    /**
     *  @class  TaskScheduler
     *  @brief  Manages cooperative execution and the lifecycle of tasks.
     */
    class TaskScheduler
    {
    public:
        /**
         * Constructor
         *
         * @param taskLimit The expected maximum of concurrent tasks running
         * @param allocator An optional allocator
         */
        TaskScheduler(uint32_t taskLimit,
                      const Allocator& allocator=Allocator());
        /**
         * Schedules a Task object for execution.  The scheduler takes ownership
         * of the Task.
         *
         * @param  task Job pointer
         * @return      Handle to the scheduled Task
         */
        TaskHandle schedule(unique_ptr<Task>&& task);
        /**
         * Cancels a scheduled task.
         *
         * @param jobHandle Handle to a scheduled Task.
         */
        void cancel(TaskHandle taskHandle);
        /**
         * Cancel all scheduled tasks.
         */
        void cancelAll();
        /**
         * Executes tasks currently scheduled
         *
         * @param timeMs Process time
         */
        void update(uint32_t timeMs);

    private:
        using TaskPair = std::pair<TaskHandle, unique_ptr<Task>>;
        friend bool operator<(const TaskPair& l, const TaskPair::first_type& r);
        friend bool operator<(const TaskPair::first_type& l, const TaskPair& r);
        intrusive_list<TaskListNode> _runList;
        vector<TaskPair> _tasks;
        TaskHandle _currentHandle;
    };

} /* namespace cinek */


#endif
