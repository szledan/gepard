/* Copyright (C) 2019, Gepard Graphics
 * Copyright (C) 2019, Szilard Ledan <szledan@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gepard-job-runner.h"

#include "gepard-defs.h"
#include <chrono>
#include <iostream>
#include <string>

namespace gepard {

JobRunner::Job::Job(std::function<void ()> func)
    : boundFunc(func)
{
}

void JobRunner::Job::run()
{
    GD_ASSERT(boundFunc != nullptr);
    boundFunc();
}

JobRunner::JobRunner(const unsigned int workerCount)
{
    unsigned int wc = workerCount > 0 ? workerCount : 1;
    do {
        _workers.push_back(std::thread(worker, this));
    } while (--wc);
}

JobRunner::~JobRunner()
{
    waitForJobs();
    { // lock
        std::lock_guard<std::mutex> guard(queueMutex);
        GD_ASSERT(queue.empty() && !activeJobCount);
        finish = true;
    } // unlock
    condVar.notify_all();

    for (size_t i = 0; i < _workers.size(); ++i) {
        _workers[i].join();
    }
}

void JobRunner::addJob(std::function<void()> func)
{
    { // lock
        std::lock_guard<std::mutex> guard(queueMutex);
        queue.push_back(new Job(func));
    } // unlock
    condVar.notify_one();
}

void JobRunner::waitForJobs()
{
    bool hasJob = true;
    while (hasJob) {
        { // lock
            std::lock_guard<std::mutex> guard(queueMutex);
            if (queue.empty() && !activeJobCount) {
                hasJob = false;
            }
        } // unlock
        condVar.notify_all();
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    };
}

void JobRunner::worker(JobRunner* jobRunner)
{
    while (true) {
        Job* job;
        { // lock
            std::unique_lock<std::mutex> lock(jobRunner->queueMutex);
            if (jobRunner->queue.empty()) {
                jobRunner->condVar.wait(lock, [&]{ return jobRunner->finish || !jobRunner->queue.empty(); });
            }
            if (jobRunner->finish) {
                break;
            }
            if (jobRunner->queue.empty()) {
                continue;
            }
            job = jobRunner->queue.front();
            GD_ASSERT(job);
            jobRunner->queue.pop_front();
            jobRunner->activeJobCount++;
        } // unlock

        job->run();
        delete job;

        { // lock
            std::lock_guard<std::mutex> guard(jobRunner->queueMutex);
            jobRunner->activeJobCount--;
        } // unlock
    };
}

} // namespace gepard
