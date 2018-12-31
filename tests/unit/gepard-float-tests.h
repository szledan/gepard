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

#ifndef GEPARD_FLOAT_TESTS_H
#define GEPARD_FLOAT_TESTS_H

#include "gepard-float.h"
#include "gtest/gtest.h"
#include <cmath>
#include <limits>

namespace {

TEST(FloatTest, PiFloat)
{
    const gepard::Float epsilon = std::numeric_limits<gepard::Float>::epsilon();

    EXPECT_LT(std::fabs(std::sin(gepard::piFloat) - 0.0), epsilon) << "Architecture used 'pi' is different than expected one.";
    EXPECT_LT(std::fabs(std::sin(-gepard::piFloat) - 0.0), epsilon) << "Architecture used 'pi' is different than expected one.";
    EXPECT_LT(std::fabs(std::cos(gepard::piFloat) - (-1.0)), epsilon) << "Architecture used 'pi' is different than expected one.";
    EXPECT_LT(std::fabs(std::cos(-gepard::piFloat) - (-1.0)), epsilon) << "Architecture used 'pi' is different than expected one.";
}

TEST(BasicMathFuncTest, Clamp)
{
    EXPECT_EQ(int(-1), gepard::clamp(int(-2), int(-1), int(1)));
    EXPECT_EQ(int(0), gepard::clamp(int(-0), int(-1), int(1)));
    EXPECT_EQ(int(1), gepard::clamp(int(2), int(-1), int(1)));
    EXPECT_EQ(gepard::Float(-1.0), gepard::clamp(gepard::Float(-2.0), gepard::Float(-1.0), gepard::Float(1.0)));
    EXPECT_EQ(gepard::Float(0.0), gepard::clamp(gepard::Float(-0.0), gepard::Float(-1.0), gepard::Float(1.0)));
    EXPECT_EQ(gepard::Float(1.0), gepard::clamp(gepard::Float(2.0), gepard::Float(-1.0), gepard::Float(1.0)));
}

} // anonymous namespace

#endif // GEPARD_FLOAT_TESTS_H
