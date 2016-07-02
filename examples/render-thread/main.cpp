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
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

static const int64_t secondInMillisec = 1000;
static const float millisecFactor = float(1.0f) / float(5.0f);
bool g_rendering = false;
std::mutex g_mutex;

void renderScheduler()
{
    do {
        std::this_thread::sleep_for(std::chrono::milliseconds(int64_t(secondInMillisec * millisecFactor)));

        {
            std::lock_guard<std::mutex> guard(g_mutex);
            g_rendering = true;
        }
    } while (true);
}

int main()
{
    const int width = 500;
    const int height = 500;
    gepard::XSurface surface(width, height);
    gepard::Gepard gepard(&surface);
    std::thread renderThread(renderScheduler);
    std::srand(1985);

    XEvent e;
    long long unsigned int rectCount = 0;
    long long unsigned int lastRectCount = 0;
    float secPartCounter = 0.0f;
    long long unsigned int oneSecRectCount = 0;
    do {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));

        bool rendering;
        {
            std::lock_guard<std::mutex> guard(g_mutex);
            rendering = g_rendering;
        }

        gepard.setFillColor(std::rand() % 256, std::rand() % 256, std::rand() % 256, std::rand() % 256);
        gepard.fillRect(std::rand() % width, std::rand() % height, std::rand() % 100, std::rand() % 100);
        rectCount++;

        if (rendering) {
            secPartCounter += millisecFactor;
            gepard.render();
            {
                std::lock_guard<std::mutex> guard(g_mutex);
                g_rendering = false;
            }
            std::cout << "\rRectangles: " << rectCount << ", from last rendering: " << rectCount - lastRectCount;
            if (secPartCounter >= 1) {
                std::cout << ", from last one second: " << rectCount - oneSecRectCount;
                oneSecRectCount = rectCount;
                secPartCounter = 0.0f;
            }
            std::cout << std::flush;
            lastRectCount = rectCount;
        }

        if (XCheckWindowEvent((Display*)surface.getDisplay(), (Window)surface.getWindow(), KeyPress | ClientMessage, &e)) {
            if (e.type == KeyPress && XLookupKeysym(&e.xkey, 0) == XK_Escape) {
                break;
            }
        }
    } while (true);

    renderThread.detach();
    std::cout << std::endl;

    return 0;
}
