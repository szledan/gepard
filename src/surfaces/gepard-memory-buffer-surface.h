/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2016, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
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

#ifndef GEPARD_MEMORY_BUFFER_SURFACE_H
#define GEPARD_MEMORY_BUFFER_SURFACE_H

#include "gepard.h"

#include <cstdlib>

namespace gepard {

/*!
 * \brief A simple memory buffer surface class for _Gepard_
 *
 * \todo: documentation is missing.
 */
class MemoryBufferSurface : public Surface {
public:
    MemoryBufferSurface(uint32_t width = 0, uint32_t height = 0)
        : Surface(width, height)
    {
        buffer = std::malloc(width * height * 4);
    }

    virtual ~MemoryBufferSurface()
    {
        if (buffer) {
            std::free(buffer);
        }
    }

    virtual void* getDisplay() { return nullptr; }
    virtual unsigned long getWindow() { return 0; }
    virtual void* getBuffer() { return buffer; }

private:
    void* buffer;
};

} // namespace gepard

#endif // GEPARD_MEMORY_BUFFER_SURFACE_H
