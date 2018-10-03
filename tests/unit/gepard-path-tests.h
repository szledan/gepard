/* Copyright (C) 2017, Gepard Graphics
 * Copyright (C) 2017, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_PATH_TESTS_H
#define GEPARD_PATH_TESTS_H

#include "gepard-path.h"
#include "gtest/gtest.h"
#include <iostream>

namespace {

TEST(Path, EmptyPath)
{
    gepard::Path path;
    gepard::PathData& pathData= *(path.pathData());
    EXPECT_EQ(nullptr, pathData.firstElement()) << "The firstElement not nullptr on empty path.";
    EXPECT_EQ(nullptr, pathData.lastElement()) << "The lastElement not nullptr on empty path.";
}

TEST(Path, SimpleMoveTo)
{
    gepard::Path path;
    gepard::PathData& pathData= *(path.pathData());
    gepard::FloatPoint to(0.0, 0.1);
    std::size_t idx = 0;

    const gepard::PathElement* prevPathelement = pathData.lastElement();
    pathData.addMoveToElement(to);
    EXPECT_EQ(gepard::PathElementTypes::MoveTo, pathData[idx]->type) << "Wrong type.";
    EXPECT_EQ(to, pathData[idx]->to) << "Differnet point.";
    EXPECT_EQ(nullptr, pathData[idx]->next) << "The next element not nullptr.";
    EXPECT_EQ(pathData.firstElement(), pathData.lastElement()) << "Differnet element.";
    EXPECT_NE(prevPathelement, pathData.lastElement()) << "The current and previous elements are same.";
}

TEST(Path, MoreMoveTo)
{
    gepard::Path path;
    gepard::PathData& pathData= *(path.pathData());
    gepard::FloatPoint to(0.0, 0.1);
    std::size_t idx = 0;

    pathData.addMoveToElement(to);
    EXPECT_EQ(gepard::PathElementTypes::MoveTo, pathData[idx]->type) << "Wrong type.";
    EXPECT_EQ(to, pathData[idx]->to) << "Differnet point.";
    EXPECT_EQ(nullptr, pathData[idx]->next) << "The next element not nullptr.";
    EXPECT_EQ(pathData.firstElement(), pathData.lastElement()) << "Differnet element.";
    to = to + gepard::FloatPoint(1.0, 1.0);

    pathData.addMoveToElement(to);
    EXPECT_EQ(gepard::PathElementTypes::MoveTo, pathData[idx]->type) << "Wrong type.";
    EXPECT_EQ(to, pathData[idx]->to) << "Differnet point.";
    EXPECT_EQ(nullptr, pathData[idx]->next) << "The next element not nullptr.";
    EXPECT_EQ(pathData.firstElement(), pathData.lastElement()) << "Differnet element.";
    to = to + gepard::FloatPoint(1.0, 1.0);
    idx++;

    pathData.addMoveToElement(to);
    EXPECT_EQ(gepard::PathElementTypes::MoveTo, pathData[idx]->type) << "Wrong type.";
    EXPECT_EQ(to, pathData[idx]->to) << "Differnet point.";
    EXPECT_EQ(nullptr, pathData[idx]->next) << "The next element not nullptr.";
    EXPECT_EQ(pathData.firstElement(), pathData.lastElement()) << "Differnet element.";
}

TEST(Path, SimpleLineTo)
{
    gepard::Path path;
    gepard::PathData& pathData= *(path.pathData());
    gepard::FloatPoint to(0.0, 0.1);
    std::size_t idx = 0;

    // Test: https://www.w3.org/TR/2dcontext/#ensure-there-is-a-subpath
    pathData.addLineToElement(to);
    EXPECT_EQ(gepard::PathElementTypes::MoveTo, pathData[idx]->type) << "Wrong type.";
    EXPECT_EQ(to, pathData[idx]->to) << "Differnet point.";
    EXPECT_EQ(nullptr, pathData[idx]->next) << "The next element not nullptr.";
    EXPECT_EQ(pathData.firstElement(), pathData.lastElement()) << "Differnet element.";
}

TEST(Path, MoreLineTo)
{
    gepard::Path path;
    gepard::PathData& pathData= *(path.pathData());
    gepard::FloatPoint to(0.0, 0.1);
    std::size_t idx = 0;

    // Test: https://www.w3.org/TR/2dcontext/#ensure-there-is-a-subpath
    pathData.addLineToElement(to);
    EXPECT_EQ(gepard::PathElementTypes::MoveTo, pathData[idx]->type) << "Wrong type.";
    EXPECT_EQ(to, pathData[idx]->to) << "Differnet point.";
    EXPECT_EQ(nullptr, pathData[idx]->next) << "The next element not nullptr.";
    EXPECT_EQ(pathData.firstElement(), pathData.lastElement()) << "Differnet element.";
    to = to + gepard::FloatPoint(1.0, 1.0);
    idx++;

    pathData.addLineToElement(to);
    EXPECT_EQ(gepard::PathElementTypes::LineTo, pathData[idx]->type) << "Wrong type.";
    EXPECT_EQ(to, pathData[idx]->to) << "Differnet point.";
    EXPECT_EQ(nullptr, pathData[idx]->next) << "The next element not nullptr.";
    EXPECT_NE(pathData.firstElement(), pathData.lastElement()) << "Differnet element.";
    to = to + gepard::FloatPoint(1.0, 1.0);
    idx++;

    pathData.addLineToElement(to);
    EXPECT_EQ(gepard::PathElementTypes::LineTo, pathData[idx]->type) << "Wrong type.";
    EXPECT_EQ(to, pathData[idx]->to) << "Differnet point.";
    EXPECT_EQ(nullptr, pathData[idx]->next) << "The next element not nullptr.";
    EXPECT_NE(pathData.firstElement(), pathData.lastElement()) << "Differnet element.";
}

TEST(Path, SamePointLineTo)
{
    gepard::Path path;
    gepard::PathData& pathData= *(path.pathData());
    gepard::FloatPoint to(0.0, 0.1);
    std::size_t idx = 0;

    pathData.addMoveToElement(to);
    pathData.addLineToElement(to);
    EXPECT_EQ(gepard::PathElementTypes::MoveTo, pathData[idx]->type) << "Wrong type.";
    EXPECT_EQ(to, pathData[idx]->to) << "Differnet point.";
    EXPECT_EQ(nullptr, pathData[idx]->next) << "The next element not nullptr.";
    EXPECT_EQ(pathData.firstElement(), pathData.lastElement()) << "Differnet element.";
}

} // anonymous namespace

#endif // GEPARD_PATH_TESTS_H
