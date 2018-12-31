/* Copyright (C) 2018, Gepard Graphics
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
#include <chrono>
#include <cmath>
#include <ctime>
#include <iostream>
#include <thread>

namespace {

class GepardClock {
public:
    GepardClock(const uint lineWidth = 3, const uint size = 200, const uint frequency = 60)
        : _lineWidth(lineWidth)
        , _size(size / 2)
        , _frequency(frequency)
        , _cx(_size)
        , _cy(_size)
        , _surface(new gepard::XSurface(size, size, "Gepard clock"))
        , _ctx(new gepard::Gepard(_surface))

    {
    }
    ~GepardClock()
    {
        if (_surface) {
            delete _surface;
        }
        if (_ctx) {
            delete _ctx;
        }
    }
    void run()
    {
        clockFace();
        while (!_surface->hasToQuit()) {
            std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
            struct std::tm* np = std::localtime(&now);

            showNow(np->tm_hour, np->tm_min, np->tm_sec);
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / _frequency));
            clear();
        }
    }

private:
    void clockFace()
    {
        _ctx->lineWidth = _lineWidth;
        _ctx->lineCap = "butt";
        _ctx->lineJoin = "miter";
        _ctx->miterLimit = 5;

        _ctx->strokeStyle = "#ff0";
        _ctx->beginPath();
        for (int i = 0; i < 12; ++i) {
            const float h = float(i) / 12.0;
            _ctx->moveTo(_cx + 0.91 * _size * std::sin(h * 2 * M_PI), _cy - 0.91 * _size * std::cos(h * 2 * M_PI));
            _ctx->lineTo(_cx + 1.0 * _size * std::sin(h * 2 * M_PI), _cx - 1.0 * _size * std::cos(h * 2 * M_PI));
        }
        _ctx->stroke();

        _ctx->strokeStyle = "#f60";
        _ctx->beginPath();
        _ctx->moveTo(_cx + _size, _cy);
        _ctx->arc(_cx, _cy, _size, 0, 2 * M_PI);
        _ctx->stroke();
        _ctx->lineCap = "round";
    }
    void clockHand(const float size, const float value, const std::string& color)
    {
        _ctx->strokeStyle = color;
        _ctx->beginPath();
        _ctx->moveTo(_cx, _cy);
        _ctx->lineTo(_cx + size * std::sin(value * 2 * M_PI), _cy - size * std::cos(value * 2 * M_PI));
        _ctx->stroke();
    }
    void showNow(const float h, const float m, const float s, const bool isClear = false)
    {
        if (!isClear) {
            _s = s / 60.0;
            _m = (m + _s) / 60.0;
            _h = (h + _m) / 12.0;
        }
        _ctx->lineWidth = _lineWidth + (isClear ? 2 : 0);
        clockHand(0.5 * _size, _h, isClear ? "#000" : "#f00");
        clockHand(0.66 * _size, _m, isClear ? "#000" : "#0f0");
        clockHand(0.86 * _size, _s, isClear ? "#000" : "#00f");
    }
    void clear() { showNow(0, 0, 0, true); }

    const float _lineWidth;
    const float _size;
    const uint _frequency;
    const float _cx;
    const float _cy;
    float _h, _m, _s;
    gepard::XSurface* _surface = nullptr;
    gepard::Gepard* _ctx = nullptr;
};

} // anonymous namespace

#define CHECK_FLAG(SFLAG, LFLAG, FROM, TO, ARGV) [&]()->int { for (int _i = FROM; _i < TO; ++_i) { if (std::string(ARGV[_i]) == SFLAG || std::string(ARGV[_i]) == LFLAG) return _i; } return 0; }()
#define HAS_FLAG(SFLAG, LFLAG, ARGC, ARGV) CHECK_FLAG(SFLAG, LFLAG, 1, ARGC, ARGV)
#define PARSE_FLAG_VALUE(SFLAG, LFLAG, DEFAULT, FROM, TO, ARGC, ARGV) [&](){ int _j = CHECK_FLAG(SFLAG, LFLAG, FROM, TO, ARGV); return (_j && (++_j) < ARGC) ? std::atoi(ARGV[_j]) : DEFAULT; }()
#define PARSE_FLAG(SFLAG, LFLAG, DEFAULT, ARGC, ARGV) PARSE_FLAG_VALUE(SFLAG, LFLAG, DEFAULT, 1, ARGC, ARGC, ARGV);

int main(int argc, char* argv[])
{
    uint frequency = 60;
    uint size = 200;
    uint lineWidth = 3;

    if (HAS_FLAG("-h", "--help", argc, argv)) {
        std::cout << "Usage: " << argv[0] << " [options]" << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        std::cout << "  -h, --help              show this help." << std::endl << std::endl;
        std::cout << "  -f, --frequency FREQ    set rendering frequency. Default: " << frequency << std::endl;
        std::cout << "  -s, --size SIZE         set size of clock. Default: " << size << std::endl;
        std::cout << "  -w, --line-width WIDTH  set width of line. Default: " << lineWidth << std::endl;
        return 0;
    }
    frequency = PARSE_FLAG("-f", "--frequency", frequency, argc, argv);
    size = PARSE_FLAG("-s", "--size", size, argc, argv);
    lineWidth = PARSE_FLAG("-w", "--line-width", lineWidth, argc, argv);

    GepardClock clck(lineWidth, size, frequency);
    clck.run();

    return 0;
}
