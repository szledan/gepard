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
#include <iostream>

void pathShape(gepard::Gepard& ctx)
{

    ctx.beginPath();
    ctx.fillStyle = "#000";  // Green path
    ctx.rect(0, 0, 300, 150);
    ctx.fill();

    ctx.beginPath();
    ctx.lineWidth = "5";
    ctx.strokeStyle = "#abc";  // Green path
    ctx.moveTo(0, 75);
    ctx.lineTo(250, 75);
    ctx.stroke();  // Draw it

    ctx.beginPath();
    ctx.lineWidth = "5";
    ctx.strokeStyle = "#abc";  // Green path
    ctx.moveTo(75, 0);
    ctx.lineTo(75, 250);
    ctx.stroke();  // Draw it

    ctx.beginPath();
    ctx.strokeStyle = "#0120ff";  // Purple path
    ctx.moveTo(50, 0);
    ctx.lineTo(150, 130);
    ctx.stroke();  // Draw it

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

    gepard::PNGSurface pngSurface(600, 600);
    gepard::Gepard pngGepard(&pngSurface);

    pathShape(pngGepard);

    pngSurface.save(pngFile);

    return 0;
}
