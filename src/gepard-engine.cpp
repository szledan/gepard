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

#include "gepard-engine.h"

#include "gepard-types.h"

namespace gepard {

/*!
 * \brief GepardEngine::closePath
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::closePath()
{
    _context.path.pathData()->addCloseSubpathElement();
}

/*!
 * \brief GepardEngine::moveTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::moveTo(Float x, Float y)
{
    _context.path.pathData()->addMoveToElement(FloatPoint(x, y));
}

/*!
 * \brief GepardEngine::lineTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::lineTo(Float x, Float y)
{
    _context.path.pathData()->addLineToElement(FloatPoint(x, y));
}

/*!
 * \brief GepardEngine::quadraticCurveTo
 * \param cpx  X-axis value of _control_ poin
 * \param cpy  Y-axis value of _control_ poin
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::quadraticCurveTo(Float cpx, Float cpy, Float x, Float y)
{
    _context.path.pathData()->addQuadaraticCurveToElement(FloatPoint(cpx, cpy), FloatPoint(x, y));
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
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::bezierCurveTo(Float cp1x, Float cp1y, Float cp2x, Float cp2y, Float x, Float y)
{
    _context.path.pathData()->addBezierCurveToElement(FloatPoint(cp1x, cp1y), FloatPoint(cp2x, cp2y), FloatPoint(x, y));
}

/*!
 * \brief GepardEngine::arcTo
 * \param x1  X-axis value of _tangent_ point
 * \param y1  Y-axis value of _tangent_ point
 * \param x2  X-axis value of _end_ point
 * \param y2  Y-axis value of _end_ point
 * \param radius  size of arc
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::arcTo(Float x1, Float y1, Float x2, Float y2, Float radius)
{
    _context.path.pathData()->addArcToElement(FloatPoint(x1, y1), FloatPoint(x2, y2), radius);
}

/*!
 * \brief GepardEngine::rect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::rect(Float x, Float y, Float w, Float h)
{
    moveTo(x, y);
    lineTo(x + w, y);
    lineTo(x + w, y + h);
    lineTo(x, y + h);
    closePath();
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
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::arc(Float x, Float y, Float radius, Float startAngle, Float endAngle, bool counterclockwise)
{
    _context.path.pathData()->addArcElement(FloatPoint(x, y), FloatPoint(radius, radius), startAngle, endAngle, counterclockwise);
}

/*!
 * \brief GepardEngine::beginPath
 *
 * More information:
 * <a href="https://www.w3.org/TR/2dcontext/#drawing-state">https://www.w3.org/TR/2dcontext/#drawing-state</a>.
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::beginPath()
{
    GD_LOG1("Clear path data.");
    _context.path.clear();
}

/*!
 * \brief GepardEngine::fill
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::fill()
{
    GD_ASSERT(_engineBackend);
#ifdef GD_USE_GLES2
    _engineBackend->fillPath(_context.path.pathData(), _context.currentState().fillColor);
#else // !GD_USE_GLES2
    _engineBackend->fill();
#endif // GD_USE_GLES2
}

/*!
 * \brief GepardEngine::stroke
 *
 * \internal
 * \todo unit tests missing
 */
void GepardEngine::stroke()
{
    GD_ASSERT(_engineBackend);
#ifdef GD_USE_GLES2
    _engineBackend->strokePath(_context.path.pathData(), _context.currentState());
#else // !GD_USE_GLES2
    _engineBackend->stroke();
#endif // GD_USE_GLES2
}

/*!
 * \brief GepardEngine::drawFocusIfNeeded
 *
 * \todo unimplemented function
 */
void GepardEngine::drawFocusIfNeeded(/*Element element*/)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardEngine::clip
 *
 * \todo unimplemented function
 */
void GepardEngine::clip()
{
    GD_NOT_IMPLEMENTED();
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
    GD_NOT_IMPLEMENTED();
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
    GD_ASSERT(_engineBackend);
#ifdef GD_USE_GLES2
    _engineBackend->fillRect(x, y, w, h, _context.currentState().fillColor);
#else // !GD_USE_GLES2
    _engineBackend->fillRect(x, y, w, h);
#endif // GD_USE_GLES2
}

void GepardEngine::setFillColor(const Color& color)
{
    GD_LOG1("Set fill color (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")");
    _context.currentState().fillColor = color;
}

void GepardEngine::setFillColor(const Float red, const Float green, const Float blue, const Float alpha)
{
    setFillColor(Color(red, green, blue, alpha));
}

void GepardEngine::setStrokeColor(const Color& color)
{
    GD_LOG1("Set stroke color (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")");
    _context.currentState().strokeColor = color;
}

} // namespace gepard
