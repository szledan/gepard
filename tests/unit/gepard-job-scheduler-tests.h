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

#include "gepard-job-scheduler.h"
#include "gtest/gtest.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

namespace {

TEST(JobScheduler, Constructor)
{
    gepard::JobRunner jobRunner(2u);
    gepard::JobScheduler jobScheduler0(jobRunner);
    gepard::JobScheduler jobScheduler1(jobRunner, 0);
    gepard::JobScheduler jobScheduler2(jobRunner, 1000);
}

struct JobSchedulerTestClass {
    void printThreadIDAndWait()
    {
        std::cerr << std::this_thread::get_id() << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    void incNumberAndWait(const int64_t milliseconds)
    {
        { // lock
            std::lock_guard<std::mutex> guard(_mutex);
            _number++;
        } // unlock
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    const int number() const { return _number; }

private:
    int _number = 0;
    std::mutex _mutex;
};

TEST(JobScheduler, SimpleWaitForFence)
{
    gepard::JobRunner jobRunner(2u);
    JobSchedulerTestClass test;

    gepard::JobScheduler jobScheduler(jobRunner);
    jobScheduler.addJob(std::bind(&JobSchedulerTestClass::incNumberAndWait, &test, 1));
    jobScheduler.waitForJobs();
    EXPECT_EQ(jobScheduler.timeouted(), false);
    EXPECT_EQ(test.number(), 1);
    jobScheduler.addJob(std::bind(&JobSchedulerTestClass::incNumberAndWait, &test, 1));
    jobScheduler.waitForJobs();
    EXPECT_EQ(jobScheduler.timeouted(), false);
    EXPECT_EQ(test.number(), 2);
}

TEST(JobScheduler, WaitForFence)
{
    gepard::JobRunner jobRunner(2u);
    JobSchedulerTestClass test;
    const int64_t oneMilliSec = 1000000;

    gepard::JobScheduler jobScheduler(jobRunner);
    jobScheduler.addJob(std::bind(&JobSchedulerTestClass::incNumberAndWait, &test, 1));
    jobScheduler.addJob(std::bind(&JobSchedulerTestClass::incNumberAndWait, &test, 1));
    jobScheduler.addJob(std::bind(&JobSchedulerTestClass::incNumberAndWait, &test, 1));
    jobScheduler.waitForJobs(oneMilliSec);
    EXPECT_EQ(jobScheduler.timeouted(), true);
    EXPECT_LT(test.number(), 3);
}

} // anonymous namespace

#endif // GEPARD_JOB_SCHEDULER_TESTS_H
