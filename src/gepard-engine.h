/* Copyright (C) 2016-2017, Gepard Graphics
 * Copyright (C) 2016-2017, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_ENGINE_H
#define GEPARD_ENGINE_H

#include "gepard-defs.h"

#include "gepard-color.h"
#include "gepard-context.h"
#include "gepard-float.h"
#include "gepard-float-point.h"
#include "gepard-image.h"
#include "gepard-state.h"


// Include engine backend.
#if defined(GD_USE_GLES2)
#include "gepard-gles2.h"
#elif defined(GD_USE_SOFTWARE)
#include "gepard-software.h"
#elif defined(GD_USE_VULKAN)
#include "gepard-vulkan.h"
#else
#error "No engine backend defined!"
#endif // Include engine backend.

namespace gepard {

class Image;
class Surface;

class GepardEngine {
public:
    explicit GepardEngine(Surface* surface)
        : _context(surface)
        , _engineBackend(new GepardEngineBackend(_context))
    {
    }
    ~GepardEngine()
    {
        if (_engineBackend) {
            delete _engineBackend;
        }
    }

    /* 2. State */
    void save();
    void restore();

    /* 5. Building paths */
    void closePath();
    void moveTo(Float x, Float y);
    void lineTo(Float x, Float y);
    void quadraticCurveTo(Float cpx, Float cpy, Float x, Float y);
    void bezierCurveTo(Float cp1x, Float cp1y, Float cp2x, Float cp2y, Float x, Float y);
    void arcTo(Float x1, Float y1, Float x2, Float y2, Float radius);
    void rect(Float x, Float y, Float w, Float h);
    void arc(Float x, Float y, Float radius, Float startAngle, Float endAngle, bool counterclockwise = false);

    /* 6. Transformations */
    void scale(Float x, Float y);
    void rotate(Float angle);
    void translate(Float x, Float y);
    void transform(Float a, Float b, Float c, Float d, Float e, Float f);
    void setTransform(Float a, Float b, Float c, Float d, Float e, Float f);

    /* 11. Drawing paths to the canvas */
    void beginPath();
    void fill();
    void stroke();
    void drawFocusIfNeeded(/*Element element*/);
    void clip();
    bool isPointInPath(Float x, Float y);

    void fillRect(Float x, Float y, Float w, Float h);

    void drawImage(Image& imagedata, Float sx, Float sy, Float sw, Float sh, Float dx, Float dy, Float dw, Float dh);
    Image getImage(Float sx, Float sy, Float sw, Float sh);
    void putImage(Image& imagedata, Float dx, Float dy, Float dirtyX, Float dirtyY, Float dirtyWidth, Float dirtyHeight);

    void setFillColor(const Color& color);
    void setFillColor(const Float red, const Float green, const Float blue, const Float alpha = 1.0f);

    void setStrokeColor(const Color& color);

    void setFillStyle(const std::string&);
    void setStrokeStyle(const std::string&);
    void setLineWidth(const std::string&);
    void setLineCap(const std::string&);
    void setLineJoin(const std::string&);
    void setMiterLimit(const std::string&);

    GepardContext& context() { return _context; }

private:
    GepardState& state();

    GepardContext _context;
    GepardEngineBackend* _engineBackend;
};

} // namespace gepard

#endif // GEPARD_ENGINE_H
