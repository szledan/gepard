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
#include "surfaces/gepard-xsurface.h"
#include <iostream>
#include <thread>
#include <chrono>

void pathShape(gepard::Gepard& ctx)
{
    ctx.lineWidth = 20;

    ctx.fillStyle = "#0f0";
    ctx.strokeStyle = "#0ff";
    ctx.beginPath();
    ctx.moveTo(300, 100);
    ctx.lineTo(50, 230);
    ctx.lineTo(380, 200);
    ctx.closePath();
    ctx.fill();

    ctx.save();
    ctx.rotate(3.1415926 * 0.1);
    ctx.stroke();
    ctx.restore();

    ctx.fillStyle = "#f00";
    ctx.strokeStyle = "#ff0";
    ctx.beginPath();
    ctx.moveTo(100, 100);
    ctx.lineTo(180, 200);
    ctx.bezierCurveTo(400, 200, 40, 50, 300, 250);
//    ctx.arcTo(400, 200, 40, 50, 300, 250);
    ctx.fill();
    ctx.stroke();
}

int main(int argc, char* argv[])
{
    // Parse arguments.
    std::string pngFile = (argc > 1) ? argv[1] : "build/fill-path.png";
    if ((argc > 1) && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        std::cout << "Usage: " << argv[0] << " [options] [file-name=" << pngFile << "]" << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help    show this help." << std::endl;
        return 0;
    } else if (argc > 1) {
        pngFile = argv[1];
    }

    // Create an X surface and Gepard context.
    gepard::XSurface surface(600, 600);
    gepard::Gepard ctx(&surface);

    // Clear 'black' the canvas.
    ctx.fillStyle = "#000";
    ctx.fillRect(0, 0, 600, 600);

    // Set stroke line width.
    ctx.lineWidth = 5;

    // Transformed 'mandala'.
    ctx.save();
    ctx.translate(300, 300);
    ctx.beginPath();
    for (int i = 0; i < 21; ++i) {
        ctx.save();
        ctx.scale(2.1, 1.0);
        ctx.rotate(double(i * 2.0 * 3.1415926) / 21.0);
        ctx.moveTo(0, 0);
        ctx.bezierCurveTo(0, 100, 20, 100, 20, 80);
        ctx.quadraticCurveTo(20, 60, 10, 40);
        ctx.arc(40, 60, 40, 3.8, 4.0);
        ctx.restore();
    }
    ctx.fillStyle = "#00f";
    ctx.fill();
    ctx.strokeStyle = "#0aa";
    ctx.stroke();
    ctx.restore();

    // Buttons.
    ctx.fillStyle = "#ddd";
    ctx.lineJoin = "round";
    // 'Accept' button.
    ctx.beginPath();
    ctx.moveTo(320, 500);
    ctx.arcTo(390, 500, 390, 510, 10);
    ctx.arcTo(390, 530, 380, 530, 10);
    ctx.arcTo(310, 530, 310, 520, 10);
    ctx.arcTo(310, 500, 320, 500, 10);
    ctx.closePath();
    ctx.fill();
    ctx.moveTo(340, 515);
    ctx.lineTo(345, 525);
    ctx.lineTo(360, 505);
    ctx.strokeStyle = "#0a0";
    ctx.stroke();
    // 'Cancel' button.
    ctx.beginPath();
    ctx.rect(410, 500, 80, 30);
    ctx.moveTo(440, 505);
    ctx.lineTo(460, 525);
    ctx.moveTo(460, 505);
    ctx.lineTo(440, 525);
    ctx.fill();
    ctx.strokeStyle = "#f00";
    ctx.stroke();

    // Save result in to PNG.
    gepard::Image img = ctx.getImageData(0, 0, surface.width(), surface.height());
    gepard::utils::savePng(img, pngFile);

    // Waiting for quit.
    while (true) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));   // Only for CPU sparing.
        if (surface.hasToQuit()) {
            break;
        }
    }

    return 0;
}
