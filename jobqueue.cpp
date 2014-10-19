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
 * @file    cinek/jobqueue.cpp
 * @author  Samir Sinha
 * @date    2/22/2014
 * @brief   A job housekeeping and dispatcher
 * @copyright Cinekine
 */

#include "cinek/jobqueue.hpp"

#include <algorithm>

namespace cinek {

    /**
     * @param allocator  An (optional) allocator for custom memory
     *                   management
     */
    JobQueue::JobQueue(size_t queueLimit, void* context,
                       const Allocator& allocator) :
        _allocator(allocator),
        _nextHandle(1),
        _context(context),
        _jobs(std_allocator<JobHandleObject>(_allocator))

    {
        _jobs.reserve(queueLimit);
    }

    JobHandle JobQueue::schedule(unique_ptr<Job>&& job)
    {
        auto jobHandleObject = JobHandleObject(_nextHandle, std::move(job));
        auto it = std::lower_bound(_jobs.begin(), _jobs.end(),
                            jobHandleObject,
                            [](const JobHandleObject& h1, const JobHandleObject& h2) -> bool
                            {
                                return h1.second->priority() < h2.second->priority();
                            });
        it = _jobs.emplace(it, std::move(jobHandleObject));
        return it != _jobs.end() ? _nextHandle++ : kNullHandle;
    }

    void JobQueue::cancel(JobHandle jobHandle)
    {
        for (auto it = _jobs.begin(); it != _jobs.end(); ++it)
        {
            if (it->first == jobHandle)
            {
                _jobs.erase(it);
                return;
            }
        }
    }

    unique_ptr<Job> JobQueue::remove()
    {
        if (_jobs.empty())
            return unique_ptr<Job>();

        unique_ptr<Job> job = std::move(_jobs.back().second);
        _jobs.pop_back();
        return std::move(job);
    }

} /* namespace cinek */

