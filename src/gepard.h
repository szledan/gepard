/* Copyright (C) 2015, Szilard Ledan <szledan@gmail.com>
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
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#ifndef gepard_h
#define gepard_h

#include "config.h"

#include "gepard-defs.h"
#include "gepard-path.h"
#include "gepard-surface.h"
#include "gepard-texture.h"
#include "gepard-trapezoid-tessellator.h"
#include "gepard-types.h"
#include "gepard-utils.h"
#include <string>

namespace gepard {

class Gepard {
public:
    Gepard(GepardSurface* surface)
        : _surface(surface)
        , _path(0)
    {
        if (surface) {
            // We don't use depth: glEnable(GL_DEPTH_TEST);
            // Note: Depth test is > by default (instead of >=), so the red
            // triangle overlaps with the green which is not our intention.
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glClearColor(0, 0, 0, 0);
        }
    }
    ~Gepard()
    {
        // FIXME: delete _surface;
        delete _path;
    }

    // 5. Building paths (W3-2DContext-2015)
    void moveTo(float x, float y);
    void closePath();
    void lineTo(float x, float y);
    void quadraticCurveTo(float cpx, float cpy, float x, float y);
    void bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y);
    void arcTo(float x1, float y1, float x2, float y2, float radius);
    void arc(float x, float y, float radius, float startAngle, float endAngle, bool anticlockwise = true);
    void rect(float x, float y, float w, float h);

    // 11. Drawing paths to the canvas (W3-2DContext-2015)
    void beginPath();
    void fill(const std::string fillRule = "nonzero");
    void stroke();
    void drawFocusIfNeeded(/* Element& */);
    void clip();
    void isPointInPath(const float x, const float y);

    // Extras
    Path* path() const { return _path; }

private:
    GepardSurface* _surface;
    Path* _path;
};

} // namespace gepard

#endif // gepard_h
