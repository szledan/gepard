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
#include "surfaces/gepard-xsurface.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>

std::mutex g_mutex;
static const int64_t g_waitSecondsInMillisec = 1000;
int64_t g_drawingPerSec = 25;
bool g_drawing = false;

void drawScheduler()
{
    do {
        int64_t ms = 1;
        { // lock
            std::lock_guard<std::mutex> guard(g_mutex);
            g_drawing = true;
            ms = g_waitSecondsInMillisec / g_drawingPerSec;
        } // unlock
        std::this_thread::sleep_for(std::chrono::milliseconds(ms));
    } while (true);
}

void showHelp()
{
    std::cout << "Keys:" << std::endl;
    std::cout << " [Esc|q]      - exit." << std::endl;
    std::cout << " [h]          - show this help." << std::endl;
    std::cout << " [Up|Down]    - inc/dec draw() call per scheduler time." << std::endl;
    std::cout << "Columns:" << std::endl;
    std::cout << " d/s    - draw() call per sec in [1-1000]." << std::endl;
    std::cout << " r/d    - avarage number of rectangles per draw() call. In parenthesis: (max - avg)." << std::endl;
    std::cout << " r/s    - number of rectangles per second." << std::endl;
    std::cout << " r(d)/s - number of rectangles in the last second after call draw()." << std::endl;
    std::cout << " r(f)/s - number of rectangles in the last second after buffer was full. (r/s = r(d)/s + r(f)/s)" << std::endl;
    std::cout << " S(r)   - number of all drawn rectangles." << std::endl;
}

int main()
{
    std::cout << "*** Thread driven drawing Gepard, 2016. ***" << std::endl;

    const int width = 500;
    const int height = 500;
    gepard::XSurface surface(width, height);
    gepard::Gepard gepard(&surface);
    std::thread drawThread(drawScheduler);
    std::srand(1985);

    showHelp();

    XEvent e;
    long long int sumRects = 0;
    long long int lastSumRects = 0;
    long long int rectsPerSec = 0;
    long long int rectsFromDrawCall = 0;
    int drawPerSec = 0;
    int drawingPerSec = 0;

    int rectsPerDrawingAvg = 0;
    int rectsPerDrawingDev = 0;
    do {
        std::this_thread::sleep_for(std::chrono::nanoseconds(1));   // Only for CPU sparing.

        // Create a colored fillRect.
        gepard.setFillColor(std::rand() % 256, std::rand() % 256, std::rand() % 256, std::rand() % 256);
        gepard.fillRect(std::rand() % width, std::rand() % height, std::rand() % 100, std::rand() % 100);
        sumRects++;

        // Check drawScheduler.
        bool drawing;
        { // lock
            std::lock_guard<std::mutex> guard(g_mutex);
            drawing = g_drawing;
        } // unlock

        // Call drawing function.
        if (drawing) {
            // Call draw().
            rectsFromDrawCall += gepard.draw() / 2;

            { // lock
                std::lock_guard<std::mutex> guard(g_mutex);
                g_drawing = false;
                drawingPerSec = g_drawingPerSec;
                drawPerSec++;
            } // unlock

            int currentRectsPerDraw = sumRects - lastSumRects;
            rectsPerDrawingDev = std::max(currentRectsPerDraw, rectsPerDrawingDev);
            rectsPerDrawingAvg += currentRectsPerDraw;

            if (drawPerSec >= drawingPerSec) {
                std::cout << drawPerSec << " d/s \t";
                std::cout << "~" << rectsPerDrawingAvg / drawPerSec << "(+";
                std::cout << rectsPerDrawingDev - rectsPerDrawingAvg / drawPerSec << ") r/d \t";
                std::cout << sumRects - rectsPerSec << " r/s \t";
                std::cout << rectsFromDrawCall << " r(d)/s \t";
                std::cout << (sumRects - rectsPerSec) - rectsFromDrawCall << " r(f)/s \t";
                std::cout << sumRects << " S(r) \t";
                std::cout << std::endl;
                rectsPerSec = sumRects;
                lastSumRects = 0;
                drawPerSec = 0;
                rectsFromDrawCall = 0;
                rectsPerDrawingAvg = 0;
                rectsPerDrawingDev = 0;
            }
            lastSumRects = sumRects;
        }

        if (XCheckWindowEvent((Display*)surface.getDisplay(), (Window)surface.getWindow(), KeyPress | ClientMessage, &e)) {
            if (surface.hasToQuit()) {
                break;
            }

            if (e.type == KeyPress) {
                bool wasRenderingDensityChanged = false;
                switch (XLookupKeysym(&e.xkey, 0)) {
                case XK_Up:
                    drawingPerSec++;
                    drawingPerSec = std::min(drawingPerSec, 1000);
                    wasRenderingDensityChanged = true;
                    break;
                case XK_Down:
                    drawingPerSec--;
                    drawingPerSec = std::max(drawingPerSec, 1);
                    wasRenderingDensityChanged = true;
                    break;
                case XK_H:
                case XK_h:
                    showHelp();
                    break;
                default:
                    break;
                };
                if (wasRenderingDensityChanged) { // lock
                    std::lock_guard<std::mutex> guard(g_mutex);
                    g_drawingPerSec = drawingPerSec;
                } // unlock
            }
        }
    } while (true);

    drawThread.detach();
    std::cout << std::endl;

    return 0;
}
