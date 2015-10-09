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
    gepard::Float leftSideCrossBottomRight = (trpzd.bottomLeft - trpzd.bottomRight) * dY;
    gepard::Float rightSideCrossBottomLeft = (trpzd.bottomRight - trpzd.bottomLeft) * dY;

    gepard::Float leftSideCrossXLeft = (trpzd.bottomLeft - trpzd.topLeft) * (y - trpzd.top) - (xLeft - trpzd.topLeft) * dY;
    gepard::Float rightSideCrossXLeft = (trpzd.bottomRight - trpzd.topRight) * (y - trpzd.top) - (xLeft - trpzd.topRight) * dY;
    gepard::Float leftSideCrossXRight = (trpzd.bottomLeft - trpzd.topLeft) * (y - trpzd.top) - (xRight - trpzd.topLeft) * dY;
    gepard::Float rightSideCrossXRight = (trpzd.bottomRight - trpzd.topRight) * (y - trpzd.top) - (xRight - trpzd.topRight) * dY;

    // 'xLeft' and 'xRight' are on different sides of either side of the trapezoid.
    if (NCHECKANDLOG(leftSideCrossXLeft * leftSideCrossXRight < 0 || rightSideCrossXLeft * rightSideCrossXRight < 0))
        return false;

    // The xLeft is outside.
    if (CHECKANDLOG((leftSideCrossBottomRight * leftSideCrossXLeft < 0) || (rightSideCrossBottomLeft * rightSideCrossXLeft < 0)))
        return true;

    // The xRight is outside.
    if (CHECKANDLOG((leftSideCrossBottomRight * leftSideCrossXRight < 0) || (rightSideCrossBottomLeft * rightSideCrossXRight < 0)))
        return true;

    return false;
}

static bool hasIntersectionOrWrongTrapezoids(const gepard::Trapezoid& trpzd1, const gepard::Trapezoid& trpzd2)
{
    if (!((trpzd1.bottom < trpzd1.top)
        && (trpzd2.bottom < trpzd2.top)
        && (trpzd1.bottomLeft <= trpzd1.bottomRight)
        && (trpzd1.topLeft <= trpzd1.topRight)
        && (trpzd2.bottomLeft <= trpzd2.bottomRight)
        && (trpzd2.topLeft <= trpzd2.topRight)))
        return true;

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

static const gepard::Trapezoid trapezoids[1][2] = {
    { {0, 0, 10, 10, 0, 10}, {0, 10, 20, 10, 10, 20} }
};

void test_tessallation()
{
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
    //g.quadraticCurveTo(20, 10, 30, 20);
    //g.bezierCurveTo(30, 30, 10, 30, 10, 20);
    g.closePath();

    // Testing the hasIntersection function.
    gepard::Trapezoid trpzd = {0, 0, 10, 10, 0, 10};
    trpzd.top = trpzd.bottom - 5;
    CHECKANDLOG(hasIntersectionOrWrongTrapezoids(trpzd, trpzd));
    trpzd.top = 20;
    trpzd.bottomLeft = trpzd.bottomRight + 5;
    CHECKANDLOG(hasIntersectionOrWrongTrapezoids(trpzd, trpzd));
    trpzd.bottomLeft = 1;
    trpzd.topLeft = trpzd.topRight + 5;
    CHECKANDLOG(hasIntersectionOrWrongTrapezoids(trpzd, trpzd));
    logOn = false;
    NCHECKANDLOG(hasIntersectionOrWrongTrapezoids(trapezoids[0][0], trapezoids[0][1]));
    NCHECKANDLOG(hasIntersectionOrWrongTrapezoids(trapezoids[0][1], trapezoids[0][0]));
    logOn = true;

    if (HASFAIL)
        return;

    // Test the correctness of the tessallation is generated by the given trapezoidList.
    bool pass = true;
    gepard::TrapezoidTessallator tt(g.path());
    gepard::TrapezoidList trapezoidList = tt.trapezoidList();
    for (gepard::TrapezoidList::iterator trapezoid = trapezoidList.begin(); pass && trapezoid != trapezoidList.end(); ++trapezoid) {
        gepard::TrapezoidList::iterator testTrapezoid = trapezoid;
        for (++testTrapezoid; pass && testTrapezoid != trapezoidList.end(); ++testTrapezoid) {
            if (NCHECKANDMSG(hasIntersectionOrWrongTrapezoids(*trapezoid, *testTrapezoid), "There's an intersection between trapezoids: " << *trapezoid << " " << *testTrapezoid))
                pass = false;
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
