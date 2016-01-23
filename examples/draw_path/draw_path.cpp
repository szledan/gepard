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

void lineCap(gepard::Gepard& gepardContext, const float ex, const float ey, const float sx, const float sy, const bool withArcTo = true)
{
    if (withArcTo) {
        const float c1x = ex + (sy - ey) / 2.0;
        const float c1y = ey - (sx - ex) / 2.0;
        const float c2x = sx - (ey - sy) / 2.0;
        const float c2y = sy + (ex - sx) / 2.0;
        float h1x = (c2x - c1x) / 2.0;
        float h1y = (c2y - c1y) / 2.0;
        const float radius = sqrt(h1x * h1x + h1y * h1y);
        h1x += c1x;
        h1y += c1y;
        gepardContext.arcTo(c1x, c1y, h1x, h1y, radius);
        gepardContext.arcTo(c2x, c2y, sx, sy, radius);
    } else {
        gepardContext.lineTo(sx, sy);
    }
}

int main()
{
    gepard::XGepardSurface xSurface(512, 512);
    gepard::Gepard gepardContext(&xSurface);

    gepardContext.beginPath();

#if 0
    // One pixel high triangle.
    gepardContext.moveTo(10.0, 90.0);
    gepardContext.lineTo(10.0 + 256.0, 90.0);
    gepardContext.lineTo(10.0, 91.0);

    // One pixel high line.
    gepardContext.rect(10.0, 100.0, 256.0, 1.0);

    // One pixel wide triangle.
    gepardContext.moveTo(10.0, 110.0);
    gepardContext.lineTo(11.0, 110.0);
    gepardContext.lineTo(10.0, 110.0 + 256.0);

    // One pixel wide line.
    gepardContext.rect(20.0, 110.0, 1.0, 256.0);

    // Full pixel size rect on one pixel.
    gepardContext.rect(30.0, 110.0, 1.0, 1.0);

    // Half pixel size rect on one pixel.
    gepardContext.rect(40.5, 110.0, 0.5, 1.0);

    // Full pixel size rect on two pixels.
    gepardContext.rect(50.5, 110.0, 1.0, 1.0);

    // Full pixel size rect on four pixels.
    gepardContext.rect(60.5, 110.5, 1.0, 1.0);

    // Half pixel size triangle on one pixel.
    gepardContext.moveTo(70.0, 110.0);
    gepardContext.lineTo(71.0, 111.0);
    gepardContext.lineTo(71.0, 110.0);

    // Tests for fitting shapes and drawing directions.
    {
        float shiftBottomX = 10.0;
        // Two quads slided onto each other with same drawing directions.
        gepardContext.moveTo(30.0, 120.0); /* ccw */
        gepardContext.lineTo(30.0 + shiftBottomX, 150.0);
        gepardContext.lineTo(90.0 + shiftBottomX, 150.0);
        gepardContext.lineTo(90.0, 120.0);
        gepardContext.moveTo(60.0, 130.0); /* ccw */
        gepardContext.lineTo(60.0 + shiftBottomX, 160.0);
        gepardContext.lineTo(120.0 + shiftBottomX, 160.0);
        gepardContext.lineTo(120.0, 130.0);

        // Two quads slided onto each other with opposite drawing directions.
        gepardContext.moveTo(130.0, 120.0); /* ccw */
        gepardContext.lineTo(130.0 + shiftBottomX, 150.0);
        gepardContext.lineTo(190.0 + shiftBottomX, 150.0);
        gepardContext.lineTo(190.0, 120.0);
        gepardContext.moveTo(160.0, 130.0); /* cw */
        gepardContext.lineTo(220.0, 130.0);
        gepardContext.lineTo(220.0 + shiftBottomX, 160.0);
        gepardContext.lineTo(160.0 + shiftBottomX, 160.0);

        // Three quads fitted together.0, they may be combined to one trapezoid.
        gepardContext.moveTo(30.0, 170.0); /* ccw */
        gepardContext.lineTo(30.0 + shiftBottomX, 210.0);
        gepardContext.lineTo(75.0 + shiftBottomX, 210.0);
        gepardContext.lineTo(75.0, 170.0);
        gepardContext.moveTo(75.0, 170.0); /* ccw */
        gepardContext.lineTo(75.0 + shiftBottomX / 2.0, 190.0);
        gepardContext.lineTo(120.0 + shiftBottomX / 2.0, 190.0);
        gepardContext.lineTo(120.0, 170.0);
        gepardContext.moveTo(75.0 + shiftBottomX / 2.0, 190.0); /* cw */
        gepardContext.lineTo(120.0 + shiftBottomX / 2.0, 190.0);
        gepardContext.lineTo(120.0 + shiftBottomX, 210.0);
        gepardContext.lineTo(75.0 + shiftBottomX, 210.0);

        // Three quads where the last two are slided to the first one.
        // The last two quads may be combined to one trapezoid.
        gepardContext.moveTo(130.0, 170.0); /* ccw */
        gepardContext.lineTo(130.0 + shiftBottomX, 210.0);
        gepardContext.lineTo(190.0 + shiftBottomX, 210.0);
        gepardContext.lineTo(190.0, 170.0);
        gepardContext.moveTo(170.0, 170.0); /* ccw */
        gepardContext.lineTo(170.0 + shiftBottomX / 2.0, 190.0);
        gepardContext.lineTo(220.0 + shiftBottomX / 2.0, 190.0);
        gepardContext.lineTo(220.0, 170.0);
        gepardContext.moveTo(170.0 + shiftBottomX / 2.0, 190.0); /* cw */
        gepardContext.lineTo(220.0 + shiftBottomX / 2.0, 190.0);
        gepardContext.lineTo(220.0 + shiftBottomX, 210.0);
        gepardContext.lineTo(170.0 + shiftBottomX, 210.0);

        // Two quads fitted on top of each other with same drawing directions.
        // They may be combined to one trapezoid.
        gepardContext.moveTo(30.0, 220.0); /* ccw */
        gepardContext.lineTo(30.0 + shiftBottomX / 2.0, 240.0);
        gepardContext.lineTo(120.0 + shiftBottomX / 2.0, 240.0);
        gepardContext.lineTo(120.0, 220.0);
        gepardContext.moveTo(30.0 + shiftBottomX / 2.0, 240.0); /* cw */
        gepardContext.lineTo(120.0 + shiftBottomX / 2.0, 240.0);
        gepardContext.lineTo(120.0 + shiftBottomX, 260.0);
        gepardContext.lineTo(30.0 + shiftBottomX, 260.0);

        // Two quads fitted on top of each other with opposite drawing directions.
        // They may be combined to one trapezoid.
        gepardContext.moveTo(130.0, 220.0); /* ccw */
        gepardContext.lineTo(130.0 + shiftBottomX / 2.0, 240.0);
        gepardContext.lineTo(220.0 + shiftBottomX / 2.0, 240.0);
        gepardContext.lineTo(220.0, 220.0);
        gepardContext.moveTo(130.0 + shiftBottomX / 2.0, 240.0); /* ccw */
        gepardContext.lineTo(130.0 + shiftBottomX, 260.0);
        gepardContext.lineTo(220.0 + shiftBottomX, 260.0);
        gepardContext.lineTo(220.0 + shiftBottomX / 2.0, 240.0);

        // One quad inside the other one with same drawing directions.
        gepardContext.moveTo(30.0, 270.0); /* ccw */
        gepardContext.lineTo(30.0 + shiftBottomX, 310.0);
        gepardContext.lineTo(120.0 + shiftBottomX, 310.0);
        gepardContext.lineTo(120.0, 270.0);
        gepardContext.moveTo(70.0, 280.0); /* ccw */
        gepardContext.lineTo(70.0 + shiftBottomX / 2.0, 300.0);
        gepardContext.lineTo(100.0 + shiftBottomX / 2.0, 300.0);
        gepardContext.lineTo(100.0, 280.0);

        // One quad inside the other one with opposite drawing directions.
        gepardContext.moveTo(130.0, 270.0); /* ccw */
        gepardContext.lineTo(130.0 + shiftBottomX, 310.0);
        gepardContext.lineTo(220.0 + shiftBottomX, 310.0);
        gepardContext.lineTo(220.0, 270.0);
        gepardContext.moveTo(170.0, 280.0); /* cw */
        gepardContext.lineTo(200.0, 280.0);
        gepardContext.lineTo(200.0 + shiftBottomX / 2.0, 300.0);
        gepardContext.lineTo(170.0 + shiftBottomX / 2.0, 300.0);

        bool anticlockwise = true;
        // Two full circles slided onto each other with same drawing directions.
        gepardContext.moveTo(65.0, 340.0);
        gepardContext.arc(65.0, 340.0, 20.0, 0.0, 4.0 * asin(1.0), !anticlockwise);
        gepardContext.moveTo(95.0, 340.0);
        gepardContext.arc(95.0, 340.0, 20.0, 0.0, 4.0 * asin(1.0), !anticlockwise);

        // Two full circles slided onto each other with opposite drawing directions.
        gepardContext.moveTo(165.0, 340.0);
        gepardContext.arc(165.0, 340.0, 20.0, 0.0, 4.0 * asin(1.0), !anticlockwise);
        gepardContext.moveTo(195.0, 340.0);
        gepardContext.arc(195.0, 340.0, 20.0, 0.0, 4.0 * asin(1.0), anticlockwise);
    }
#elif 1
    bool lineCapWithArcTo = true;
    // The shape of "Erdély" with old Hungarian alphabet.
    // "E" character.
    gepardContext.moveTo(358.0, 278.0);
    gepardContext.bezierCurveTo(419.0, 211.0, 417.0, 192.0, 358.0, 127.0);
    lineCap(gepardContext, 358.0, 127.0, 372.0, 116.0, lineCapWithArcTo);
    gepardContext.bezierCurveTo(440.0, 186.0, 440.0, 220.0, 371.0, 289.0);
    lineCap(gepardContext, 371.0, 289.0, 358.0, 278.0, lineCapWithArcTo);
    gepardContext.moveTo(380.0, 175.0);
    lineCap(gepardContext, 380.0, 175.0, 366.0, 160.0, lineCapWithArcTo);
    gepardContext.lineTo(406.0, 123.0);
    lineCap(gepardContext, 406.0, 123.0, 420.0, 138.0, lineCapWithArcTo);
    gepardContext.moveTo(364.0, 248.0);
    lineCap(gepardContext, 364.0, 248.0, 376.0, 233.0, lineCapWithArcTo);
    gepardContext.lineTo(418.0, 266.0);
    lineCap(gepardContext, 418.0, 266.0, 406.0, 281.0, lineCapWithArcTo);

    // "RD" characters.
    gepardContext.moveTo(330.0, 282.0);
    lineCap(gepardContext, 330.0, 282.0, 310.0, 282.0, lineCapWithArcTo);
    gepardContext.lineTo(311.0, 121.0);
    lineCap(gepardContext, 311.0, 121.0, 331.0, 121.0, lineCapWithArcTo);
    gepardContext.moveTo(254.0, 282.0);
    lineCap(gepardContext, 254.0, 282.0, 234.0, 282.0, lineCapWithArcTo);
    gepardContext.lineTo(236.0, 121.0);
    lineCap(gepardContext, 236.0, 121.0, 256.0, 121.0, lineCapWithArcTo);
    gepardContext.moveTo(211.0, 245.0);
    lineCap(gepardContext, 211.0, 245.0, 199.0, 228.0, lineCapWithArcTo);
    gepardContext.lineTo(316.0, 150.0);
    lineCap(gepardContext, 316.0, 150.0, 327.0, 167.0, lineCapWithArcTo);

    // "ÉLY" characters.
    gepardContext.moveTo(112.0, 247.0);
    lineCap(gepardContext, 112.0, 247.0, 96.0, 235.0, lineCapWithArcTo);
    gepardContext.lineTo(126.0, 194.0);
    lineCap(gepardContext, 126.0, 194.0, 142.0, 206.0, lineCapWithArcTo);
    gepardContext.moveTo(174.0, 158.0);
    lineCap(gepardContext, 174.0, 158.0, 160.0, 143.0, lineCapWithArcTo);
    gepardContext.lineTo(185.0, 119.0);
    lineCap(gepardContext, 185.0, 119.0, 199.0, 134.0, lineCapWithArcTo);
    gepardContext.moveTo(158.0, 257.0);
    lineCap(gepardContext, 158.0, 257.0, 170.0, 241.0, lineCapWithArcTo);
    gepardContext.lineTo(200.0, 263.0);
    lineCap(gepardContext, 200.0, 263.0, 187.0, 279.0, lineCapWithArcTo);
    gepardContext.moveTo(126.0, 277.0);
    gepardContext.bezierCurveTo(187.0, 210.0, 185.0, 191.0, 126.0, 126.0);
    lineCap(gepardContext, 126.0, 126.0, 140.0, 115.0, lineCapWithArcTo);
    gepardContext.bezierCurveTo(208.0, 185.0, 208.0, 219.0, 139.0, 288.0);
    lineCap(gepardContext, 139.0, 288.0, 126.0, 277.0, lineCapWithArcTo);
    gepardContext.moveTo(128.0, 288.0);
    gepardContext.bezierCurveTo(59.0, 219.0, 59.0, 185.0, 127.0, 115.0);
    lineCap(gepardContext, 127.0, 115.0, 141.0, 126.0, lineCapWithArcTo);
    gepardContext.bezierCurveTo(80.0, 191.0, 79.0, 210.0, 140.0, 278.0);
    lineCap(gepardContext, 140.0, 278.0, 128.0, 288.0, lineCapWithArcTo);
#endif

    gepardContext.closePath();
    gepardContext.fill("nonzero");

    int a;
    std::cin >> a;

    return 0;
}
