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
#include <thread>

namespace {

TEST(JobScheduler, Constructor)
{
    gepard::JobRunner jobRunner(2u);
    gepard::JobScheduler jobScheduler0(jobRunner);
    gepard::JobScheduler jobScheduler1(jobRunner, 0);
    gepard::JobScheduler jobScheduler2(jobRunner, 1000);
}

TEST(JobScheduler, SimpleWaitForFence)
{
    gepard::JobRunner jobRunner(2u);

    gepard::JobScheduler jobScheduler(jobRunner);

    gepard::JobScheduler jobScheduler1(jobRunner, 0);
    gepard::JobScheduler jobScheduler2(jobRunner, 1000);
}

} // anonymous namespace

#endif // GEPARD_JOB_SCHEDULER_TESTS_H
