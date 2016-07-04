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

#include "gepard-engine.h"

namespace gepard {

/*!
 * \brief GepardEngine::closePath
 */
void GepardEngine::closePath()
{
    if (_engineBackend) {
        _engineBackend->closePath();
    }
}

/*!
 * \brief GepardEngine::moveTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardEngine::moveTo(Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::lineTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardEngine::lineTo(Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::quadraticCurveTo
 * \param cpx  X-axis value of _control_ poin
 * \param cpy  Y-axis value of _control_ poin
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardEngine::quadraticCurveTo(Float cpx, Float cpy, Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::bezierCurveTo
 * \param cp1x  X-axis value of _first control_ point
 * \param cp1y  Y-axis value of _first control_ point
 * \param cp2x  X-axis value of _second control_ point
 * \param cp2y  Y-axis value of _second control_ point
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardEngine::bezierCurveTo(Float cp1x, Float cp1y, Float cp2x, Float cp2y, Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::arcTo
 * \param x1  X-axis value of _tangent_ point
 * \param y1  Y-axis value of _tangent_ point
 * \param x2  X-axis value of _end_ point
 * \param y2  Y-axis value of _end_ point
 * \param radius  size of arc
 *
 * \todo unimplemented function
 */
void GepardEngine::arcTo(Float x1, Float y1, Float x2, Float y2, Float radius)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::rect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \todo unimplemented function
 */
void GepardEngine::rect(Float x, Float y, Float w, Float h)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::arc
 * \param x  X-axis value of _center_ point
 * \param y  Y-axis value of _center_ point
 * \param radius  size of arc
 * \param startAngle  specify the _start_ point on arc
 * \param endAngle  specify the _end_ point on arc
 * \param counterclockwise  specify the draw direction on arc
 *
 * \todo unimplemented function
 */
void GepardEngine::arc(Float x, Float y, Float radius, Float startAngle, Float endAngle, bool counterclockwise)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::beginPath
 *
 * \todo unimplemented function
 */
void GepardEngine::beginPath()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::fill
 *
 * \todo unimplemented function
 */
void GepardEngine::fill()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::stroke
 *
 * \todo unimplemented function
 */
void GepardEngine::stroke()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::drawFocusIfNeeded
 *
 * \todo unimplemented function
 */
void GepardEngine::drawFocusIfNeeded(/*Element element*/)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::clip
 *
 * \todo unimplemented function
 */
void GepardEngine::clip()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::isPointInPath
 * \param x  X-axis value of the given point
 * \param y  Y-axis value of the given point
 * \return  true if the given _point_ is in the current path
 *
 * \todo unimplemented function
 */
bool GepardEngine::isPointInPath(Float x, Float y)
{
    NOT_IMPLEMENTED();
    return false;
}

/*!
 * \brief GepardEngine::fillRect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \todo documentation is missing
 */
void GepardEngine::fillRect(Float x, Float y, Float w, Float h)
{
    if (_engineBackend) {
        _engineBackend->fillRect(x, y, w, h);
    }
}

void GepardEngine::setFillColor(const Float red, const Float green, const Float blue, const Float alpha)
{
    GD_LOG1("Set fill color (" << red << ", " << green << ", " << blue << ", " << alpha << ")");
    _engineBackend->state.fillColor = Color(red, green, blue, alpha);
}

int GepardEngine::draw()
{
    int renderedTriangles = -1;
    if (_engineBackend) {
        renderedTriangles = _engineBackend->draw();
    }
    return renderedTriangles;
}

} // namespace gepard
