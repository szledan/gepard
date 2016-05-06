/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2016, Szilard Ledan <szledan@gmail.com>
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
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#include "gepard-types.h"
#include "gtest/gtest.h"

namespace {

/* Float tests */

TEST(FloatTest, ConstValuePrecisionOfFloat)
{
    ASSERT_NE(0.0, gepard::precisionOfFloat) << "The precisionOfFloat value is 0.";
    EXPECT_LT(0.0, gepard::precisionOfFloat) << "The precisionOfFloat is non-pozitive.";
    ASSERT_NE(INFINITY, gepard::precisionOfFloat) << "The precisionOfFloat is INFINITY.";
    ASSERT_NE(NAN, gepard::precisionOfFloat) << "The precisionOfFloat is NAN.";
}

TEST(FloatTest, FixPrecision)
{
    gepard::Float g = 1.0 / gepard::precisionOfFloat;
    gepard::Float h = 1.99 / gepard::precisionOfFloat;
    EXPECT_EQ(gepard::fixPrecision(g), gepard::fixPrecision(h)) << "The fixPrecision() rounds wrong.";

    g = 0.0 / gepard::precisionOfFloat;
    h = 0.99 / gepard::precisionOfFloat;
    EXPECT_EQ(gepard::fixPrecision(g), gepard::fixPrecision(h)) << "The fixPrecision() rounds wrong on positive numbers.";

    g = -1.0 / gepard::precisionOfFloat;
    h = -0.99 / gepard::precisionOfFloat;
    EXPECT_EQ(gepard::fixPrecision(g), gepard::fixPrecision(h)) << "The fixPrecision() rounds wrong on negative numbers.";
}

TEST(FloatTest, PiFloat)
{
    gepard::Float pi100 = 3.1415926535897932384626433832795028841971693993751058209749445923078164062862089986280348253421170679;
    gepard::Float pi = gepard::fixPrecision(pi100);
    gepard::Float gepardPi = gepard::fixPrecision(gepard::piFloat);
    EXPECT_EQ(pi, gepardPi) << "Architecture used 'pi' is different than expected one.";
}

/* Basic math functions tests */

TEST(BasicMathFuncTest, Min)
{
    EXPECT_EQ(int(0), gepard::min(int(0), int(1)));
    EXPECT_EQ(int(-1), gepard::min(int(-1), int(0)));
    EXPECT_EQ(int(0), gepard::min(int(0), int(-0)));
    EXPECT_EQ(gepard::Float(0.0), gepard::min(gepard::Float(0.0), gepard::Float(1.0)));
    EXPECT_EQ(gepard::Float(-1.0), gepard::min(gepard::Float(-1.0), gepard::Float(0.0)));
    EXPECT_EQ(gepard::Float(0.0), gepard::min(gepard::Float(0.0), gepard::Float(-0.0)));
}

TEST(BasicMathFuncTest, Max)
{
    EXPECT_EQ(int(1), gepard::max(int(0), int(1)));
    EXPECT_EQ(int(0), gepard::max(int(-1), int(0)));
    EXPECT_EQ(int(0), gepard::max(int(0), int(-0)));
    EXPECT_EQ(gepard::Float(1.0), gepard::max(gepard::Float(0.0), gepard::Float(1.0)));
    EXPECT_EQ(gepard::Float(0.0), gepard::max(gepard::Float(-1.0), gepard::Float(0.0)));
    EXPECT_EQ(gepard::Float(0.0), gepard::max(gepard::Float(0.0), gepard::Float(-0.0)));
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

/* Region tests */

TEST(RegionTest, SizeTooBig)
{
    gepard::Region<REGION_BLOCK_SIZE> region;
    void* ptr = region.alloc(REGION_BLOCK_SIZE + 1);

    EXPECT_EQ(nullptr, ptr) << "The alloc() doesn't return 'nullptr' when 'size' is bigger than REGION_BLOCK_SIZE.";
}

TEST(RegionTest, SizeIsZero)
{
    gepard::Region<REGION_BLOCK_SIZE> region;
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

/* FloatPoint tests */

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

/* BoundingBox */

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

/* Color */

} // anonymous namespace

/* Gepard types tests */

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
