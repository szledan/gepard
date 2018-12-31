/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2018, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
 * Copyright (C) 2018, Szilard Ledan <szledan@gmail.com>
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
#include <thread>
#include <chrono>

void generateCheckerBoard(gepard::Gepard& ctx, const int size)
{
    gepard::Image chessDarkTile = gepard::utils::loadPng("apps/examples/image/Chess_d44.png");
    gepard::Image chessLightTile = gepard::utils::loadPng("apps/examples/image/Chess_l44.png");

    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            gepard::Image tileImage;
            if ((row + col) % 2) {
                tileImage = chessDarkTile;
            } else {
                tileImage = chessLightTile;
            }
            ctx.drawImage(tileImage, 0, 0, tileImage.width(), tileImage.height(), row * size, col * size, size, size);
        }
    }
}

int main()
{
    const uint32_t size = 50;
    const uint32_t width = 8 * size;
    const uint32_t height = 8 * size;
    gepard::XSurface surface(width, height);
    gepard::Gepard ctx(&surface);

    // Load images and draw chess pieces from file. (loadPNG and putImageData)
    gepard::Image chessPieces = gepard::utils::loadPng("apps/examples/image/ChessPiecesArray.png");
    ctx.putImageData(chessPieces, 0, 0);

    // Crop the 'white knight' figure. (getImageData)
    gepard::Image knightImage = ctx.getImageData(3 * chessPieces.width() / 6, 1 * chessPieces.height() / 2,
                                                 chessPieces.width() / 6, chessPieces.height() / 2);

    // Generate a 8x8 chess board (32-32 drawImage calls).
    // This call is overdrawing the canvas.
    generateCheckerBoard(ctx, size);

    // Blit size x size 'knight' into the first chess tile.
    ctx.drawImage(knightImage, 0, 0, size, size);
    // Put size x size 'knight' into the sixth chess tile. (Redraw background!)
    ctx.putImageData(knightImage, 5 * size, 0, 0, 0, size, size);

    // Blit 2size x size 'knight' into the second and third chess tiles.
    ctx.drawImage(knightImage, 1 * size, 0, 2 * size, size);
    // Put 2size x size 'knight' into the seventh and eighth chess tiles. (Redraw background!)
    ctx.putImageData(knightImage, 6 * size, 0, 0, 0, 2 * size, size);

    // Flip 'knight'.
    ctx.save();
    ctx.translate(2 * 0 * size + size, 0);
    ctx.scale(-1, 1);
    ctx.drawImage(knightImage, 0 * size, 1 * size, size, size);
    ctx.putImageData(knightImage, 5 * size, 1 * size, 0, 0, size, size);
    ctx.restore();

    // Flip and scale 'knight'.
    ctx.save();
    ctx.translate(-1.0 * size, 2 * 1 * size + size);
    ctx.scale(2, -1);
    ctx.drawImage(knightImage, 1 * size, 1 * size, size, size);
    ctx.putImageData(knightImage, 6 * size, 1 * size, 0, 0, size, size);
    ctx.restore();

    // Some complex transformations.
    const double pi = 3.1415926535897932;
    ctx.save();
    ctx.translate(1 * size, 2 * size);
    ctx.rotate(pi / 2.0);
    ctx.drawImage(knightImage, 0, 0, size, size);
    ctx.restore();
    ctx.save();
    ctx.translate(0 * size, 4 * size);
    ctx.rotate(-pi / 2.0);
    ctx.drawImage(knightImage, 0, 0, size, size);
    ctx.restore();
    ctx.save();
    ctx.translate(1 * size, 2 * size);
    ctx.rotate(pi / 2.0);
    ctx.scale(2, -1);
    ctx.drawImage(knightImage, 0, 0, size, size);
    ctx.restore();
    ctx.save();
    ctx.translate(3 * size, 4 * size);
    ctx.rotate(-pi / 2.0);
    ctx.scale(2, -1);
    ctx.drawImage(knightImage, 0, 0, size, size);
    ctx.restore();

    // More rotations.
    for (uint32_t i = 0; i < size * 7; ++i) {
        ctx.save();
        ctx.translate(i, 7 * size);
        ctx.rotate(float(i) / float(size) * pi);
        ctx.drawImage(knightImage, 0, 0, size, size);
        ctx.restore();
    }

    // Blitting.
    gepard::Image fullCanvas = ctx.getImageData(0, 0, surface.width(), surface.height());
    ctx.drawImage(fullCanvas, 4 * size, 3 * size, 2 * size, 2 * size);

    // Blitting a part from source.
    ctx.drawImage(fullCanvas, 4 * size, 4 * size, 2 * size, 3 * size, 6.5 * size, 3.5 * size, 1 * size, 1.5 * size);

    // Save result image.
    gepard::utils::savePng(ctx.getImageData(0, 0, surface.width(), surface.height()), "build/image.png");

    while (!surface.hasToQuit()) {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));   // Only for CPU sparing.
    }

    return 0;
}
