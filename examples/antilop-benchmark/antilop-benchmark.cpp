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

#include "antilop-benchmark.h"
#include "gepard.h"
#include "gepard-xsurface.h"
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <sstream>
#include <string>
#include <time.h>

namespace antilop {

// AntilopMark

AntilopMark::AntilopMark()
    : warmupCount(0)
    , steps(0)
    , sum(0)
    , currentTime(0)
    , oldTime(0)
{
}

void AntilopMark::step()
{
    steps++;
    time(&currentTime);

    if (currentTime - oldTime >= 1) {

        if (warmupCount >= 0) {
            sum += steps;
        }
        warmupCount++;

        float avg = 0;
        if (warmupCount > 0) {
            avg = float(sum) / float(warmupCount);
        }
        LOG(warmupCount << ". FPS: " << steps << ", AVG: " << avg << "");

        // Reset.
        steps = 0;
        time(&oldTime);
    }
}

// Antilop

template<typename T>
inline static std::unique_ptr<T> makeUnique(T* ptr)
{
    std::unique_ptr<T> uniquePtr(ptr);
    return std::move(uniquePtr);
}

void Antilop::init(int argc, char* argv[])
{
    LOG("*** Mini-BenchMark for Gepard, 2016, *** ");
    LOG("Usage: " << argv[0] << " [config-file]");

    std::string configFile = "";
    if (argc > 1) {
        configFile = argv[1];
    }

    // Set default configs.
    configs["iterateCount"] = 100;
    configs["maxVelocity"] = 51;
    configs["paintRectSize"] = 30;
    configs["rectHeight"] = 5;
    configs["rectNumbers"] = 20;
    configs["rectWidth"] = 5;
    configs["seed"] = 1985;
    configs["warmupCount"] = 3;
    configs["windowHeight"] = 500;
    configs["windowWidth"] = 500;

    LOG("Read config file...");
    if (!configFile.empty()) {
        std::fstream fs(configFile, std::fstream::in);
        std::string line;

        while (std::getline(fs, line)) {
            std::istringstream issLine(line);
            std::string key;

            if (std::getline(issLine, key, '=')) {
                std::string value;

                if (std::getline(issLine, value)) {
                    configs[key] = atoi(value.c_str());
                }
            }
        }

        LOG("...done.");
    } else {
        LOG("...not found. Use default configs.");
    }
    LOG("");

    LOG(" * Configs:");
    for (ConfigMap::iterator config = configs.begin(); config != configs.end(); ++config) {
        LOG(config->first << "=" << config->second);
    }
    LOG("");

    surface = makeUnique(new gepard::XSurface(configs["windowWidth"], configs["windowHeight"]));
}

void Antilop::add(AntilopMark* antilopMark)
{
    benchMarks.push_back(antilopMark);
}

int Antilop::run()
{
    LOG("Antilop BenchMark start...");
    int fail = 0;
    for (std::list<AntilopMark*>::iterator it = benchMarks.begin(); it != benchMarks.end() && !fail; ++it) {
        (*it)->init(configs, surface.get());
        (*it)->start();
        fail = (*it)->run();
        (*it)->stop();
    }
    LOG("Stop");
    return fail > 0 ? fail : 0;
}

// SnakeMark

void SnakeMark::init(ConfigMap& configMap, gepard::XSurface* srfc)
{
    configs = configMap;

    iterateCount = configs["iterateCount"];
    maxVelocity = configs["maxVelocity"];
    paintRectSize = configs["paintRectSize"];
    rectHeight = configs["rectHeight"];
    rectNumbers = configs["rectNumbers"];
    rectWidth = configs["rectWidth"];
    warmupCount = -configs["warmupCount"];
    windowHeight = configs["windowHeight"];
    windowWidth = configs["windowWidth"];

    maxSize2 = 2 * paintRectSize;
    maxFlet = 2 * paintRectSize * paintRectSize;
    ratio = float(255) / float(maxFlet);

    std::srand(configs["seed"]);

    surface = srfc;
}

void SnakeMark::start()
{
    gepard = new gepard::Gepard(surface);

    LOG("SnakeBenchMark START.");
}

int SnakeMark::stop()
{
    LOG("SnakeBenchMark STOP.");

    if (gepard) {
        delete gepard;
    }

    return Antilop::PASS;
}

int SnakeMark::run()
{
    int x = (windowWidth / 4) + std::rand() % (windowWidth / 2);
    int y = (windowHeight / 4) + std::rand() % (windowHeight / 2);
    int dx = std::rand() % maxVelocity - trunc(maxVelocity / 2);
    int dy = std::rand() % maxVelocity - trunc(maxVelocity / 2);

    XEvent e;

    for (int i = 0; i <= iterateCount; ++i) {
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

        drawRects(x, y);

        if (XCheckWindowEvent((Display*)surface->getDisplay(), (Window)surface->getWindow(), KeyPress | ClientMessage, &e)) {
            if (e.type == KeyPress && XLookupKeysym(&e.xkey, 0) == XK_Escape) {
                std::cout << "EXITING at " << iterateCount << std::endl;
                return Antilop::EXIT;
            }
        }
    }

    return Antilop::PASS;
}

void SnakeMark::drawRects(int x, int y)
{
    for (int i = 0; i < rectNumbers; ++i) {
        int dx = std::rand() % (maxSize2) - paintRectSize;
        int dy = std::rand() % (maxSize2) - paintRectSize;
        int a = 255 - int(float(dx * dx + dy * dy) * ratio);
        gepard->setFillColor(std::rand() % 256, std::rand() % 256, std::rand() % 256, a);
        gepard->fillRect(x + dx, y + dy, rectWidth, rectHeight);
        step();
     }
}

int SnakeMark::newVelocity(int x)
{
    const int sign = x < 0 ? -1 : 1;

    return sign * clamp(abs(x + std::rand() % 7 - 3), 1, maxVelocity);
}

} // antilop namespace
