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
    if (!_path) {
        beginPath();
    }

    _path->pathData().addMoveToElement(FloatPoint(x, y));
}

void Gepard::closePath()
{
    if (_path) {
        _path->pathData().addCloseSubpathElement();
    }
}

void Gepard::lineTo(float x, float y)
{
    if (!_path) {
        moveTo(x, y);
    } else {
        _path->pathData().addLineToElement(FloatPoint(x, y));
    }
}

void Gepard::quadraticCurveTo(float cpx, float cpy, float x, float y)
{
    if (!_path) {
        moveTo(cpx, cpy);
    }

    _path->pathData().addQuadaraticCurveToElement(FloatPoint(cpx, cpy), FloatPoint(x, y));
}

void Gepard::bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y)
{
    if (!_path) {
        moveTo(cp1x, cp1y);
    }

    _path->pathData().addBezierCurveToElement(FloatPoint(cp1x, cp1y), FloatPoint(cp2x, cp2y), FloatPoint(x, y));
}

void Gepard::arcTo(float x1, float y1, float x2, float y2, float radius)
{
    if (!_path) {
        moveTo(x1, y1);
    }

    _path->pathData().addArcToElement(FloatPoint(x1, y1), FloatPoint(x2, y2), radius);
}

void Gepard::arc(float x, float y, float radius, float startAngle, float endAngle, bool anticlockwise)
{
    if (!_path) {
        moveTo(x, y);
    }

    _path->pathData().addArcElement(FloatPoint(x, y), FloatPoint(radius, radius), startAngle, endAngle, anticlockwise);
}

void Gepard::rect(float x, float y, float w, float h)
{
    moveTo(x, y);
    lineTo(x + w, y);
    lineTo(x + w, y + h);
    lineTo(x, y + h);
    closePath();
}

void Gepard::beginPath()
{
    if (!_path) {
        _path = new Path(_surface);
    } else {
        // TODO: reset path! -> make unit-test
    }
}

void Gepard::fill(const std::string fillRule)
{
    if (_path) {
        closePath();
        _path->fillPath(_fillColorRGBA, fillRule);
    }
}

void Gepard::stroke()
{
   // TODO: Unimplemented
}

void Gepard::drawFocusIfNeeded(/* Element& */)
{
    // TODO: Unimplemented
}

void Gepard::clip()
{
    // TODO: Unimplemented
}

void Gepard::isPointInPath(const float x ATTR_UNUSED, const float y ATTR_UNUSED)
{
    // TODO: Unimplemented
}

void Gepard::setFillColor(const int red, const int green, const int blue, const int alpha)
{
    _fillColorRGBA = Color(red, green, blue, alpha);
}

} // namespace gepard
