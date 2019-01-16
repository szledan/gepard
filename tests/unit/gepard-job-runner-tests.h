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
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>

namespace {

struct JobRunnerTestClass {
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

    void addValue(const int value)
    {
        { // lock
            std::lock_guard<std::mutex> guard(_mutex);
            _number += value;
        } // unlock
    }

    const int number() const { return _number; }

private:
    int _number = 0;
    std::mutex _mutex;
};

TEST(JobRunner, Constructor)
{
    gepard::JobRunner jobRunner0;
    EXPECT_EQ(jobRunner0.workerCount(), 1u) << "";

    gepard::JobRunner jobRunner1(2u);
    EXPECT_EQ(jobRunner1.workerCount(), 2u) << "";

    gepard::JobRunner jobRunner2(-2u);
    EXPECT_EQ(jobRunner2.workerCount(), 1u) << "";
}

TEST(JobRunner, BoundFunc)
{
    JobRunnerTestClass test;
    {
        gepard::JobRunner js(2);

        js.addJob(std::bind(&JobRunnerTestClass::incNumberAndWait, &test, 1));
        js.addJob(std::bind(&JobRunnerTestClass::incNumberAndWait, &test, 10));
        js.addJob(std::bind(&JobRunnerTestClass::incNumberAndWait, &test, 1));
        js.addJob(std::bind(&JobRunnerTestClass::incNumberAndWait, &test, 1));
    }

    EXPECT_EQ(test.number(), 4) << "";

    {
        gepard::JobRunner js(2);

        js.addJob(std::bind(&JobRunnerTestClass::addValue, &test, 1));
        js.addJob(std::bind(&JobRunnerTestClass::addValue, &test, 2));
        js.addJob(std::bind(&JobRunnerTestClass::addValue, &test, 3));
        js.addJob(std::bind(&JobRunnerTestClass::addValue, &test, 4));
    }

    EXPECT_EQ(test.number(), 14) << "";
}

TEST(JobRunner, CallbackFunc)
{
    int value = 0;
    {
        gepard::JobRunner js(2);

        js.addJob([&]{ value++; }, [&]{ value = 10; });
    }

    EXPECT_EQ(value, 10) << "";
}

} // anonymous namespace

#endif // GEPARD_JOB_RUNNER_TESTS_H
