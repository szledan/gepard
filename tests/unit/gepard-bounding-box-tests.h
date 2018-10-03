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

#ifndef GEPARD_BOUNDING_BOX_TESTS_H
#define GEPARD_BOUNDING_BOX_TESTS_H

#include "gepard-bounding-box.h"
#include "gtest/gtest.h"

namespace {

TEST(BoundingBoxTest, Initialize)
{
    gepard::BoundingBox bb;

    bb.stretchX(NAN);
    bb.stretchY(NAN);

    EXPECT_EQ(INFINITY, bb.minX);
    EXPECT_EQ(INFINITY, bb.minY);
    EXPECT_EQ(-INFINITY, bb.maxX);
    EXPECT_EQ(-INFINITY, bb.maxY);
}

TEST(BoundingBoxTest, Stretch)
{
    gepard::BoundingBox bb;
    bb.stretchX(1.0);
    EXPECT_EQ(1.0, bb.minX);
    EXPECT_EQ(1.0, bb.maxX);

    bb.stretchY(1.0);
    EXPECT_EQ(1.0, bb.minY);
    EXPECT_EQ(1.0, bb.maxY);

    bb.stretch(gepard::FloatPoint(0.0, 0.5));
    EXPECT_EQ(0.0, bb.minX);
    EXPECT_EQ(0.5, bb.minY);

    bb.stretch(gepard::FloatPoint(2.0, 2.5));
    EXPECT_EQ(2.0, bb.maxX);
    EXPECT_EQ(2.5, bb.maxY);
}

} // anonymous namespace

#endif // GEPARD_BOUNDING_BOX_TESTS_H
