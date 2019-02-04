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

#ifndef GEPARD_JOB_SCHEDULER_TESTS_H
#define GEPARD_JOB_SCHEDULER_TESTS_H

#include "gepard-job-runner.h"
#include "gepard-job-runner-tests.h" // JobTestClass
#include "gepard-job-scheduler.h"
#include "gtest/gtest.h"
#include <chrono>
#include <mutex>
#include <sstream>
#include <thread>

namespace {

TEST(JobScheduler, Constructor)
{
    gepard::JobRunner jobRunner(2u);
    gepard::JobScheduler jobScheduler0(jobRunner);
    gepard::JobScheduler jobScheduler1(jobRunner, 0);
    gepard::JobScheduler jobScheduler2(jobRunner, millisec);
    gepard::JobScheduler jobScheduler3(jobRunner, -1);
}

TEST(JobScheduler, Destructor)
{
    const int threads = 2;
    JobTestClass test;
    gepard::JobRunner jobRunner((unsigned int)threads);

    {
        gepard::JobScheduler jobScheduler(jobRunner);
        for (int i = 0; i < 10 * threads; ++i) {
            jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, 1.0 * microsec));
        }
        EXPECT_LT(test.number(), 10 * threads);
    }
    EXPECT_EQ(test.number(), 10 * threads);
}

TEST(JobScheduler, SimpleWaitForJobs)
{
    gepard::JobRunner jobRunner(2u);
    JobTestClass test;
    gepard::JobScheduler jobScheduler(jobRunner);

    jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, 1.0 * microsec));
    jobScheduler.waitForJobs();
    EXPECT_EQ(jobScheduler.state()->hadTimeout, false);
    EXPECT_EQ(test.number(), 1);
    jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, 1.0 * microsec));
    jobScheduler.waitForJobs();
    EXPECT_EQ(jobScheduler.state()->hadTimeout, false);
    EXPECT_EQ(test.number(), 2);
}

TEST(JobScheduler, WaitForJobs)
{
    const Second waitingTime = 1.0 * microsec;

    for (int threads = 1; threads < 8; ++threads) {
        gepard::JobRunner jobRunner((unsigned int)threads);

        for (int n = 3; n < 10; ++n)
        for (int t = 1; t < n - 1; ++t) {
            JobTestClass test;
            gepard::JobScheduler jobScheduler(jobRunner);
            std::stringstream info;
            info << "n: " << n << ", t: " << t << ", threads: " << threads;

            const int jobCount = n * threads;
            for (int i = 0; i < jobCount; ++i) {
                jobScheduler.addJob([&]{ test.incNumberAndWait(waitingTime); });
            }
            const gepard::JobScheduler::StatePtr state = jobScheduler.state();

            EXPECT_EQ(state->isValid, true) << info.str();
            EXPECT_EQ(state->hadTimeout, false) << info.str();
            EXPECT_EQ(state->unfinishedJobCount, 0) << info.str();

            jobScheduler.waitForJobs(t * waitingTime);

            EXPECT_EQ(state->isValid, true) << info.str();

            jobRunner.waitForJobs();
        }
    }
}

TEST(JobScheduler, Reset)
{
    const Second waitingTime = 1.0 * microsec;
    const int threads = 2;
    gepard::JobRunner jobRunner((unsigned int)threads);

    for (int waitAllJobs = 0; waitAllJobs < 2; ++waitAllJobs)
    for (int n = 3; n < 10; ++n)
    for (int t = 1; t < n - 1; ++t) {
        std::shared_ptr<JobTestClass> test = std::make_shared<JobTestClass>();
        gepard::JobScheduler jobScheduler(jobRunner);
        std::stringstream info;
        info << "waitAllJobs: " << waitAllJobs << ", n: " << n << ", t: " << t << ", threads: " << threads;
        gepard::JobScheduler::StatePtr state = jobScheduler.state();

        const int jobCount = n * threads;
        for (int i = 0; i < jobCount; ++i) {
            jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, test, waitingTime));
        }

        EXPECT_EQ(state->unfinishedJobCount, 0) << info.str();
        EXPECT_EQ(state->hadTimeout, false) << info.str();

        jobScheduler.waitForJobs(t * waitingTime);

        EXPECT_GE(state->activeJobCount, 0) << info.str();
        if (state->activeJobCount > 0) {
            EXPECT_EQ(state->hadTimeout, true) << info.str();
            EXPECT_EQ(state->isValid, true) << info.str();
        }

        if (waitAllJobs) {
            jobRunner.waitForJobs();
        }

        jobScheduler.reset();

        EXPECT_NE(jobScheduler.state().get(), state.get()) << info.str();
        EXPECT_EQ(jobScheduler.state().use_count(), 1) << info.str();
        if (waitAllJobs) {
            EXPECT_EQ(state.use_count(), 1) << info.str();
        }

        EXPECT_EQ(state->isValid, true) << info.str();

        if (waitAllJobs) {
            EXPECT_EQ(state->activeJobCount, 0) << info.str();
            EXPECT_EQ(test->number(), (jobCount - state->unfinishedJobCount)) << info.str();
        } else {
            EXPECT_GE(state->activeJobCount, 0) << info.str();
        }

        EXPECT_EQ(jobScheduler.state()->isValid, true) << info.str();
        EXPECT_EQ(jobScheduler.state()->hadTimeout, false) << info.str();
        EXPECT_EQ(jobScheduler.state()->activeJobCount, 0) << info.str();
        EXPECT_EQ(jobScheduler.state()->unfinishedJobCount, 0) << info.str();
    }
}

