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
#include <list>
#include <map>
#include <math.h>
#include <memory>
#include <sstream>
#include <string>
#include <time.h>

namespace {

#ifdef LOG
#undef LOG
#endif
#define LOG(X) do { std::cout << X << std::endl; } while(false)

class MiniBench;
typedef std::unique_ptr<MiniBench> MiniBenchUniPtr;
typedef std::map<std::string, int> ConfigMap;

template<typename T>
T clamp(T x, const T min, const T max)
{
    return std::min(std::max(x, min), max);
}

class MiniBench {
public:
    MiniBench(ConfigMap& configMap)
        : configs(configMap)
        , sum(0)
        , currentTime(0)
        , oldTime(0)
        , steps(0)
    {
        warmupCount = -configs["warmupCount"];
    }

    void step()
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

    virtual void start() = 0;
    virtual void run() = 0;
    virtual void stop() = 0;

    ConfigMap& configs;
    long long unsigned int sum;
    time_t currentTime;
    time_t oldTime ;
    int steps;
    int warmupCount;
};

class SnakeBench : public MiniBench {
public:
    SnakeBench(ConfigMap configMap)
        : MiniBench(configMap)
        , windowWidth(configs["windowWidth"])
        , windowHeight(configs["windowHeight"])
        , maxVelocity(configs["maxVelocity"])
        , paintRectSize(configs["paintRectSize"])
        , rectNumbers(configs["rectNumbers"])
        , rectWidth(configs["rectWidth"])
        , rectHeight(configs["rectHeight"])
        , maxSize2(2 * paintRectSize)
        , maxFlet(2 * paintRectSize * paintRectSize)
        , ratio(float(255) / float(maxFlet))
        , iterateCount(configs["iterateCount"])
    {
        std::srand(configs["seed"]);

        std::unique_ptr<gepard::XSurface> s(new gepard::XSurface(windowWidth, windowHeight));
        surface = std::move(s);
        std::unique_ptr<gepard::Gepard> g(new gepard::Gepard(surface.get()));
        gepard = std::move(g);
    }

    void drawRects(int x, int y)
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

    int newVelocity(int x)
    {
        const int sign = x < 0 ? -1 : 1;

        x = abs(x + std::rand() % 7 - 3);

        if (x < 1) {
            x = 1;
        }

        x = clamp(x, 1, maxVelocity);

        return sign * x;
    }

    void start()
    {
        LOG("SnakeBenchMark start...");
    }

    void stop()
    {
        LOG("...SnakeBenchMark stop.");
    }

    void run()
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
                break;
            }
        }
    }

    const int windowWidth;
    const int windowHeight;
    const int maxVelocity;
    const int paintRectSize;
    const int rectNumbers;
    const int rectWidth;
    const int rectHeight;
    const int maxSize2;
    const int maxFlet;
    const float ratio;
    const int iterateCount;

    std::unique_ptr<gepard::XSurface> surface;
    std::unique_ptr<gepard::Gepard> gepard;
};

} // anonymous namespace

int main(int argc, char* argv[])
{
    LOG("*** Mini-BenchMark for Gepard, 2016, *** ");
    LOG("Usage: " << argv[0] << " [config-file]");

    std::string configFile = "";
    if (argc > 1) {
        configFile = argv[1];
    }

    // Set default configs.
    ConfigMap configs;
    configs["maxVelocity"] = 51;
    configs["paintRectSize"] = 30;
    configs["rectHeight"] = 5;
    configs["rectNumbers"] = 20;
    configs["rectWidth"] = 5;
    configs["seed"] = 1985;
    configs["warmupCount"] = 3;
    configs["windowHeight"] = 500;
    configs["windowWidth"] = 500;
    configs["iterateCount"] = 1000;

    LOG("Read config file...");
    if (configFile.empty()) {
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

    std::list<MiniBenchUniPtr> benchMarks;

    MiniBenchUniPtr bm1(new SnakeBench(configs));
    benchMarks.push_back(std::move(bm1));

    MiniBenchUniPtr bm2(new SnakeBench(configs));
    benchMarks.push_back(std::move(bm2));

    MiniBenchUniPtr bm3(new SnakeBench(configs));
    benchMarks.push_back(std::move(bm3));


    LOG("Mini-BenchMark start...");
    for (std::list<MiniBenchUniPtr>::iterator it = benchMarks.begin(); it != benchMarks.end(); ++it) {
        it->get()->start();
        it->get()->run();
        it->get()->stop();
    }
    LOG("Stop");

    return 0;
}
