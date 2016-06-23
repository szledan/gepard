/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2016, Szilard Ledan <szledan@gmail.com>
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
#include "gepard-xsurface.h"
#include <fstream>
#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <sstream>
#include <time.h>

typedef std::map<std::string, int> ConfigMap;
ConfigMap s_configs;

inline void step()
{
    static int warmupCount = -s_configs["warmupCount"];

    static time_t s_currentTime, s_oldTime = 0;
    static long long unsigned int s_sum = 0;
    static int s_steps = 0;

    s_steps++;
    time(&s_currentTime);

    if (s_currentTime - s_oldTime >= 1) {
        if (warmupCount >= 0) {
            s_sum += s_steps;
        }
        warmupCount++;

        float avg = 0;
        if (warmupCount > 0) {
            avg = float(s_sum) / float(warmupCount);
        }
        std::cout << warmupCount << ". FPS: " << s_steps << ", AVG: " << avg << "" << std::endl;

        // Reset.
        s_steps = 0;
        time(&s_oldTime);
    }
}

void drawRects(gepard::Gepard& g, int x, int y)
{
    static const int paintRectSize = s_configs["paintRectSize"];
    static const int rectNumbers = s_configs["rectNumbers"];
    static const int rectWidth = s_configs["rectWidth"];
    static const int rectHeight = s_configs["rectHeight"];

    static const int s_maxSize2 = 2 * paintRectSize;
    static const int s_maxFlet = 2 * paintRectSize * paintRectSize;
    static const float s_ratio = float(255) / float(s_maxFlet);

    for (int i = 0; i < rectNumbers; ++i) {
        int dx = std::rand() % (s_maxSize2) - paintRectSize;
        int dy = std::rand() % (s_maxSize2) - paintRectSize;
        int a = 255 - int(float(dx * dx + dy * dy) * s_ratio);
        g.setFillColor(std::rand() % 256, std::rand() % 256, std::rand() % 256, a);
        g.fillRect(x + dx, y + dy, rectWidth, rectHeight);
        step();
     }
}

inline int newVelocity(int x)
{
    const int sign = x < 0 ? -1 : 1;
    static const int maxVelocity = s_configs["maxVelocity"];

    x = abs(x + std::rand() % 7 - 3);

    if (x < 1) {
        x = 1;
    }
    if (x > maxVelocity) {
        x = maxVelocity;
    }

    return sign * x;
}

int main(int argc, char* argv[])
{
    std::cout << "*** Mini-BenchMark for Gepard, 2016, *** " << std::endl;
    std::cout << "Usage: " << argv[0] << " [config-file]" << std::endl;

    // Set default configs.
    s_configs["paintRectSize"] = 30;
    s_configs["rectHeight"] = 5;
    s_configs["rectNumbers"] = 20;
    s_configs["rectWidth"] = 5;
    s_configs["seed"] = 1985;
    s_configs["maxVelocity"] = 51;
    s_configs["warmupCount"] = 3;
    s_configs["windowHeight"] = 500;
    s_configs["windowWidth"] = 500;

    std::cout << "Read config file...";
    if (argc > 1) {
        std::fstream fs(argv[1], std::fstream::in);
        std::string line;

        while (std::getline(fs, line)) {
            std::istringstream issLine(line);
            std::string key;

            if (std::getline(issLine, key, '=')) {
                std::string value;

                if (std::getline(issLine, value)) {
                    s_configs[key] = atoi(value.c_str());
                }
            }
        }

        std::cout << "done.";
    } else {
        std::cout << "not found. Use default configs.";
    }
    std::cout << std::endl << std::endl;

    std::cout << " * Configs:" << std::endl;
    for (ConfigMap::iterator config = s_configs.begin(); config != s_configs.end(); ++config) {
        std::cout << config->first << "=" << config->second << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Mini-BenchMark start..." << std::endl;

    static const int windowWidth = s_configs["windowWidth"];
    static const int windowHeight = s_configs["windowHeight"];
    static const int maxVelocity = s_configs["maxVelocity"];
    static const int paintRectSize = s_configs["paintRectSize"];

    std::srand(s_configs["seed"]);

    int x = (windowWidth / 4) + std::rand() % (windowWidth / 2);
    int y = (windowHeight / 4) + std::rand() % (windowHeight / 2);
    int dx = std::rand() % maxVelocity - trunc(maxVelocity / 2);
    int dy = std::rand() % maxVelocity - trunc(maxVelocity / 2);

    // Draw on XWindow
    gepard::XSurface surface(windowWidth, windowHeight);
    gepard::Gepard gepard(&surface);
    XEvent e;

    while (true) {
        x += dx;
        if (x - paintRectSize < 0 || x + paintRectSize > windowWidth) {
            dx = -dx;
            x += dx;
            dx = newVelocity(dx);
        }

        y += dy;
        if (y - paintRectSize < 0 || y + paintRectSize > windowHeight) {
            dy = -dy;
            y += dy;
            dy = newVelocity(dy);
        }

        drawRects(gepard, x, y);

        if (XCheckWindowEvent((Display*)surface.getDisplay(), (Window)surface.getWindow(), KeyPress | ClientMessage, &e)) {
            break;
        }
    }

    std::cout << "Stop" << std::endl;
    return 0;
}
