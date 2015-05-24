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
    // FIXME: Unimplemented
}

void Gepard::lineTo(float x ATTR_UNUSED, float y ATTR_UNUSED)
{
    // FIXME: Unimplemented
}

void Gepard::quadraticCurveTo(float cpx ATTR_UNUSED, float cpy ATTR_UNUSED, float x ATTR_UNUSED, float y ATTR_UNUSED)
{
    // FIXME: Unimplemented
}

void Gepard::bezierCurveTo(float cp1x ATTR_UNUSED, float cp1y ATTR_UNUSED, float cp2x ATTR_UNUSED, float cp2y ATTR_UNUSED, float x ATTR_UNUSED, float y ATTR_UNUSED)
{
    // FIXME: Unimplemented
}

void Gepard::arcTo(float x1 ATTR_UNUSED, float y1 ATTR_UNUSED, float x2 ATTR_UNUSED, float y2 ATTR_UNUSED, float radius ATTR_UNUSED)
{
    // FIXME: Unimplemented
}

void Gepard::arc(float x ATTR_UNUSED, float y ATTR_UNUSED, float radius ATTR_UNUSED, float startAngle ATTR_UNUSED, float endAngle ATTR_UNUSED, bool anticlockwise ATTR_UNUSED)
{
    // FIXME: Unimplemented
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
