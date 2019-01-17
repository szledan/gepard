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
#include "gepard-job-runner.h"
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

namespace gepard {

JobScheduler::JobScheduler(JobRunner& jobRunner, const NanoSec timeout)
    : _jobRunner(jobRunner)
    , _timeout(timeout < 0 ? 0 : timeout)
{
    reset();
}

JobScheduler::~JobScheduler()
{
    waitForJobs();
}

void JobScheduler::addJob(std::function<void()> func)
{
    _jobRunner.addJob(std::bind(&JobScheduler::bindFunc, this, func, _state));
    { // lock
        std::lock_guard<std::mutex> guard(_state->mutex);
        _state->activeJobCount++;
    } // unlock
}

void JobScheduler::waitForJobs(const NanoSec timeout)
{
    using hrc = std::chrono::high_resolution_clock;
    hrc timer;
    hrc::time_point startTime = timer.now();
    hrc::duration spentTime;
    do {
        { // lock
            std::lock_guard<std::mutex> guard(_state->mutex);
            GD_ASSERT(_state->activeJobCount >= 0);
            if (_state->timeouted || !_state->activeJobCount) {
                return;
            }
        } // unlock
        std::this_thread::sleep_for(std::chrono::nanoseconds(_waitTime));
        spentTime = timer.now() - startTime;
    } while (spentTime.count() < timeout);

    { // lock
        std::lock_guard<std::mutex> guard(_state->mutex);
        _state->timeouted = true;
    } // unlock
}

void JobScheduler::reset()
{
    if (_state.get()) {
        { // lock
            std::lock_guard<std::mutex> guard(_state->mutex);
            _state->isValid = false;
        } // unlock
        _state.reset();
    }
    _state = std::make_shared<State>();
}

void JobScheduler::bindFunc(std::function<void ()> func, StatePtr& state_)
{
    GD_ASSERT(func != nullptr);
    GD_ASSERT(state_.get());
    StatePtr state = state_;

    { // lock
        std::lock_guard<std::mutex> guard(state->mutex);

        if ((!state->isValid) || state->timeouted) {
            state->activeJobCount--;
            state->unfinishedJobCount++;
            return;
        }
    } // unlock

    func();

    { // lock
        std::lock_guard<std::mutex> guard(state->mutex);
        state->activeJobCount--;
    } // unlock
}

} // namespace gepard
