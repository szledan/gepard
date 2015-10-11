/* Copyright (C) 2015, Szilard Ledan <szledan@gmail.com>
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

#include "gepard.h"
#include "gepard-unit-test.h"

#include <assert.h>
#include <iostream>

void test_beginPath01()
{
    gepard::Gepard g(0);
    g.beginPath();
    TESTCASE(NEQ(g.path(), nullptr), "Path was not created");
}

//bool test_beginPath02()
//{
//    bool result = true;
//    gepard::Gepard g(0);
//    g.beginPath();
//    g.moveTo(0, 0);
//    g.beginPath();
//    return result;
//}

void test_closeSubpath()
{
    gepard::Gepard g(0);
    g.beginPath();
    g.closePath();
    TESTCASE(EQ(g.path()->pathData().lastElement(), nullptr), "Empty path contains something.");

    g.moveTo(0, 0);
    g.closePath();
    TESTCASE(EQ(g.path()->pathData().lastElement()->type, gepard::PathElementTypes::CloseSubpath), "The last element isn't a 'CloseSubpath'.");

    int n = 100;

    g.moveTo(0, 0);
    for (int i = 0; i < n; ++i) {
        switch (i % 10) {
        case 0:
            g.moveTo(i, n);
            break;
        case 1:
            g.lineTo(i, n);
            break;
        case 2:
            g.closePath();
            break;
        default:
            g.lineTo(i, n);
            break;
        }
    }
    g.fill();

    TESTCASE(EQ(g.path()->pathData().lastElement()->type, gepard::PathElementTypes::CloseSubpath), "The last element isn't a 'CloseSubpath'.");
}

inline static bool arePointsOutside(const gepard::Trapezoid& trpzd, const gepard::Float& y, const gepard::Float& xLeft, const gepard::Float& xRight)
{
    if (y < trpzd.bottom || y > trpzd.top)
        return true;

    gepard::Float dY = (trpzd.bottom - trpzd.top);
    assert(dY);
    gepard::Float dX = (trpzd.bottomLeft - trpzd.bottomRight) ? trpzd.bottomLeft - trpzd.bottomRight : trpzd.bottomLeft - trpzd.topRight;
    gepard::Float leftSideCrossBottomRight = dX * dY;

    gepard::Float leftSideCrossXLeft = (trpzd.bottomLeft - trpzd.topLeft) * (y - trpzd.top) - (xLeft - trpzd.topLeft) * dY;
    gepard::Float rightSideCrossXLeft = (trpzd.bottomRight - trpzd.topRight) * (y - trpzd.top) - (xLeft - trpzd.topRight) * dY;
    gepard::Float leftSideCrossXRight = (trpzd.bottomLeft - trpzd.topLeft) * (y - trpzd.top) - (xRight - trpzd.topLeft) * dY;
    gepard::Float rightSideCrossXRight = (trpzd.bottomRight - trpzd.topRight) * (y - trpzd.top) - (xRight - trpzd.topRight) * dY;

    // 'xLeft' and 'xRight' are on different sides of either side of the trapezoid.
    if (NCHECKANDLOG(leftSideCrossXLeft * leftSideCrossXRight < 0 || rightSideCrossXLeft * rightSideCrossXRight < 0))
        return false;

    // The xLeft is outside.
    if (CHECKANDLOG((leftSideCrossBottomRight * leftSideCrossXLeft < 0)
                    || (-leftSideCrossBottomRight * rightSideCrossXLeft < 0)
                    || (!dX && leftSideCrossXLeft && rightSideCrossXLeft)))
        return true;

    // The xRight is outside.
    if (CHECKANDLOG((leftSideCrossBottomRight * leftSideCrossXRight < 0)
                    || (-leftSideCrossBottomRight * rightSideCrossXRight < 0)
                    || (!dX && leftSideCrossXRight && rightSideCrossXRight)))
        return true;

    return false;
}

static bool hasIntersectionOrWrongTrapezoids(const gepard::Trapezoid& trpzd1, const gepard::Trapezoid& trpzd2)
{
    // Wrong trapezoid.
    if (!((trpzd1.bottom < trpzd1.top)
        && (trpzd2.bottom < trpzd2.top)
        && (trpzd1.bottomLeft <= trpzd1.bottomRight)
        && (trpzd1.topLeft <= trpzd1.topRight)
        && (trpzd2.bottomLeft <= trpzd2.bottomRight)
        && (trpzd2.topLeft <= trpzd2.topRight)))
        return true;

    // Test intersection.
    if (trpzd1.bottom >= trpzd2.top || trpzd1.top <= trpzd2.bottom)
        return false;

    gepard::Float aMinX = (trpzd1.bottomLeft < trpzd1.topLeft) ? trpzd1.bottomLeft : trpzd1.topLeft;
    gepard::Float aMaxX = (trpzd1.bottomRight > trpzd1.topRight) ? trpzd1.bottomRight : trpzd1.topRight;
    gepard::Float bMinX = (trpzd2.bottomLeft < trpzd2.topLeft) ? trpzd2.bottomLeft : trpzd2.topLeft;
    gepard::Float bMaxX = (trpzd2.bottomRight > trpzd2.topRight) ? trpzd2.bottomRight : trpzd2.topRight;

    if (aMinX >= bMaxX || aMaxX <= bMinX)
        return false;

    return !(CHECKANDMSG(arePointsOutside(trpzd1, trpzd2.bottom, trpzd2.bottomLeft, trpzd2.bottomRight), "(" << trpzd1 << ";" << trpzd2.bottom << "," << trpzd2.bottomLeft << "," << trpzd2.bottomRight)
             && CHECKANDMSG(arePointsOutside(trpzd1, trpzd2.top, trpzd2.topLeft, trpzd2.topRight), "(" << trpzd1 << ";" << trpzd2.top << "," << trpzd2.topLeft << "," << trpzd2.topRight)
             && CHECKANDMSG(arePointsOutside(trpzd2, trpzd1.bottom, trpzd1.bottomLeft, trpzd1.bottomRight), "(" << trpzd2 << ";" << trpzd1.bottom << "," << trpzd1.bottomLeft << "," << trpzd1.bottomRight)
             && CHECKANDMSG(arePointsOutside(trpzd2, trpzd1.top, trpzd1.topLeft, trpzd1.topRight), "(" << trpzd2 << ";" << trpzd1.top << "," << trpzd1.topLeft << "," << trpzd1.topRight));
}

void test_tessallation()
{
    // 1. Test the correctness of the 'hasIntersectionOrWrongTrapezoids' function.
    // The gepard::Trapezoid is {bottomY, bottomLeftX, bottomRightX, topY, topLeftX, topRightX}.
    gepard::Trapezoid trpzd = {0, 0, 10, 10, 0, 10};
    // Test of wrong trapezoid catch.
    {
        trpzd.top = trpzd.bottom - 5;
        TESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, trpzd), "Testing of wrong trapezoid catch 1, " << trpzd);
        trpzd.top = 20;
        trpzd.bottomLeft = trpzd.bottomRight + 5;
        TESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, trpzd), "Testing of wrong trapezoid catch 2, " << trpzd);
        trpzd.bottomLeft = 1;
        trpzd.topLeft = trpzd.topRight + 5;
        TESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, trpzd), "Testing of wrong trapezoid catch 3, " << trpzd);
    }
    // Testing the hasIntersectionOrWrongTrapezoids function.
    {
        trpzd = {0, 0, 10, 10, 0, 10}; // Basic trapezoid.
        const int numberOfIntersectionTrapezoids = 12;
        const gepard::Trapezoid intersectionTrpzds[numberOfIntersectionTrapezoids] = {
            { 1,  1, 11,  9, 10, 11},   //  1. One corner point is inside.
            { 1,  1, 11,  9,  1, 11},   //  2. Two corner points are inside.
            { 1,  1,  9,  9,  1, 11},   //  3. Three corner points are inside.
            { 1,  1,  9,  9,  1,  9},   //  4. Fully inside.
            { 2, -1, 11, 11, -1, 11},   //  5. No corners are inside but bottom edge is crossing it.
            {-1, -1, 11,  9, -1, 11},   //  6. No corners are inside but top edge is crossing it.
            { 2, -1, 11,  9, -1, 11},   //  7. No corners are inside but bottom and top edges are crossing it.
            {-1,  2, 11, 11,  2, 11},   //  8. No corners are inside but left edge is crossing it.
            {-1, -1,  9, 11, -1,  9},   //  9. No corners are inside but right edge is crossing it.
            {-1,  2,  9, 11,  2,  9},   // 10. No corners are inside but left and right edges are crossing it.
            { 0,  0, 10, 10,  0, 10},   // 11. Same.
            {-1, -1, 12, 11, 11, 12},   // 12. Diagonal crossing.
        };
        for (int i = 1; i < numberOfIntersectionTrapezoids; ++i) {
            TESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, intersectionTrpzds[i]), i << ". trapezoid test: " << trpzd << " " << intersectionTrpzds[i]);
            TESTCASE(hasIntersectionOrWrongTrapezoids(intersectionTrpzds[i], trpzd), i << ". trapezoid test: " << intersectionTrpzds[i] << " " << trpzd);
        }
        const int numberOfOutsideTrapezoids = 4;
        const gepard::Trapezoid outsideTrpzds[numberOfOutsideTrapezoids] = {
            {-1,  0, 10,  0,  0, 10},   //  1. Contact with bottom edge.
            {10,  0, 10, 11,  0, 10},   //  2. Contact with top edge.
            { 0, -1,  0, 10, -1,  0},   //  3. Contact with left edge.
            { 0, 10, 11, 10, 10, 11},   //  4. Contact with right edge.
        };
        for (int i = 1; i < numberOfOutsideTrapezoids; ++i) {
            NTESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, outsideTrpzds[i]), i << ". trapezoid test: " << trpzd << " " << outsideTrpzds[i]);
            NTESTCASE(hasIntersectionOrWrongTrapezoids(outsideTrpzds[i], trpzd), i << ". trapezoid test: " << outsideTrpzds[i] << " " << trpzd);
        }
        // Some extreme cases.
        trpzd = {0, 5, 5, 10, 0, 10};
        gepard::Trapezoid testTrpzd = {0, 6, 12, 10, 11, 12};
        NTESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, testTrpzd), " : " << trpzd << " " << testTrpzd);
        testTrpzd = {1, 0, 10, 9, 0, 10};
        TESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, testTrpzd), " : " << trpzd << " " << testTrpzd);
        trpzd = {0, 5, 5, 10, 10, 10};
        testTrpzd = {1, 0, 10, 9, 0, 10};
        TESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, testTrpzd), " : " << trpzd << " " << testTrpzd);
        testTrpzd = {1, 0, 2, 2, 0, 2};
        NTESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, testTrpzd), " : " << trpzd << " " << testTrpzd);
    }
    if (HASFAIL)
        return;

    // 2. Test the correctness of the tessallation is generated by the given trapezoidList.
    gepard::Gepard g(nullptr);
    g.beginPath();
    g.moveTo(10, 0);
    g.lineTo(0, 50);
    g.lineTo(10, 50);
    g.lineTo(12, 40);
    g.lineTo(28, 40);
    g.lineTo(30, 50);
    g.lineTo(40, 50);
    g.lineTo(30, 0);
    g.lineTo(20, 0);
    g.lineTo(20, 5);
    g.lineTo(20, 0);
    g.moveTo(10, 20);
    g.quadraticCurveTo(20, 10, 30, 20);
    g.bezierCurveTo(30, 30, 10, 30, 10, 20);
    g.closePath();

    bool pass = true;
    gepard::TrapezoidTessallator tt(g.path());
    gepard::TrapezoidList trapezoidList = tt.trapezoidList();
    for (gepard::TrapezoidList::iterator trapezoid = trapezoidList.begin(); pass && trapezoid != trapezoidList.end(); ++trapezoid) {
        gepard::TrapezoidList::iterator testTrapezoid = trapezoid;
        for (++testTrapezoid; pass && testTrapezoid != trapezoidList.end(); ++testTrapezoid) {
            pass = CHECKANDMSG(!hasIntersectionOrWrongTrapezoids(*trapezoid, *testTrapezoid), "There's an intersection between trapezoids: " << *trapezoid << " " << *testTrapezoid);
        }
    }
    TESTCASE(pass, "There's an intersection between trapezoids.");
}

int main()
{
    TEST(test_beginPath01());
//    TEST(test_beginPath02());
//    TEST(test_closeSubpath());
    TEST(test_tessallation());

    FINALIZE();

    return EXIT_CODE();
}
