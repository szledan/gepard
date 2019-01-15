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

#include "gepard-job-scheduler.h"

#include "gepard-defs.h"
#include <chrono>
#include <iostream>
#include <string>

namespace gepard {

JobScheduler::JobScheduler(const unsigned int workerCount)
{
    unsigned int wc = (workerCount && workerCount < 8) ? workerCount : 1;
    do {
        _workers.push_back(std::thread(worker, this));
    } while (--wc);
}

JobScheduler::~JobScheduler()
{
    while (!finish) {
        { // lock
            std::lock_guard<std::mutex> guard(queueMutex);
            if (queue.empty() && !activeJobCount) {
                finish = true;
            }
        } // unlock
        condVar.notify_all();
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));
    };

    for (size_t i = 0; i < _workers.size(); ++i) {
        _workers[i].join();
    }
}

void JobScheduler::addJob(Job* job)
{
    { // lock
        std::lock_guard<std::mutex> guard(queueMutex);
        queue.push(job);
    } // unlock
    condVar.notify_one();
}

void JobScheduler::worker(JobScheduler* jobScheduler)
{
    while (true) {
        Job* job;
        { // lock
            std::unique_lock<std::mutex> lock(jobScheduler->queueMutex);
            if (jobScheduler->queue.empty()) {
                jobScheduler->condVar.wait(lock, [&]{ return jobScheduler->finish || !jobScheduler->queue.empty(); });
            }
            if (jobScheduler->finish) {
                break;
            }
            if (jobScheduler->queue.empty()) {
                continue;
            }
            job = jobScheduler->queue.front();
            jobScheduler->queue.pop();
            jobScheduler->activeJobCount++;
        } // unlock

        job->run();
        delete job;

        { // lock
            std::lock_guard<std::mutex> guard(jobScheduler->queueMutex);
            jobScheduler->activeJobCount--;
        } // unlock
    };
}

Job::~Job()
{
}

void Job::run()
{
    GD_ASSERT(false);
}

} // namespace gepard
