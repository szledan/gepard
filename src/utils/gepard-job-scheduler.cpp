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

JobScheduler::JobScheduler(JobRunner& jobRunner, const int64_t timeout)
    : _jobRunner(jobRunner)
    , _timeout(timeout)
{
}

JobScheduler::~JobScheduler()
{
}

void JobScheduler::addJob(std::function<void()> func, std::function<void()> callback)
{
    { // lock
        std::lock_guard<std::mutex> guard(_mutex);
        _activeJobCount++;
    } // unlock
    _jobRunner.addJob(std::bind(&JobScheduler::bindFunc, this, func), std::bind(&JobScheduler::callbackFunc, this, callback));
}

void JobScheduler::waitForJobs(const int64_t timeout)
{
    { // lock
        std::lock_guard<std::mutex> guard(_mutex);
        _timeouted = false;
    } // unlock

    using hrc = std::chrono::high_resolution_clock;
    hrc timer;
    hrc::time_point startTime = timer.now();
    hrc::duration spentTime;
    do {
        { // lock
            std::lock_guard<std::mutex> guard(_mutex);
            GD_ASSERT(_activeJobCount >= 0);
            if (!_activeJobCount) {
                return;
            }
        } // unlock
        std::this_thread::sleep_for(std::chrono::nanoseconds(_waitTime));
        spentTime = timer.now() - startTime;
    } while (spentTime.count() < timeout);

    { // lock
        std::lock_guard<std::mutex> guard(_mutex);
        _activeJobCount = 0;
        _timeouted = true;
    } // unlock
}

void JobScheduler::bindFunc(std::function<void ()> func)
{
    GD_ASSERT(func != nullptr);

    { // lock
        std::lock_guard<std::mutex> guard(_mutex);
        if (_timeouted) {
            return;
        }
    } // unlock

    func();
}

void JobScheduler::callbackFunc(std::function<void ()> callback)
{
    { // lock
        std::lock_guard<std::mutex> guard(_mutex);
        if (_timeouted) {
            return;
        }
    } // unlock

    if (callback != nullptr) {
        callback();
    }

    { // lock
        std::lock_guard<std::mutex> guard(_mutex);
        _activeJobCount--;
    } // unlock
}

} // namespace gepard
