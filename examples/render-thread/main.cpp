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

bool g_rendering = false;
std::mutex g_mutex;

void renderScheduler()
{
    do {
        std::this_thread::sleep_for(std::chrono::seconds(1));

        {
            std::lock_guard<std::mutex> guard(g_mutex);
            g_rendering = !g_rendering;
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
    do {
        std::this_thread::sleep_for(std::chrono::microseconds(1));

        bool rendering;
        {
            std::lock_guard<std::mutex> guard(g_mutex);
            rendering = g_rendering;
        }

        if (rendering) {
            gepard.setFillColor(std::rand() % 256, std::rand() % 256, std::rand() % 256, std::rand() % 256);
            gepard.fillRect(std::rand() % width, std::rand() % height, 20, 20);
        }

        if (XCheckWindowEvent((Display*)surface.getDisplay(), (Window)surface.getWindow(), KeyPress | ClientMessage, &e)) {
            if (e.type == KeyPress && XLookupKeysym(&e.xkey, 0) == XK_Escape) {
                break;
            }
        }
    } while (true);

    renderThread.detach();

    return 0;
}
