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

#ifndef GEPARD_JOB_SCHEDULER_H
#define GEPARD_JOB_SCHEDULER_H

#include "gepard-defs.h"
#include "gepard-job-runner.h"
#include <atomic>
#include <condition_variable>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

namespace gepard {

/*!
 * \brief The JobScheduler class
 */
class JobScheduler {
public:
    using Second = double;
    static constexpr double kYearInSec = 31556926.0;

    struct State {
        void setInvalid();
        const bool waitForJobs(const Second timeout = kYearInSec);

        std::atomic<bool> hadTimeout = { false };
        std::atomic<bool> isValid = { true };
        std::atomic<int32_t> activeJobCount = { 0 };
        std::atomic<int32_t> unfinishedJobCount = { 0 };

        std::mutex stateMutex;
        std::condition_variable stateCondVar;
    };

    using StatePtr = std::shared_ptr<State>;

    JobScheduler(JobRunner&, const Second timeout = kYearInSec);
    ~JobScheduler();

    void addJob(std::function<void()> func);

    const bool waitForJobs() { return waitForJobs(_timeout); }
    const bool waitForJobs(const Second timeout) { return state()->waitForJobs(timeout); }

    void reset();
    const StatePtr releaseState();
    const StatePtr& state() const { return _currentState; }

private:
    void bindFunc(std::function<void()>, StatePtr& state);

    JobRunner& _jobRunner;
    const Second _timeout;

    StatePtr _currentState = std::make_shared<State>();
};

} // namespace gepard

#endif // GEPARD_JOB_SCHEDULER_H
