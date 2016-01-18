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
    // One pixel heigh triangle.
    gepardContext.moveTo(10, 90);
    gepardContext.lineTo(10 + 256, 90);
    gepardContext.lineTo(10, 91);

    // One pixel heigh line.
    gepardContext.moveTo(10, 100);
    gepardContext.lineTo(10 + 256, 100);
    gepardContext.lineTo(10 + 256, 101);
    gepardContext.lineTo(10, 101);

    // One pixel wide triangle.
    gepardContext.moveTo(10, 110);
    gepardContext.lineTo(11, 110);
    gepardContext.lineTo(10, 110 + 256);

    // One pixel wide line.
    gepardContext.moveTo(20, 110);
    gepardContext.lineTo(20, 110 + 256);
    gepardContext.lineTo(21, 110 + 256);
    gepardContext.lineTo(21, 110);

    // Full pixel size rect on one pixel.
    gepardContext.moveTo(30, 110);
    gepardContext.lineTo(30, 111);
    gepardContext.lineTo(31, 111);
    gepardContext.lineTo(31, 110);

    // Half pixel size rect on one pixel.
    gepardContext.moveTo(40.5, 110);
    gepardContext.lineTo(40.5, 111);
    gepardContext.lineTo(41, 111);
    gepardContext.lineTo(41, 110);

    // Full pixel size rect on two pixels.
    gepardContext.moveTo(50.5, 110);
    gepardContext.lineTo(50.5, 111);
    gepardContext.lineTo(51.5, 111);
    gepardContext.lineTo(51.5, 110);

    // Full pixel size rect on forth pixels.
    gepardContext.moveTo(60.5, 110.5);
    gepardContext.lineTo(60.5, 111.5);
    gepardContext.lineTo(61.5, 111.5);
    gepardContext.lineTo(61.5, 110.5);

    // Half pixel size triangle on one pixel.
    gepardContext.moveTo(70, 110);
    gepardContext.lineTo(71, 111);
    gepardContext.lineTo(71, 110);

    //
    gepardContext.moveTo(30, 120); /* ccw */
    gepardContext.lineTo(30, 150);
    gepardContext.lineTo(90, 150);
    gepardContext.lineTo(90, 120);
    gepardContext.moveTo(60, 130); /* ccw */
    gepardContext.lineTo(60, 160);
    gepardContext.lineTo(120, 160);
    gepardContext.lineTo(120, 130);

    //
    gepardContext.moveTo(130, 120); /* ccw */
    gepardContext.lineTo(130, 150);
    gepardContext.lineTo(190, 150);
    gepardContext.lineTo(190, 120);
    gepardContext.moveTo(160, 130); /* cw */
    gepardContext.lineTo(220, 130);
    gepardContext.lineTo(220, 160);
    gepardContext.lineTo(160, 160);

    //
    gepardContext.moveTo(30, 170); /* ccw */
    gepardContext.lineTo(30, 210);
    gepardContext.lineTo(75, 210);
    gepardContext.lineTo(75, 170);
    gepardContext.moveTo(75, 170); /* ccw */
    gepardContext.lineTo(75, 190);
    gepardContext.lineTo(120, 190);
    gepardContext.lineTo(120, 170);
    gepardContext.moveTo(75, 190); /* cw */
    gepardContext.lineTo(120, 190);
    gepardContext.lineTo(120, 210);
    gepardContext.lineTo(75, 210);

    //
    gepardContext.moveTo(130, 170); /* ccw */
    gepardContext.lineTo(130, 210);
    gepardContext.lineTo(190, 210);
    gepardContext.lineTo(190, 170);
    gepardContext.moveTo(170, 170); /* ccw */
    gepardContext.lineTo(170, 190);
    gepardContext.lineTo(220, 190);
    gepardContext.lineTo(220, 170);
    gepardContext.moveTo(170, 190); /* cw */
    gepardContext.lineTo(220, 190);
    gepardContext.lineTo(220, 210);
    gepardContext.lineTo(170, 210);

    //
    gepardContext.moveTo(30, 220); /* ccw */
    gepardContext.lineTo(30, 240);
    gepardContext.lineTo(120, 240);
    gepardContext.lineTo(120, 220);
    gepardContext.moveTo(30, 240); /* cw */
    gepardContext.lineTo(120, 240);
    gepardContext.lineTo(120, 260);
    gepardContext.lineTo(30, 260);

    //
    gepardContext.moveTo(130, 220); /* ccw */
    gepardContext.lineTo(130, 240);
    gepardContext.lineTo(220, 240);
    gepardContext.lineTo(220, 220);
    gepardContext.moveTo(130, 240); /* ccw */
    gepardContext.lineTo(130, 260);
    gepardContext.lineTo(220, 260);
    gepardContext.lineTo(220, 240);

    //
    gepardContext.moveTo(30, 270); /* ccw */
    gepardContext.lineTo(30, 310);
    gepardContext.lineTo(120, 310);
    gepardContext.lineTo(120, 270);
    gepardContext.moveTo(70, 280); /* ccw */
    gepardContext.lineTo(70, 300);
    gepardContext.lineTo(100, 300);
    gepardContext.lineTo(100, 280);

    //
    gepardContext.moveTo(130, 270); /* ccw */
    gepardContext.lineTo(130, 310);
    gepardContext.lineTo(220, 310);
    gepardContext.lineTo(220, 270);
    gepardContext.moveTo(170, 280); /* cw */
    gepardContext.lineTo(200, 280);
    gepardContext.lineTo(200, 300);
    gepardContext.lineTo(170, 300);
