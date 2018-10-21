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

#ifndef GEPARD_FLOAT_POINT_TESTS_H
#define GEPARD_FLOAT_POINT_TESTS_H

#include "gepard-float-point.h"
#include "gtest/gtest.h"

namespace {

TEST(FloatPointTest, Initialize)
{
    gepard::FloatPoint fp0;

    EXPECT_EQ(0.0, fp0.x);
    EXPECT_EQ(0.0, fp0.y);

    gepard::FloatPoint fp(1.0, 2.0);

    EXPECT_EQ(1.0, fp.x);
    EXPECT_EQ(2.0, fp.y);
}

TEST(FloatPointTest, LengthSquared)
{
    gepard::FloatPoint fp(3.0, -4.0);

    EXPECT_EQ(25.0, fp.lengthSquared());
}

TEST(FloatPointTest, Dot)
{
    gepard::FloatPoint fp(3.0, -4.0);

    EXPECT_EQ(2.0, fp.dot(gepard::FloatPoint(2.0, 1.0)));
}

TEST(FloatPointTest, Operators)
{
    gepard::FloatPoint fp(3.0, -4.0);

    // Operator '=='.
    EXPECT_TRUE(gepard::FloatPoint(3.0, -4.0) == fp);
    // Operator '<'.
    EXPECT_TRUE(gepard::FloatPoint(3.0, -5.0) < fp);
    EXPECT_TRUE(gepard::FloatPoint(2.0, -4.0) < fp);
    // Operator '+'.
    EXPECT_EQ(gepard::FloatPoint(), gepard::FloatPoint(-3.0, 4.0) + fp);
    // Operator '-'.
    EXPECT_EQ(gepard::FloatPoint(), gepard::FloatPoint(3.0, -4.0) - fp);
    // Operator '/'.
    EXPECT_EQ(gepard::FloatPoint(1.5, -2.0), fp / 2.0);
    // Operator '*'.
    EXPECT_EQ(gepard::FloatPoint(4.5, 2.0), gepard::FloatPoint(1.5, -0.5) * fp);
}

} // anonymous namespace

#endif // GEPARD_FLOAT_POINT_TESTS_H
