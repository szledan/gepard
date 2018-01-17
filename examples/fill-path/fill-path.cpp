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

#include "gepard.h"
#include "surfaces/gepard-xsurface.h"
#include "surfaces/gepard-png-surface.h"
#include <chrono>
#include <iostream>
#include <thread>

void gShape(gepard::Gepard& ctx)
{
    ctx.fillStyle = "#0F0";
    ctx.beginPath();
    ctx.moveTo(300, 100);
    ctx.lineTo(50, 230);
    ctx.lineTo(380, 200);
    ctx.closePath();
    ctx.fill();

    ctx.fillStyle = "#F00";
    ctx.beginPath();
    ctx.moveTo(100, 100);
    ctx.lineTo(180, 200);
    ctx.bezierCurveTo(400, 200, 40, 50, 300, 250);
    ctx.closePath();
    ctx.fill();
}

int main(int argc, char* argv[])
{
    // Parse arguments.
    const int flags = argc < 2 ? 3 : std::atoi(argv[1]);

    // Draw to PNG file.
    if (flags & 1) {
        gepard::PNGSurface pngSurface(600, 600);
        gepard::Gepard pngGepard(&pngSurface);

        gShape(pngGepard);

        pngSurface.save("fillPath.png");
    }

    // Draw on XWindow.
    if (flags & 2) {
        gepard::XSurface surface(600, 600);
        gepard::Gepard gepard(&surface);

        gShape(gepard);

        XEvent xEvent;
        while (true) {
            std::this_thread::sleep_for(std::chrono::nanoseconds(1));   // Only for CPU sparing.
            if (XCheckWindowEvent((Display*)surface.getDisplay(), (Window)surface.getWindow(), KeyPress | ClientMessage, &xEvent)) {
                break;
            }
        }
    }

    return 0;
}
