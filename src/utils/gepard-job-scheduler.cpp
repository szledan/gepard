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

namespace gepard {

JobScheduler::JobScheduler(JobRunner& jobRunner, const Second timeout)
    : _jobRunner(jobRunner)
    , _timeout(timeout < 0.0 ? 0.0 : timeout)
{
}

JobScheduler::~JobScheduler()
{
    waitForJobs();
}

void JobScheduler::addJob(std::function<void()> func)
{
    _jobRunner.addJob(std::bind(&JobScheduler::bindFunc, this, func, _currentState));
    _currentState->activeJobCount++;
}

void JobScheduler::reset()
{
    if (_currentState.get()) {
        _currentState.reset();
    }
    _currentState = std::make_shared<State>();
}

const JobScheduler::StatePtr JobScheduler::releaseState()
{
    StatePtr state = _currentState;
    reset();
    return state;
}

void JobScheduler::bindFunc(std::function<void ()> func, StatePtr& state_)
{
    GD_ASSERT(func != nullptr);
    GD_ASSERT(state_.get());
    StatePtr state = state_;

    if ((!state->isValid)) {
        state->unfinishedJobCount++;
    } else {
        func();
    }
    state->activeJobCount--;

    { // lock
        std::lock_guard<std::mutex> guard(state->stateMutex);
        state->stateCondVar.notify_all();
    } // unlock
}

void JobScheduler::State::setInvalid()
{
    isValid = false;
}

const bool JobScheduler::State::waitForJobs(const JobScheduler::Second timeout)
{
    std::unique_lock<std::mutex> ulock(stateMutex);
    return hadTimeout = !stateCondVar.wait_for(ulock, std::chrono::duration<Second>(timeout), [&]{
        GD_ASSERT(activeJobCount >= 0);
        return !activeJobCount;
    });
}

} // namespace gepard
