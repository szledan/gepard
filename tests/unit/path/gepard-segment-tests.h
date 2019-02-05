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

#include "gepard-segment.h"
#include "gtest/gtest.h"
#include <cmath>
#include <limits>

namespace {

TEST(SegmentTest, Constructor)
{
    const gepard::UIntID currentUID = gepard::s_uid;
    const gepard::Segment segments[3] = {
        gepard::Segment(gepard::FloatPoint(1.0, 2.0), gepard::FloatPoint(3.0, 4.0)),
        gepard::Segment(gepard::FloatPoint(1.0, 2.0), gepard::FloatPoint(3.0, 4.0), 10),
        gepard::Segment(gepard::FloatPoint(1.0, 2.0), gepard::FloatPoint(3.0, 4.0), 10, 5.0),
    };

    for (int i = 0; i < sizeof(segments) / sizeof(gepard::Segment); ++i) {
        EXPECT_EQ(segments[i].from().x, 1.0) << "Step: " << i << "";
        EXPECT_EQ(segments[i].from().y, 2.0) << "Step: " << i << "";
        EXPECT_EQ(segments[i].to().x, 3.0) << "Step: " << i << "";
        EXPECT_EQ(segments[i].to().y, 4.0) << "Step: " << i << "";
        EXPECT_EQ(segments[i].uid(), i > 0 ? 10 : currentUID) << "Step: " << i << "";
        EXPECT_EQ(segments[i].slope(), i == 2 ? 5.0 : (segments[i].to().x - segments[i].from().x) / (segments[i].to().y - segments[i].from().y)) << "Step: " << i << "";
    }
}

static const double pi = 2.0 * std::asin(1.0);

TEST(SegmentTest, GettersA)
{
    const gepard::Segment segment(gepard::FloatPoint(-pi, pi), gepard::FloatPoint(pi, -pi));

    EXPECT_EQ(segment.from().x, pi) << "";
    EXPECT_EQ(segment.from().y, -pi) << "";
    EXPECT_EQ(segment.to().x, -pi) << "";
    EXPECT_EQ(segment.to().y, pi) << "";

    EXPECT_EQ(segment.top().x, pi) << "";
    EXPECT_EQ(segment.top().y, -4.0) << "";
    EXPECT_EQ(segment.bottom().x, -pi) << "";
    EXPECT_EQ(segment.bottom().y, 3.0) << "";

    EXPECT_EQ(segment.topY(), -4.0) << "";
    EXPECT_EQ(segment.bottomY(), 3.0) << "";

    EXPECT_EQ(segment.slope(), -1.0) << "";

    EXPECT_EQ(segment.direction(), gepard::Segment::Negative) << "";
}

TEST(SegmentTest, GettersB)
{
    const gepard::Segment segment(gepard::FloatPoint(-pi, -pi), gepard::FloatPoint(pi, pi));

    EXPECT_EQ(segment.from().x, -pi) << "";
    EXPECT_EQ(segment.from().y, -pi) << "";
    EXPECT_EQ(segment.to().x, pi) << "";
    EXPECT_EQ(segment.to().y, pi) << "";

    EXPECT_EQ(segment.top().x, -pi) << "";
    EXPECT_EQ(segment.top().y, -4.0) << "";
    EXPECT_EQ(segment.bottom().x, pi) << "";
    EXPECT_EQ(segment.bottom().y, 3.0) << "";

    EXPECT_EQ(segment.topY(), -4.0) << "";
    EXPECT_EQ(segment.bottomY(), 3.0) << "";

    EXPECT_EQ(segment.slope(), 1.0) << "";

    EXPECT_EQ(segment.direction(), gepard::Segment::Positive) << "";
}

} // anonymous namespace

#endif // GEPARD_SEGMENT_TESTS_H
