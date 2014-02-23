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
 * @file    cinek/framework/jobqueue.hpp
 * @author  Samir Sinha
 * @date    2/17/2014
 * @brief   A job housekeeping and dispatcher
 * @copyright Cinekine
 */

#ifndef CK_FRAMEWORK_JOBQUEUE_HPP
#define CK_FRAMEWORK_JOBQUEUE_HPP

#include <cinek/framework/allocator.hpp>
#include <cinek/framework/job.hpp>
#include <cinek/framework/vector.hpp>

namespace cinekine {

    /**
     * @class JobQueue
     * @brief Handles dispatch of scheduled Jobs
     */
    class JobQueue
    {
    public:
        /**
         * @param queueLimit Number of queue entries to reserve on
         *                   initialization
         * @param context    A context pointer passed along to Jobs managed by
         *                   this queue (see Job::execute)
         * @param allocator  An (optional) allocator for custom memory
         *                   management
         */
        JobQueue(size_t queueLimit,
                 void* context,
                 const Allocator& allocator=Allocator());
        /**
         * Schedules a Job object for execution based on priority.  The queue
         * dispatches the job as soon as it can, against other jobs
         * @param  job      Job pointer
         * @return          Handle to the scheduled job
         */
        JobHandle schedule(unique_ptr<Job>&& job);
        /**
         * Cancels a scheduled job.  Note this does not affect currently
         * running jobs, only queued jobs.
         * @param jobHandle Handle to a scheduled job.
         */
        void cancel(JobHandle jobHandle);
        /**
         * Removes the next scheduled job from the queue
         * @return A valid Job object or nullptr if no job is available to run
         */
        unique_ptr<Job> remove();
        /**
         * @return The context pointer managed by this JobQueue
         */
        void* context() {
            return _context;
        }

    private:
        Allocator _allocator;
        JobHandle _nextHandle;
        void* _context;
        typedef std::pair<JobHandle, unique_ptr<Job>> JobHandleObject; 
        vector<JobHandleObject> _jobs;
    };

} /* namespace cinekine */


#endif
