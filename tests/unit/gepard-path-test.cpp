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

void test_beginPath01()
{
    gepard::Gepard g(0);
    g.beginPath();
    TEST(NEQ(g.path(), nullptr), "Path was not created");
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
    TEST(EQ(g.path()->pathData().lastElement(), nullptr), "");

    g.moveTo(0, 0);
    g.closePath();
    TEST(EQ(g.path()->pathData().lastElement()->type, gepard::PathElementTypes::CloseSubpath), "");

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

    TEST(EQ(g.path()->pathData().lastElement()->type, gepard::PathElementTypes::CloseSubpath), "");
}

void test_approxximator()
{
    gepard::XGepardSurface XSurface(600, 600);

    gepard::Gepard g(&XSurface);
    g.beginPath();
    g.moveTo(1, 0);
    g.lineTo(0, 5);
    g.lineTo(1, 5);
    g.lineTo(1.2, 4);
    g.lineTo(2.8, 4);
    g.lineTo(3, 5);
    g.lineTo(4, 5);
    g.lineTo(3, 0);
    g.moveTo(1.6, 2);
    g.lineTo(2.6, 3);
    g.lineTo(1.4, 3);
    g.lineTo(2.4, 2);
    g.moveTo(11.6, 2);
    g.lineTo(12.6, 3);
    g.lineTo(11.4, 3);
    g.lineTo(12.4, 2);
    g.moveTo(21.6, 2);
    g.lineTo(22.6, 3);
    g.lineTo(21.4, 3);
    g.lineTo(22.4, 2);
    g.closePath();

    // FIXME: testing!
}

int main()
{
    TESTER(test_beginPath01());
//    TESTER(test_beginPath02);
    TESTER(test_closeSubpath());
    TESTER(test_approxximator());

    FINALIZER();

    return EXIT_CODE();
}
