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

#ifndef GEPARD_ENGINE_BACKEND_H
#define GEPARD_ENGINE_BACKEND_H

#include "gepard-color.h"
#include "gepard-float.h"
#include "gepard-image.h"
#include "gepard-state.h"

namespace gepard {

class Image;
class Surface;

class GepardEngineBackend {
public:
    virtual ~GepardEngineBackend() {}

    virtual void fillRect(const Float x, const Float y, const Float w, const Float h) = 0;
    virtual void fillPath(PathData*, const GepardState&) = 0;
    virtual void strokePath() = 0;
    virtual void drawImage(const Image& imagedata, const Float sx, const Float sy, const Float sw, const Float sh, const Float dx, const Float dy, const Float dw, const Float dh) = 0;
    virtual void putImage(const Image& imagedata, const Float dx, const Float dy, const Float dirtyX, const Float dirtyY, const Float dirtyWidth, const Float dirtyHeight) = 0;
    virtual const Image getImage(const Float sx, const Float sy, const Float sw, const Float sh) = 0;
};

} // namespace gepard

#endif // GEPARD_ENGINE_BACKEND_H
