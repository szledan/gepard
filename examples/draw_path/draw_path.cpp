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
#include <iostream>
#include <string>

int main()
{
    gepard::XGepardSurface xSurface(512, 512);
    gepard::Gepard gepardContext(&xSurface);

    gepardContext.beginPath();

#if 0
    gepardContext.moveTo(150, 190);
    gepardContext.lineTo(150, 210);
    gepardContext.lineTo(160, 210);
    gepardContext.lineTo(160, 190);

    gepardContext.moveTo(145, 200);
    gepardContext.lineTo(145, 201.25);
    gepardContext.lineTo(146, 201.25);
    gepardContext.lineTo(146, 200);
#elif 0

    gepardContext.moveTo(100, 20);
    gepardContext.lineTo(20, 500);
    gepardContext.lineTo(100, 500);
    gepardContext.lineTo(120, 400);
    gepardContext.lineTo(280, 400);
    gepardContext.lineTo(300, 500);
    gepardContext.lineTo(400, 500);
    gepardContext.lineTo(300, 20);
    gepardContext.lineTo(200, 20);
    gepardContext.lineTo(200, 50);
    gepardContext.lineTo(200, 20);
    gepardContext.moveTo(100, 200);
    gepardContext.quadraticCurveTo(200, 100, 300, 200);
    gepardContext.bezierCurveTo(300, 300, 100, 300, 100, 200);
    gepardContext.moveTo(50, 30);
    gepardContext.lineTo(20, 100);
    gepardContext.lineTo(300, 400);
    gepardContext.lineTo(380, 350);

#else
    // Erdely
    // E
    gepardContext.moveTo(300.0 ,300.0);
    gepardContext.lineTo(287.0 ,311.0);
    gepardContext.bezierCurveTo(349.0 ,379.0 ,347.0 ,398.0 ,286.0 ,463.0);
    gepardContext.lineTo(301.0 ,474.0);
    gepardContext.bezierCurveTo(369.0 ,404.0 ,369.0 ,370.0 ,300.0 ,300.0);
    gepardContext.moveTo(309.0 ,413.0);
    gepardContext.lineTo(294.0 ,429.0);
    gepardContext.lineTo(335.0 ,466.0);
    gepardContext.lineTo(350.0 ,451.0);
    gepardContext.lineTo(309.0 ,413.0);
    gepardContext.moveTo(335.0 ,308.0);
    gepardContext.lineTo(292.0 ,341.0);
    gepardContext.lineTo(304.0 ,357.0);
    gepardContext.lineTo(347.0 ,323.0);
    gepardContext.lineTo(335.0 ,308.0);

//    // RD
//    gepardContext.moveTo(259.0 ,307.0);
//    gepardContext.lineTo(238.0 ,308.0);
//    gepardContext.lineTo(242.0 ,469.0);
//    gepardContext.lineTo(263.0 ,469.0);
//    gepardContext.lineTo(259.0 ,307.0);
//    gepardContext.moveTo(183.0 ,307.0);
//    gepardContext.lineTo(162.0 ,307.0);
//    gepardContext.lineTo(165.0 ,469.0);
//    gepardContext.lineTo(186.0 ,469.0);
//    gepardContext.lineTo(183.0 ,307.0);
//    gepardContext.moveTo(140.0 ,344.0);
//    gepardContext.lineTo(128.0 ,361.0);
//    gepardContext.lineTo(245.0 ,439.0);
//    gepardContext.lineTo(256.0 ,421.0);
//    gepardContext.lineTo(140.0 ,344.0);

//    // E'LY
//    gepardContext.moveTo(41.0 ,342.0);
//    gepardContext.lineTo(24.0 ,354.0);
//    gepardContext.lineTo(54.0 ,396.0);
//    gepardContext.lineTo(71.0 ,383.0);
//    gepardContext.lineTo(41.0 ,342.0);
//    gepardContext.moveTo(103.0 ,431.0);
//    gepardContext.lineTo(88.0 ,446.0);
//    gepardContext.lineTo(114.0 ,470.0);
//    gepardContext.lineTo(128.0 ,455.0);
//    gepardContext.lineTo(103.0 ,431.0);
//    gepardContext.moveTo(116.0 ,309.0);
//    gepardContext.lineTo(87.0 ,332.0);
//    gepardContext.lineTo(99.0 ,348.0);
//    gepardContext.lineTo(129.0 ,326.0);
//    gepardContext.lineTo(116.0 ,309.0);
//    gepardContext.moveTo(67.0 ,300.0);
//    gepardContext.lineTo(55.0 ,311.0);
//    gepardContext.bezierCurveTo(116.0 ,379.0 ,115.0 ,398.0 ,54.0 ,463.0);
//    gepardContext.lineTo(68.0 ,474.0);
//    gepardContext.bezierCurveTo(137.0 ,404.0 ,137.0 ,370.0 ,67.0 ,300.0);
//    gepardContext.moveTo(57.0 ,301.0);
//    gepardContext.lineTo(69.0 ,312.0);
//    gepardContext.bezierCurveTo(8.0 ,380.0 ,9.0 ,399.0 ,70.0 ,464.0);
//    gepardContext.lineTo(56.0 ,475.0);
//    gepardContext.bezierCurveTo(-11.0 ,405.0 ,-11.0 ,370.0 ,57.0 ,301.0);

#endif

    gepardContext.closePath();

    gepardContext.fill();
    int a;
    std::cin >> a;

    XEvent xEvent;
    bool exit = true;
    while (!exit) {
        while (XPending(xSurface.display())) {
            XNextEvent(xSurface.display(), &xEvent);
            if (xEvent.type == KeyPress && xEvent.xkey.keycode == 27) {
                exit = true;
            }

        }
        if (!exit) {
            gepardContext.fill();
        }
    }

    return 0;
}