TEST(JobScheduler, SimpleAddJob)
{
    const Second waitingTime = 1.0 * microsec;
    const int threads = 2;
    gepard::JobRunner jobRunner((unsigned int)threads);
    JobTestClass test;
    gepard::JobScheduler jobScheduler(jobRunner);

    const int jobCount = 10 * threads;
    for (int i = 0; i < jobCount; ++i) {
        jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }

    jobScheduler.waitForJobs();
    EXPECT_EQ(test.number(), jobCount);

    for (int i = 0; i < jobCount; ++i) {
        jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }

    jobScheduler.waitForJobs();
    EXPECT_EQ(test.number(), 2 * jobCount);
}

TEST(JobScheduler, SimpleOneByOneAddJob)
{
    const Second waitingTime = 1.0 * microsec;
    const int threads = 2;
    gepard::JobRunner jobRunner((unsigned int)threads);
    JobTestClass test;
    gepard::JobScheduler jobScheduler(jobRunner);

    const int jobCount = 10 * threads;
    for (int i = 0; i < jobCount; ++i) {
        jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
        jobScheduler.waitForJobs();
        EXPECT_EQ(jobScheduler.state()->isValid, true);
        EXPECT_EQ(jobScheduler.state()->hadTimeout, false);
        EXPECT_EQ(jobScheduler.state()->activeJobCount, 0);
        EXPECT_EQ(jobScheduler.state()->unfinishedJobCount, 0);
        EXPECT_EQ(test.number(), i + 1);
    }
}

TEST(JobScheduler, AddJobTwoSchedulers)
{
    const Second waitingTime = 1.0 * microsec;
    const int threads = 2;
    gepard::JobRunner jobRunner((unsigned int)threads);
    JobTestClass test;
    gepard::JobScheduler jobSchedulerA(jobRunner);
    gepard::JobScheduler jobSchedulerB(jobRunner);

    const int jobCount = 10 * threads;
    for (int i = 0; i < jobCount; ++i) {
        jobSchedulerA.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
        jobSchedulerB.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }

    jobSchedulerA.waitForJobs();
    EXPECT_EQ(jobSchedulerA.state()->isValid, true);
    EXPECT_EQ(jobSchedulerA.state()->hadTimeout, false);
    EXPECT_EQ(jobSchedulerA.state()->activeJobCount, 0);
    EXPECT_EQ(jobSchedulerA.state()->unfinishedJobCount, 0);
    EXPECT_GE(test.number(), jobCount);

    jobSchedulerB.waitForJobs();
    EXPECT_EQ(jobSchedulerB.state()->isValid, true);
    EXPECT_EQ(jobSchedulerB.state()->hadTimeout, false);
    EXPECT_EQ(jobSchedulerB.state()->activeJobCount, 0);
    EXPECT_EQ(jobSchedulerB.state()->unfinishedJobCount, 0);
    EXPECT_EQ(test.number(), 2 * jobCount);

    for (int i = 0; i < jobCount; ++i) {
        jobSchedulerA.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
        jobSchedulerB.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }

    jobSchedulerA.waitForJobs();
    EXPECT_EQ(jobSchedulerA.state()->isValid, true);
    EXPECT_EQ(jobSchedulerA.state()->hadTimeout, false);
    EXPECT_EQ(jobSchedulerA.state()->activeJobCount, 0);
    EXPECT_EQ(jobSchedulerA.state()->unfinishedJobCount, 0);
    EXPECT_GE(test.number(), 3 * jobCount);

    jobSchedulerB.waitForJobs();
    EXPECT_EQ(jobSchedulerB.state()->isValid, true);
    EXPECT_EQ(jobSchedulerB.state()->hadTimeout, false);
    EXPECT_EQ(jobSchedulerB.state()->activeJobCount, 0);
    EXPECT_EQ(jobSchedulerB.state()->unfinishedJobCount, 0);
    EXPECT_EQ(test.number(), 4 * jobCount);
}