#elif 1
    // The shape of "Erdély" with old hungarian's alphabet.
    // "E"
    gepardContext.moveTo(358.0, 278.0);
    gepardContext.bezierCurveTo(419.0, 211.0, 417.0, 192.0, 358.0, 127.0);
    gepardContext.lineTo(372.0, 116.0);
    gepardContext.bezierCurveTo(440.0, 186.0, 440.0, 220.0, 371.0, 289.0);
    gepardContext.moveTo(380.0, 175.0);
    gepardContext.lineTo(366.0, 160.0);
    gepardContext.lineTo(406.0, 123.0);
    gepardContext.lineTo(420.0, 138.0);
    gepardContext.moveTo(406.0, 281.0);
    gepardContext.lineTo(364.0, 248.0);
    gepardContext.lineTo(376.0, 233.0);
    gepardContext.lineTo(418.0, 266.0);

    // "RD"
    gepardContext.moveTo(330.0, 282.0);
    gepardContext.lineTo(310.0, 282.0);
    gepardContext.lineTo(313.0, 121.0);
    gepardContext.lineTo(333.0, 121.0);
    gepardContext.moveTo(254.0, 282.0);
    gepardContext.lineTo(234.0, 282.0);
    gepardContext.lineTo(236.0, 121.0);
    gepardContext.lineTo(256.0, 121.0);
    gepardContext.moveTo(211.0, 245.0);
    gepardContext.lineTo(199.0, 228.0);
    gepardContext.lineTo(316.0, 150.0);
    gepardContext.lineTo(327.0, 167.0);

    // "ÉLY"
    gepardContext.moveTo(112.0, 247.0);
    gepardContext.lineTo(96.0, 235.0);
    gepardContext.lineTo(126.0, 194.0);
    gepardContext.lineTo(142.0, 206.0);
    gepardContext.moveTo(174.0, 158.0);
    gepardContext.lineTo(160.0, 143.0);
    gepardContext.lineTo(185.0, 119.0);
    gepardContext.lineTo(199.0, 134.0);
    gepardContext.moveTo(187.0, 279.0);
    gepardContext.lineTo(158.0, 257.0);
    gepardContext.lineTo(170.0, 241.0);
    gepardContext.lineTo(200.0, 263.0);
    gepardContext.moveTo(126.0, 277.0);
    gepardContext.bezierCurveTo(187.0, 210.0, 185.0, 191.0, 126.0, 126.0);
    gepardContext.lineTo(140.0, 115.0);
    gepardContext.bezierCurveTo(208.0, 185.0, 208.0, 219.0, 139.0, 288.0);
    gepardContext.moveTo(128.0, 287.0);
    gepardContext.bezierCurveTo(59.0, 218.0, 59.0, 184.0, 127.0, 114.0);
    gepardContext.lineTo(141.0, 125.0);
    gepardContext.bezierCurveTo(80.0, 190.0, 79.0, 209.0, 140.0, 277.0);
#elif 1
    // An ugly test.
    gepardContext.moveTo(256, 256);
    for (float x = 0; x < 512.0; x += 512.0 / 16.0) {
        std::cout << x << " ";
        for (float y = 250; y < 512.0; y += 512.0 / 16.0) {
            gepardContext.lineTo(256, 256);
            gepardContext.lineTo(x, y);
            gepardContext.lineTo(x, y + 5);
        }
    }
#else
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
#endif

    gepardContext.closePath();
    gepardContext.fill();

    int a;
    std::cin >> a;

    return 0;
}
