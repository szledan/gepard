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
#include "config.h"
#include "gepard-unit-test.h"

#include <iostream>

enum PathAPIAndMethods {
    // path API
    w3c_beginPath,
    w3c_fill,
    w3c_stroke,
    w3c_drawFocusIfNeeded,
    w3c_clip,
    w3c_isPointInPath,

    // shared path API methods
    w3c_closePath,
    w3c_moveTo,
    w3c_lineTo,
    w3c_quadraticCurveTo,
    w3c_bezierCurveTo,
    w3c_arcTo,
    w3c_rect,
    w3c_arc,

    w3c_numberOfFunctions,
};

inline int callPathFunction(gepard::Gepard &g, int t, float x = 0, float y = 0)
{
    switch (t) {
    case w3c_beginPath: g.beginPath(); break;
    case w3c_fill: g.fill(); break;
    case w3c_stroke: g.stroke(); break;
    case w3c_drawFocusIfNeeded: g.drawFocusIfNeeded(); break;
    case w3c_clip: g.clip(); break;
    case w3c_isPointInPath: g.isPointInPath(x, y); break;
    case w3c_closePath: g.closePath(); break;
    case w3c_moveTo: g.moveTo(x, y); break;
    case w3c_lineTo: g.lineTo(x, y); break;
    case w3c_quadraticCurveTo: g.quadraticCurveTo(y, x, x, y); break;
    case w3c_bezierCurveTo: g.bezierCurveTo(x + y, y, x, y + x, x, y); break;
// TODO: Not implemented
//    case w3c_arcTo: g.arcTo(x, y, x, y, x); break;
//    case w3c_rect: g.rect(y, x, x, y); break;
//    case w3c_arc: g.arc(x, y, x, y, x, true); break;
    default:
        g.lineTo(x, y);
        break;
    }
    return t;
}

inline static int callPathFunctionRand(gepard::Gepard &g, int from = 0, int to = 7, float x = NAN, float y = NAN)
{
    return callPathFunction(g, rand() % (to - from) + from, x, y);
}

void test_beginPath()
{
    {
        gepard::Gepard g(0);
        TESTCASE(EQ(g.path(), nullptr), "Path was created");
    }

    {
        gepard::Gepard g(0);
        g.beginPath();

        for (int type = w3c_beginPath; type < w3c_numberOfFunctions; ++type) {
            gepard::Gepard gi(0);
            callPathFunction(gi, type);
            if (type == w3c_beginPath || type >w3c_closePath) {
                TESTCASE(NEQ(g.path(), nullptr), "Path was not created");
            } else {
                TESTCASE(EQ(gi.path(), nullptr), "Path was created, where type is " << type);
            }
        }
    }

}

void test_moveTo()
{
    for (int type = w3c_moveTo; type < w3c_numberOfFunctions; ++type) {
        gepard::Gepard g(0);
        callPathFunction(g, type);
        TESTCASE(g.path()->pathData().firstElement()->isMoveTo(), "The type of first element isn't a 'MoveTo'. Type is:" << type);
    }

    for (int type = w3c_moveTo; type < w3c_numberOfFunctions; ++type) {
        gepard::Gepard g(0);
        g.beginPath();
        callPathFunction(g, type);
        TESTCASE(g.path()->pathData().firstElement()->isMoveTo(), "The type of first element isn't a 'MoveTo'. Type is:" << type);
    }
}

void test_closeSubpath()
{
    {
        gepard::Gepard g(0);
        g.beginPath();
        g.closePath();
        TESTCASE(EQ(g.path()->pathData().lastElement(), nullptr), "Empty path contains something.");
    }

    {
        gepard::Gepard g(0);
        g.beginPath();
        g.moveTo(0, 0);
        g.closePath();
        TESTCASE(EQ(g.path()->pathData().lastElement()->type, gepard::PathElementTypes::CloseSubpath), "The last element isn't a 'CloseSubpath'.");
    }
}

void test_path()
{
    gepard::Gepard g(0);

    for (int i = w3c_beginPath; i < 100; ++i) {
        // Testing all W3C CanvasPath functions randomly.
        callPathFunctionRand(g, w3c_beginPath, w3c_numberOfFunctions);
    }

    TESTCASE(EQ(g.path()->pathData().lastElement()->type, gepard::PathElementTypes::CloseSubpath), "The last element isn't a 'CloseSubpath'.");
}

