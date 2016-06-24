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

#ifndef ANTILOP_BENCHMARK_H
#define ANTILOP_BENCHMARK_H


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

#ifdef LOG
#undef LOG
#endif
/*!
 * \brief A simple LOG macro.
 */
#define LOG(X) do { std::cout << X << std::endl; } while(false)

template<typename T>
T clamp(T x, const T min, const T max)
{
    return std::min(std::max(x, min), max);
}

typedef std::map<std::string, int> ConfigMap;

/*!
 * \brief The AntilopMark class
 */
class AntilopMark {
public:
    AntilopMark();

    virtual void init(ConfigMap& configMap, gepard::XSurface*) = 0;
    virtual void start() = 0;
    virtual int run() = 0;
    virtual int stop() = 0;

    void step();

    ConfigMap configs;
    int warmupCount;
    int steps;
    long long unsigned int sum;
    time_t currentTime;
    time_t oldTime ;
};

/*!
 * \brief The Antilop class
 */
class Antilop {
public:
    typedef enum NonFails {
        PASS = 0,
        EXIT = -1,
    } NonFails;

    void init(int argc, char* argv[]);
    void add(AntilopMark*);
    int run();

private:
    std::list<AntilopMark*> benchMarks;
    ConfigMap configs;
    std::unique_ptr<gepard::XSurface> surface;
};

/*!
 * \brief The SnakeMark class
 */
class SnakeMark : public AntilopMark {
public:
    SnakeMark() : surface(nullptr), gepard(nullptr) {}

    void init(ConfigMap& configMap, gepard::XSurface*);
    void start();
    int stop();
    int run();

    int iterateCount;
    int maxVelocity;
    int paintRectSize;
    int rectHeight;
    int rectNumbers;
    int rectWidth;
    int windowHeight;
    int windowWidth;

    int maxSize2;
    int maxFlet;
    float ratio;

private:
    void drawRects(int x, int y);
    int newVelocity(int x);

    gepard::XSurface* surface;
    gepard::Gepard* gepard;
};

} // antilop namespace

#endif // ANTILOP_BENCHMARK_H
