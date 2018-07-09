/* Copyright (C) 2017-2018, Gepard Graphics
 * Copyright (C) 2017-2018, Szilard Ledan <szledan@gmail.com>
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
#include "surfaces/gepard-png-surface.h"
#include "surfaces/gepard-xsurface.h"
#include <chrono>
#include <iostream>
#include <thread>

void rightTriangle(gepard::Gepard& ctx, const float x, const float y, const float w, const float h)
{
    ctx.beginPath();
    ctx.moveTo(x, y);
    ctx.lineTo(x + w, y);
    ctx.lineTo(x, y + h);
    ctx.closePath();
    ctx.fill();
}

void fillRightTriangle(gepard::Gepard& ctx, const std::string style, const float x, const float y, const float w, const float h)
{
    ctx.fillStyle = style;
    rightTriangle(ctx, x, y, w, h);
}

void pathShape(gepard::Gepard& ctx)
{
    // One pixel size triangles.
    fillRightTriangle(ctx, "#f00", 1, 1, 1, 1);
    fillRightTriangle(ctx, "#fff", 4, 3, -1, 1);
    fillRightTriangle(ctx, "#00f", 6, 6, -1, -1);
    fillRightTriangle(ctx, "#0f0", 7, 8, 1, -1);

    // One pixel high and 255 pixels wide triangles.
    fillRightTriangle(ctx, "#f00", 10, 1, 255, 1);
    fillRightTriangle(ctx, "#fff", 10 + 255, 3, -255, 1);
    fillRightTriangle(ctx, "#00f", 10 + 255, 6, -255, -1);
    fillRightTriangle(ctx, "#0f0", 10, 8, 255, -1);

    // One pixel wide and 255 pixels high triangles.
    fillRightTriangle(ctx, "#f00", 1, 10, 1, 255);
    fillRightTriangle(ctx, "#fff", 4, 10, -1, 255);
    fillRightTriangle(ctx, "#00f", 6, 10 + 255, -1, -255);
    fillRightTriangle(ctx, "#0f0", 7, 10 + 255, 1, -255);

    // Eight pixels size triangles.
    fillRightTriangle(ctx, "#f00", 10, 10, 8, 8);
    fillRightTriangle(ctx, "#fff", 30, 10, -8, 8);
    fillRightTriangle(ctx, "#00f", 30, 30, -8, -8);
    fillRightTriangle(ctx, "#0f0", 10, 30, 8, -8);

    // Contacted triangles.
    fillRightTriangle(ctx, "#888", 40, 10, 20, 20);
    fillRightTriangle(ctx, "#888", 60, 30, -20, -20);
    fillRightTriangle(ctx, "#888", 90, 10, -20, 20);
    fillRightTriangle(ctx, "#888", 70, 30, 20, -20);

    // Stacked triangles with different alpha values.
    for (int i = 0, x = 100; i < 5; ++i) {
        const float alpha[] = {0.1f, 1.0f / 3.0f, 0.5f, 2.0f / 3.0f, 1.0f};
        ctx.setFillColor(1.0f, 0.0f, 0.0f, alpha[i]);
        rightTriangle(ctx, x + i * 30, 10, 16, 16);
        ctx.setFillColor(0.0f, 1.0f, 0.0f, alpha[i]);
        rightTriangle(ctx, x + i * 30, 30, 16, -16);
        ctx.setFillColor(0.0f, 0.0f, 1.0f, alpha[i]);
        rightTriangle(ctx, x + 20 + i * 30, 30, -16, -16);
    }

    ctx.strokeStyle = "#0f0";
    ctx.beginPath();
    ctx.moveTo(100, 100);
    ctx.lineTo(130, 100);
    ctx.stroke();
}

int main(int argc, char* argv[])
{
    if ((argc > 1) && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        std::cout << "Usage: " << argv[0] << " [options] [file-name]" << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help    show this help." << std::endl;
        return 0;
    }

    std::string pngFile = (argc > 1) ? argv[1] : "fill-path.png";

    const uint width = 300;
    const uint height = 300;
    gepard::PNGSurface pngSurface(width, height);
    gepard::XSurface surface(600, 600);
    gepard::Gepard pngGepard(&surface);

    pngGepard.fillStyle = "#000";
    pngGepard.fillRect(0, 0, width, height);

    pathShape(pngGepard);
    XEvent xEvent;

    while (true) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));   // Only for CPU sparing.
        if (XCheckWindowEvent((Display*)surface.getDisplay(), (Window)surface.getWindow(), KeyPress | ClientMessage, &xEvent)) {
            break;
        }
    }

//    pngSurface.save(pngFile);

    return 0;
}