inline static bool arePointsOutside(const gepard::Trapezoid& trpzd, const gepard::Float& y, const gepard::Float& xLeft, const gepard::Float& xRight)
{
    if (y < trpzd.bottomY || y > trpzd.topY)
        return true;

    gepard::Float dY = (trpzd.bottomY - trpzd.topY);
    ASSERT(dY);
    gepard::Float dX = (trpzd.bottomLeftX - trpzd.bottomRightX) ? trpzd.bottomLeftX - trpzd.bottomRightX : trpzd.bottomLeftX - trpzd.topRightX;
    gepard::Float leftSideCrossBottomRight = dX * dY;

    gepard::Float leftSideCrossXLeft = (trpzd.bottomLeftX - trpzd.topLeftX) * (y - trpzd.topY) - (xLeft - trpzd.topLeftX) * dY;
    gepard::Float rightSideCrossXLeft = (trpzd.bottomRightX - trpzd.topRightX) * (y - trpzd.topY) - (xLeft - trpzd.topRightX) * dY;
    gepard::Float leftSideCrossXRight = (trpzd.bottomLeftX - trpzd.topLeftX) * (y - trpzd.topY) - (xRight - trpzd.topLeftX) * dY;
    gepard::Float rightSideCrossXRight = (trpzd.bottomRightX - trpzd.topRightX) * (y - trpzd.topY) - (xRight - trpzd.topRightX) * dY;

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
    if (!((trpzd1.bottomY < trpzd1.topY)
        && (trpzd2.bottomY < trpzd2.topY)
        && (trpzd1.bottomLeftX <= trpzd1.bottomRightX)
        && (trpzd1.topLeftX <= trpzd1.topRightX)
        && (trpzd2.bottomLeftX <= trpzd2.bottomRightX)
        && (trpzd2.topLeftX <= trpzd2.topRightX)))
        return true;

    // Test intersection.
    if (trpzd1.bottomY >= trpzd2.topY || trpzd1.topY <= trpzd2.bottomY)
        return false;

    gepard::Float aMinX = (trpzd1.bottomLeftX < trpzd1.topLeftX) ? trpzd1.bottomLeftX : trpzd1.topLeftX;
    gepard::Float aMaxX = (trpzd1.bottomRightX > trpzd1.topRightX) ? trpzd1.bottomRightX : trpzd1.topRightX;
    gepard::Float bMinX = (trpzd2.bottomLeftX < trpzd2.topLeftX) ? trpzd2.bottomLeftX : trpzd2.topLeftX;
    gepard::Float bMaxX = (trpzd2.bottomRightX > trpzd2.topRightX) ? trpzd2.bottomRightX : trpzd2.topRightX;

    if (aMinX >= bMaxX || aMaxX <= bMinX)
        return false;

    return !(CHECKANDMSG(arePointsOutside(trpzd1, trpzd2.bottomY, trpzd2.bottomLeftX, trpzd2.bottomRightX), "(" << trpzd1 << ";" << trpzd2.bottomY << "," << trpzd2.bottomLeftX << "," << trpzd2.bottomRightX)
             && CHECKANDMSG(arePointsOutside(trpzd1, trpzd2.topY, trpzd2.topLeftX, trpzd2.topRightX), "(" << trpzd1 << ";" << trpzd2.topY << "," << trpzd2.topLeftX << "," << trpzd2.topRightX)
             && CHECKANDMSG(arePointsOutside(trpzd2, trpzd1.bottomY, trpzd1.bottomLeftX, trpzd1.bottomRightX), "(" << trpzd2 << ";" << trpzd1.bottomY << "," << trpzd1.bottomLeftX << "," << trpzd1.bottomRightX)
             && CHECKANDMSG(arePointsOutside(trpzd2, trpzd1.topY, trpzd1.topLeftX, trpzd1.topRightX), "(" << trpzd2 << ";" << trpzd1.topY << "," << trpzd1.topLeftX << "," << trpzd1.topRightX));
}

void test_tessallation()
{
    // 1. Test the correctness of the 'hasIntersectionOrWrongTrapezoids' function.
    // The gepard::Trapezoid is {bottomY, bottomLeftX, bottomRightX, topY, topLeftX, topRightX}.
    gepard::Trapezoid trpzd = {0, 0, 10, 10, 0, 10};
    // Test of wrong trapezoid catch.
    {
        trpzd.topY = trpzd.bottomY - 5;
        TESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, trpzd), "Testing of wrong trapezoid catch 1, " << trpzd);
        trpzd.topY = 20;
        trpzd.bottomLeftX = trpzd.bottomRightX + 5;
        TESTCASE(hasIntersectionOrWrongTrapezoids(trpzd, trpzd), "Testing of wrong trapezoid catch 2, " << trpzd);
        trpzd.bottomLeftX = 1;
        trpzd.topLeftX = trpzd.topRightX + 5;
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
            { 2, -1, 11, 11, -1, 11},   //  5. No corners are inside but bottomY edge is crossing it.
            {-1, -1, 11,  9, -1, 11},   //  6. No corners are inside but topY edge is crossing it.
            { 2, -1, 11,  9, -1, 11},   //  7. No corners are inside but bottomY and topY edges are crossing it.
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
            {-1,  0, 10,  0,  0, 10},   //  1. Contact with bottomY edge.
            {10,  0, 10, 11,  0, 10},   //  2. Contact with topY edge.
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
    gepard::TrapezoidTessellator tt(g.path());
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
    INITIALIZE();

    TEST(test_beginPath());
    TEST(test_moveTo());
    TEST(test_closeSubpath());
    TEST(test_tessallation());

    FINALIZE();

    return EXIT_CODE();
}
