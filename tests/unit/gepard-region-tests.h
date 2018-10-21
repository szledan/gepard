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

#ifndef GEPARD_REGION_TESTS_H
#define GEPARD_REGION_TESTS_H

#include "gepard-region.h"
#include "gtest/gtest.h"

namespace {

TEST(RegionTest, SizeTooBig)
{
    gepard::Region<GD_REGION_BLOCK_SIZE> region;
    void* ptr = region.alloc(GD_REGION_BLOCK_SIZE + 1);

    EXPECT_EQ(nullptr, ptr) << "The alloc() doesn't return 'nullptr' when 'size' is bigger than REGION_BLOCK_SIZE.";
}

TEST(RegionTest, SizeIsZero)
{
    gepard::Region<GD_REGION_BLOCK_SIZE> region;
    void* ptr1 = region.alloc(0);
    void* ptr2 = region.alloc(0);

    EXPECT_EQ(ptr1, ptr2) << "The returned pointers are different despite 'size' was 0 in both allocations.";
}

TEST(RegionTest, SizeIsNotDefault)
{
    const size_t word = sizeof(uint8_t*);

    // One 'word' sized buffer.
    gepard::Region<1 * word> region1;

    uint8_t* ptr1 = (uint8_t*)region1.alloc(0);
    uint8_t* ptr2 = (uint8_t*)region1.alloc(0);
    EXPECT_EQ(ptr1, ptr2) << "The returned pointers are different despite 'size' was 0 in both allocations.";

    ptr1 = (uint8_t*)region1.alloc(word);
    ptr2 = (uint8_t*)region1.alloc(word);
    EXPECT_NE(ptr1, (ptr2 - word)) << "The expected item after 'ptr1' is an intenrnal 'next' pointer.";

    // Two 'word' sized buffer.
    gepard::Region<2 * word> region2;

    ptr1 = (uint8_t*)region2.alloc(word);
    ptr2 = (uint8_t*)region2.alloc(word);
    EXPECT_EQ(ptr1, (ptr2 - word));
}

} // anonymous namespace

#endif // GEPARD_REGION_TESTS_H
