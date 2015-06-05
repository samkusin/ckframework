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

#ifndef CINEK_TASK_HPP
#define CINEK_TASK_HPP

#include "cinek/types.hpp"
#include "cinek/allocator.hpp"

namespace cinek {
    class TaskScheduler;
}

namespace cinek {

    using TaskHandle = uint32_t;

    struct TaskListNode
    {
    public:
        TaskListNode(): __prevListNode(), __nextListNode() {}
        //  as a node within a TaskScheduler's intrusive list of current tasks.
        TaskListNode* __prevListNode;
        TaskListNode* __nextListNode;
    };

    /**
     * @class Task
     * @brief A unit of execution managed by the TaskScheduler
     */
    class Task : public TaskListNode
    {
    public:
        Task();
        virtual ~Task() = default;

        /**
         * Attach the task to be executed when this Task successfully completes.
         *
         * @param task The pointer to the next task.  This task will own the
         *             supplied task.
         */
        void setNextTask(unique_ptr<Task>& task);
        /**
         * Cancels execution of a task.  The Task's onCancel() method will be
         * called upon the next scheduler update
         */
        void cancel();

        /** Task States */
        enum class State
        {
            kIdle,      /**< An unattached task.  A task's initial state */
            kStaged,    /**< Claimed by a scheduler, but not yet started */
            kActive,    /**< Task has started and running */
            kEnded,     /**< Task completed as a result of calling end() */
            kFailed,    /**< Task has failed as a result of calling fail() */
            kCanceled   /**< Task was canceled */
        };
        /** @return The task's state.  See State for more information */
        State state() const { return _state; }

    protected:
        /** Executed when the TaskScheduler schedules this task */
        virtual void onBegin() {}
        /** Executed once during a TaskScheduler update */
        virtual void onUpdate(uint32_t deltaTimeMs) = 0;
        /** Executed upon successful completion of a task */
        virtual void onEnd() {}
        /** Executed if the task marks itself for failure */
        virtual void onFail() {}
        /** Executed if the task has been canceled by a scheduler */
        virtual void onCancel() {}

        /**
         * Flags this task as completed, invoking the onEnd() override upon
         * the task's next scheduled execution frame.
         */
        void end();
        /**
         * Flags this task as having failed, invoking the onFail() override upon
         * the task's next scheduled execution frame.  The scheduler will not
         * invoke any subsequent task on this task's chain.
         */
        void fail();

    private:
        friend class TaskScheduler;

        State _state;
        TaskHandle _schedulerHandle;
        unique_ptr<Task> _nextTask;
    };

} /* namespace cinek */


#endif