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
    , _timeout(timeout < 0 ? 0 : timeout)
{
}

JobScheduler::~JobScheduler()
{
    waitForJobs();
}

void JobScheduler::addJob(std::function<void()> func)
{
    _jobRunner.addJob(std::bind(&JobScheduler::bindFunc, this, func, _state));
    _state->activeJobCount++;
}

const bool JobScheduler::waitForJobs(const Second timeout)
{
    std::unique_lock<std::mutex> lock(_mutex);
    return _state->hadTimeout = !_condVarPtr->wait_for(lock, std::chrono::duration<Second, std::nano>(timeout), [&]{
        GD_ASSERT(_state->activeJobCount >= 0);
        return !_state->activeJobCount;
    });
}

void JobScheduler::reset()
{
    if (_state.get()) {
        _state.reset();
    }
    _state = std::make_shared<State>(_condVarPtr);
}

void JobScheduler::bindFunc(std::function<void ()> func, StatePtr& state_)
{
    GD_ASSERT(func != nullptr);
    GD_ASSERT(state_.get());
    StatePtr state = state_;

    if ((!state->isValid)) {
        state->activeJobCount--;
        state->unfinishedJobCount++;
    } else {
        func();
        state->activeJobCount--;
    }

    GD_ASSERT(state->schedulerCondVarPtr.get());
    state->schedulerCondVarPtr->notify_all();
}

JobScheduler::State::State(JobScheduler::CondVarPtr& condVarPtr)
    : schedulerCondVarPtr(condVarPtr)
{
}

void JobScheduler::State::setInvalid() {
    std::lock_guard<std::mutex> guard(mutex);
    isValid = false;
}

} // namespace gepard
