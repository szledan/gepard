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
#include <cmath>
#include <ctime>
#include <iostream>
#include <thread>

void pathShape(gepard::Gepard& ctx)
{
    ctx.lineWidth = 1;
    ctx.lineCap = "round";
    ctx.lineJoin = "miter";
    ctx.miterLimit = 5;

    ctx.fillStyle = "#000";
    ctx.fillRect(0, 0, 600, 600);

    ctx.fillStyle = "#0f0";
    ctx.strokeStyle = "#00f";
    ctx.beginPath();
    ctx.moveTo(300, 100);
    ctx.lineTo(50, 230);
    ctx.lineTo(380, 200);
    ctx.closePath();
    ctx.fill();
    ctx.stroke();

    ctx.fillStyle = "#f00";
    ctx.strokeStyle = "#ff0";
    ctx.beginPath();
    ctx.moveTo(100, 100);
    ctx.lineTo(180, 200);
    ctx.bezierCurveTo(400, 200, 40, 50, 300, 250);
    ctx.closePath();
    ctx.fill();
    ctx.stroke();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ctx.beginPath();
    ctx.moveTo(100, 300);
    ctx.lineTo(200, 300);
    ctx.fill();
    ctx.stroke();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ctx.beginPath();
    ctx.moveTo(100, 320);
    ctx.lineTo(200, 320);
    ctx.fill();
    ctx.stroke();
    std::this_thread::sleep_for(std::chrono::seconds(1));

    ctx.beginPath();
    ctx.moveTo(100, 400);
    ctx.lineTo(200, 400);
    ctx.fill();
    ctx.stroke();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    ctx.beginPath();
    ctx.moveTo(150, 350);
    ctx.lineTo(150, 450);
    ctx.fill();
    ctx.stroke();
}

void testShape(gepard::Gepard& ctx)
{
    ctx.lineWidth = 10;
    ctx.fillStyle = "#000";
    ctx.fillRect(0, 0, 300, 150);

    ctx.strokeStyle = "#00f";
    ctx.beginPath();
    ctx.moveTo(100, 100);
    ctx.lineTo(199, 111);
    ctx.stroke();

    ctx.strokeStyle = "#000";
    ctx.beginPath();
    ctx.moveTo(100, 100);
    ctx.lineTo(199, 111);
    ctx.stroke();
}

int main(int argc, char* argv[])
{
    if ((argc > 1) && (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help")) {
        std::cout << "Usage: " << argv[0] << " [options]" << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help             show this help." << std::endl;
        std::cout << "  -p, --png [file-name]  use png-surface and set output filename." << std::endl;
        return 0;
    }

    if (argc > 1 && (std::string(argv[1]) == "-p" || std::string(argv[1]) == "--png")) {
        std::string pngFile = (argc > 2) ? argv[2] : "fillPath.png";

        gepard::PNGSurface pngSurface(600, 600);
        gepard::Gepard pngGepard(&pngSurface);

        pathShape(pngGepard);
        testShape(pngGepard);

        pngSurface.save(pngFile);

    } else {
        struct {
            void clockFace()
            {
                _ctx.lineWidth = _lineWidth;
                _ctx.lineCap = "butt";
                _ctx.lineJoin = "miter";
                _ctx.miterLimit = 5;

                _ctx.strokeStyle = "#ff0";
                _ctx.beginPath();
                for (int i = 0; i < 12; ++i) {
                    const float h = float(i) / 12.0;
                    _ctx.moveTo(_cx + 0.91 * _size * std::sin(h * 2 * M_PI), _cy - 0.91 * _size * std::cos(h * 2 * M_PI));
                    _ctx.lineTo(_cx + 1.0 * _size * std::sin(h * 2 * M_PI), _cx - 1.0 * _size * std::cos(h * 2 * M_PI));
                }
                _ctx.stroke();

                _ctx.strokeStyle = "#f60";
                _ctx.beginPath();
                _ctx.moveTo(_cx + _size, _cy);
                _ctx.arc(_cx, _cy, _size, 0, 2 * M_PI);
                _ctx.stroke();
                _ctx.lineCap = "round";
            }
            void clockHand(const float size, const float value, const std::string& color)
            {
                _ctx.strokeStyle = color;
                _ctx.beginPath();
                _ctx.moveTo(_cx, _cy);
                _ctx.lineTo(_cx + size * std::sin(value * 2 * M_PI), _cy - size * std::cos(value * 2 * M_PI));
                _ctx.stroke();
            }
            void showNow(const float h, const float m, const float s, const bool isClear = false)
            {
                if (!isClear) {
                    _s = s / 60.0;
                    _m = (m + _s) / 60.0;
                    _h = (h + _m) / 12.0;
                }
                _ctx.lineWidth = _lineWidth + (isClear ? 5 : 0);
                 clockHand(0.5 * _size, _h, isClear ? "#000" : "#f00");
                 clockHand(0.66 * _size, _m, isClear ? "#000" : "#0f0");
                 clockHand(0.86 * _size, _s, isClear ? "#000" : "#00f");
            }
            void clear() { showNow(0, 0, 0, true); }
            void run()
            {
                clockFace();
                XEvent xEvent;
                while (true) {
                    std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                    struct std::tm* np = std::localtime(&now);
                    showNow(np->tm_hour, np->tm_min, np->tm_sec);
                    std::this_thread::sleep_for(std::chrono::milliseconds(900));
                    clear();

                    if (XCheckWindowEvent((Display*)_surface.getDisplay(), (Window)_surface.getWindow(), KeyPress | ClientMessage, &xEvent)) {
                        break;
                    }
                }
            }

            float _lineWidth = 3.0;
            float _cx = 300.0;
            float _cy = 300.0;
            float _size = 100.0;
            float _h, _m, _s;
            gepard::XSurface _surface = gepard::XSurface(600, 600);
            gepard::Gepard _ctx = gepard::Gepard(&_surface);
        } clck;

        clck.run();
    }

    return 0;
}
