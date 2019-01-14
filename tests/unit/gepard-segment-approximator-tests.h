/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2018, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_SEGMENT_TESTS_H
#define GEPARD_SEGMENT_TESTS_H

#include "gepard-segment-approximator.h"
#include "gtest/gtest.h"

namespace {

TEST(SegmentApproximatorTest, Constructor)
{
    gepard::SegmentApproximator sa;

    EXPECT_EQ(sa.kAntiAliasLevel, GD_ANTIALIAS_LEVEL) << "";
    EXPECT_EQ(gepard::SegmentApproximator(4).kAntiAliasLevel, 4) << "";
    EXPECT_EQ(gepard::SegmentApproximator(0).kAntiAliasLevel, GD_ANTIALIAS_LEVEL) << "";
    EXPECT_EQ(gepard::SegmentApproximator(-10).kAntiAliasLevel, GD_ANTIALIAS_LEVEL) << "";
}

static const double pi = 2.0 * std::asin(1.0);

TEST(SegmentApproximatorTest, InsertOneLine)
{
    gepard::SegmentApproximator sa;
    const gepard::FloatPoint from(-pi, -pi);
    const gepard::FloatPoint to(pi, pi);

    sa.insertLine(from, to);

    const gepard::SegmentTree& st = sa.segments();

//    EXPECT_EQ(st.size(), 2u) << "";

//    EXPECT_NE(st.find(std::floor(sa.kAntiAliasLevel * (-pi))), st.end()) << "";
//    EXPECT_EQ(st.at(std::floor(sa.kAntiAliasLevel * (-pi))).size(), 1u) << "";

//    EXPECT_NE(st.find(std::floor(sa.kAntiAliasLevel * (pi))), st.end()) << "";
//    EXPECT_EQ(st.at(std::floor(sa.kAntiAliasLevel * (pi))).size(), 0u) << "";
}

} // anonymous namespace

#endif // GEPARD_SEGMENT_TESTS_H