TEST(JobScheduler, AddJobTwoDiffSchedulers)
{
    const Second waitingTime = 1.0 * microsec;
    const int threads = 2;
    gepard::JobRunner jobRunnerA((unsigned int)threads);
    gepard::JobRunner jobRunnerB((unsigned int)threads);
    JobTestClass test;
    gepard::JobScheduler jobSchedulerA(jobRunnerA);
    gepard::JobScheduler jobSchedulerB(jobRunnerB);

    const int jobCount = 10 * threads;
    for (int i = 0; i < jobCount; ++i) {
        jobSchedulerA.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
        jobSchedulerB.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }

    jobSchedulerA.waitForJobs();
    EXPECT_EQ(jobSchedulerA.state()->isValid, true);
    EXPECT_EQ(jobSchedulerA.state()->hadTimeout, false);
    EXPECT_EQ(jobSchedulerA.state()->activeJobCount, 0);
    EXPECT_EQ(jobSchedulerA.state()->unfinishedJobCount, 0);
    EXPECT_GE(test.number(), jobCount);

    jobSchedulerB.waitForJobs();
    EXPECT_EQ(jobSchedulerB.state()->isValid, true);
    EXPECT_EQ(jobSchedulerB.state()->hadTimeout, false);
    EXPECT_EQ(jobSchedulerB.state()->activeJobCount, 0);
    EXPECT_EQ(jobSchedulerB.state()->unfinishedJobCount, 0);
    EXPECT_EQ(test.number(), 2 * jobCount);

    for (int i = 0; i < jobCount; ++i) {
        jobSchedulerA.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
        jobSchedulerB.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }

    jobSchedulerA.waitForJobs();
    EXPECT_EQ(jobSchedulerA.state()->isValid, true);
    EXPECT_EQ(jobSchedulerA.state()->hadTimeout, false);
    EXPECT_EQ(jobSchedulerA.state()->activeJobCount, 0);
    EXPECT_EQ(jobSchedulerA.state()->unfinishedJobCount, 0);
    EXPECT_GE(test.number(), 3 * jobCount);

    jobSchedulerB.waitForJobs();
    EXPECT_EQ(jobSchedulerB.state()->isValid, true);
    EXPECT_EQ(jobSchedulerB.state()->hadTimeout, false);
    EXPECT_EQ(jobSchedulerB.state()->activeJobCount, 0);
    EXPECT_EQ(jobSchedulerB.state()->unfinishedJobCount, 0);
    EXPECT_EQ(test.number(), 4 * jobCount);
}

TEST(JobScheduler, SimpleStateSetInvalid)
{
    const Second waitingTime = 1.0 * microsec;
    const int threads = 2;
    gepard::JobRunner jobRunner((unsigned int)threads);
    JobTestClass test;
    gepard::JobScheduler jobScheduler(jobRunner);
    gepard::JobScheduler::StatePtr state = jobScheduler.state();

    const int jobCount = 10 * threads;
    for (int i = 0; i < jobCount; ++i) {
        jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }
    state->setInvalid();
    for (int i = 0; i < jobCount; ++i) {
        jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }

    jobScheduler.waitForJobs();
    jobRunner.waitForJobs();

    EXPECT_EQ(test.number(), (2 * jobCount - state->unfinishedJobCount));
    EXPECT_GE(state->unfinishedJobCount, jobCount);
}

TEST(JobScheduler, ReleasedStateSetInvalid)
{
    const Second waitingTime = 1.0 * microsec;
    const int threads = 2;
    gepard::JobRunner jobRunner((unsigned int)threads);
    JobTestClass test;
    gepard::JobScheduler jobScheduler(jobRunner);
    gepard::JobScheduler::StatePtr state = jobScheduler.state();

    const int jobCount = 10 * threads;
    for (int i = 0; i < jobCount; ++i) {
        jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }

    jobScheduler.reset();
    state->setInvalid();

    for (int i = 0; i < jobCount; ++i) {
        jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));
    }

    jobScheduler.waitForJobs();
    jobRunner.waitForJobs();

    EXPECT_EQ(test.number(), (2 * jobCount - state->unfinishedJobCount));
    EXPECT_LE(state->unfinishedJobCount, jobCount);
    EXPECT_EQ(jobScheduler.state()->unfinishedJobCount, 0);
}

TEST(JobScheduler, ReleasedStateWaitForJobs)
{
    const Second waitingTime = 1.0 * microsec;
    const int threads = 2;
    gepard::JobRunner jobRunner((unsigned int)threads);
    JobTestClass test;
    gepard::JobScheduler jobScheduler(jobRunner);

    const int jobCount = 10 * threads;
    std::vector<std::thread> stateThreads;
    for (int i = 0; i < jobCount; ++i) {
        jobScheduler.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, waitingTime));

        gepard::JobScheduler::StatePtr currentState = jobScheduler.releaseState();
        stateThreads.push_back(std::thread([=]{
            gepard::JobScheduler::StatePtr state = currentState;

            state->waitForJobs();

            EXPECT_EQ(state->isValid, true);
            EXPECT_EQ(state->hadTimeout, false);
            EXPECT_EQ(state->activeJobCount, 0);
            EXPECT_EQ(state->unfinishedJobCount, 0);
        }));
    }

    for (size_t i = 0; i < stateThreads.size(); ++i) {
        stateThreads[i].join();
    }

    EXPECT_EQ(test.number(), jobCount);
}

} // anonymous namespace

#endif // GEPARD_JOB_SCHEDULER_TESTS_H
