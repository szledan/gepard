/* Copyright (C) 2016-2017, 2018, Gepard Graphics
 * Copyright (C) 2016-2017, 2018, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_VEC4_TESTS_H
#define GEPARD_VEC4_TESTS_H

#include "gepard-vec4.h"
#include "gtest/gtest.h"

namespace {

TEST(Vec4Test, Initialize)
{
    gepard::Vec4 vec4;

    EXPECT_EQ(0.0f, vec4.x);
    EXPECT_EQ(0.0f, vec4.y);
    EXPECT_EQ(0.0f, vec4.z);
    EXPECT_EQ(0.0f, vec4.w);

    EXPECT_EQ(0.0f, vec4.r);
    EXPECT_EQ(0.0f, vec4.g);
    EXPECT_EQ(0.0f, vec4.b);
    EXPECT_EQ(0.0f, vec4.a);

    EXPECT_EQ(0.0f, vec4.s);
    EXPECT_EQ(0.0f, vec4.t);
    EXPECT_EQ(0.0f, vec4.p);
    EXPECT_EQ(0.0f, vec4.q);

    EXPECT_EQ(0.0f, vec4[0]);
    EXPECT_EQ(0.0f, vec4[1]);
    EXPECT_EQ(0.0f, vec4[2]);
    EXPECT_EQ(0.0f, vec4[3]);
}

TEST(Vec4Test, Union)
{
    gepard::Vec4 vec4(1.0f, 2.0f, 3.0f, 4.0f);

    EXPECT_EQ(1.0f, vec4.x);
    EXPECT_EQ(1.0f, vec4.r);
    EXPECT_EQ(1.0f, vec4.s);
    EXPECT_EQ(1.0f, vec4[0]);

    EXPECT_EQ(2.0f, vec4.y);
    EXPECT_EQ(2.0f, vec4.g);
    EXPECT_EQ(2.0f, vec4.t);
    EXPECT_EQ(2.0f, vec4[1]);

    EXPECT_EQ(3.0f, vec4.z);
    EXPECT_EQ(3.0f, vec4.b);
    EXPECT_EQ(3.0f, vec4.p);
    EXPECT_EQ(3.0f, vec4[2]);

    EXPECT_EQ(4.0f, vec4.w);
    EXPECT_EQ(4.0f, vec4.a);
    EXPECT_EQ(4.0f, vec4.q);
    EXPECT_EQ(4.0f, vec4[3]);
}

TEST(Vec4Test, CopyConstructor)
{
    gepard::Vec4 vec4(1.0f, 2.0f, 3.0f, 4.0f);
    gepard::Vec4 vec4New(vec4);

    EXPECT_EQ(1.0f, vec4New.x);
    EXPECT_EQ(2.0f, vec4New.y);
    EXPECT_EQ(3.0f, vec4New.z);
    EXPECT_EQ(4.0f, vec4New.w);
}

} // anonymous namespace

#endif // GEPARD_VEC4_TESTS_H
