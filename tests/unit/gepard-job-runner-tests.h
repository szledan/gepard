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

#ifndef GEPARD_JOB_RUNNER_TESTS_H
#define GEPARD_JOB_RUNNER_TESTS_H

#include "gepard-job-runner.h"
#include "gtest/gtest.h"
#include <atomic>
#include <chrono>
#include <thread>

namespace {

#define millisec (1.0 / 1000.0)
#define microsec (millisec / 1000.0)
#define nanosec (microsec / 1000.0)

using Second = double;
struct JobTestClass {
    void incNumberAndWait(const Second sleepTime)
    {
        std::this_thread::sleep_for(std::chrono::duration<Second>(sleepTime));
        _number++;
    }

    void addValue(const int value)
    {
        _number += value;
    }

    const int number() const { return _number; }

private:
    std::atomic<int> _number = { 0 };
};

TEST(JobRunner, Constructor)
{
    gepard::JobRunner jobRunner0;
    EXPECT_EQ(jobRunner0.workerCount(), 1u) << "";

    gepard::JobRunner jobRunner1(2u);
    EXPECT_EQ(jobRunner1.workerCount(), 2u) << "";
}

TEST(JobRunner, BoundFunc)
{
    JobTestClass test;
    {
        gepard::JobRunner js(2u);

        js.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, 1.0 * millisec));
        js.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, 10.0 * millisec));
        js.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, 1.0 * millisec));
        js.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, 1.0 * millisec));
    }

    EXPECT_EQ(test.number(), 4) << "";

    {
        gepard::JobRunner js(2u);

        js.addJob(std::bind(&JobTestClass::addValue, &test, 1));
        js.addJob(std::bind(&JobTestClass::addValue, &test, 2));
        js.addJob(std::bind(&JobTestClass::addValue, &test, 3));
        js.addJob(std::bind(&JobTestClass::addValue, &test, 4));
    }

    EXPECT_EQ(test.number(), 14) << "";
}

TEST(JobRunner, WaitForJobs)
{
    JobTestClass test;
    {
        gepard::JobRunner jobRunner(2u);

        for (int i = 0; i < 10; ++i) {
            jobRunner.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, 1.0 * millisec));
        }
        EXPECT_EQ(jobRunner.isQueueEmpty(), false);
        jobRunner.waitForJobs();
        EXPECT_EQ(jobRunner.isQueueEmpty(), true);
    }

    EXPECT_EQ(test.number(), 10) << "";
}

TEST(JobRunner, ConcurrentJobRunners)
{
    JobTestClass test;
    {
        gepard::JobRunner jobRunnerA(2u);
        gepard::JobRunner jobRunnerB(2u);

        {
            gepard::JobRunner jobRunnerC(2u);

            for (int i = 0; i < 10; ++i) {
                jobRunnerA.addJob(std::bind(&JobTestClass::incNumberAndWait, &test, 1.0 * millisec));
                jobRunnerB.addJob([&]{ test.incNumberAndWait(1.0 * millisec); });
                jobRunnerC.addJob([&]{ test.incNumberAndWait(1.0 * millisec); });
            }
        }

        jobRunnerA.waitForJobs();
        jobRunnerB.waitForJobs();
        EXPECT_EQ(jobRunnerA.isQueueEmpty(), true);
        EXPECT_EQ(jobRunnerB.isQueueEmpty(), true);
    }

    EXPECT_EQ(test.number(), 30) << "";
}

} // anonymous namespace

#endif // GEPARD_JOB_RUNNER_TESTS_H
