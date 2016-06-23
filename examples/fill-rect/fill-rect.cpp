/* Copyright (C) 2015-2016, Gepard Graphics
 * Copyright (C) 2015-2016, Szilard Ledan <szledan@gmail.com>
 * Copyright (C) 2015, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
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
#include "gepard-memory-buffer-surface.h"
#include <iostream>

int main()
{
    // Draw on XWindow
    gepard::XSurface s(600, 600);
    gepard::Gepard g(&s);

    g.setFillColor(0.5f, 0.4f, 0.1f, 0.2f);
    g.fillRect(50, 50, 500, 500);

    g.setFillColor(0.0f, 0.8f, 0.3f);
    g.fillRect(100, 100, 80, 320);

    g.setFillColor(0.0f, 0.0f, 1.0f);
    g.fillRect(100, 420, 280, 80);

    g.setFillColor(0.3f, 0.0f, 0.7f);
    g.fillRect(180, 100, 200, 80);

    g.setFillColor(0.3f, 0.7f, 0.2f);
    g.fillRect(380, 80, 80, 130);

    g.setFillColor("#af5f4f");
    g.fillRect(380, 380, 80, 130);

    g.setFillColor(220, 180, 40);
    g.fillRect(330, 320, 160, 60);

    char a;
    std::cin >> a;

    // Draw to memory buffer
    gepard::MemoryBufferSurface s2(500, 500);
    gepard::Gepard g2(&s2);

    g2.fillRect(100, 100, 400, 300);

    return 0;
}
