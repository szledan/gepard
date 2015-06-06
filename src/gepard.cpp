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

#include "gepard.h"

namespace gepard {

void Gepard::moveTo(float x, float y)
{
    _path->pathData().addMoveToElement(FloatPoint(x, y));
}

void Gepard::closePath()
{
    _path->pathData().addCloseSubpath();
}

void Gepard::lineTo(float x, float y)
{
    _path->pathData().addLineToElement(FloatPoint(x, y));
}

void Gepard::quadraticCurveTo(float cpx, float cpy, float x, float y)
{
    _path->pathData().addQuadaraticCurveToElement(FloatPoint(cpx, cpy), FloatPoint(x, y));
}

void Gepard::bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y)
{
    _path->pathData().addBezierCurveToElement(FloatPoint(cp1x, cp1y), FloatPoint(cp2x, cp2y), FloatPoint(x, y));
}

void Gepard::arcTo(float x1 ATTR_UNUSED, float y1 ATTR_UNUSED, float x2 ATTR_UNUSED, float y2 ATTR_UNUSED, float radius ATTR_UNUSED)
{
    // FIXME: Unimplemented
}

void Gepard::arc(float x, float y, float radius, float startAngle, float endAngle, bool anticlockwise)
{
    _path->pathData().addArcElement(FloatPoint(x, y), FloatPoint(radius, radius), startAngle, endAngle, anticlockwise);
}

void Gepard::rect(float x ATTR_UNUSED, float y ATTR_UNUSED, float w ATTR_UNUSED, float h ATTR_UNUSED)
{
    // FIXME: Unimplemented
}

void Gepard::beginPath()
{
    if (!_path)
        _path = new Path(_surface);
}

void Gepard::fill()
{
    ASSERT(_path);

    _path->fillPath();
}

void Gepard::stroke()
{
   // FIXME: Unimplemented
}

void Gepard::drawFocusIfNeeded(/* Element& */)
{
    // FIXME: Unimplemented
}

void Gepard::clip()
{
    // FIXME: Unimplemented
}

void Gepard::isPointInPath(const float x ATTR_UNUSED, const float y ATTR_UNUSED)
{
    // FIXME: Unimplemented
}

} // namespace gepard
