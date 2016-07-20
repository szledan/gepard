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

#ifdef GD_USE_SOFTWARE

#include "gepard-software.h"

#include "gepard-defs.h"

namespace gepard {
namespace software {

GepardSoftware::GepardSoftware(Surface* surface)
    : _surface(surface)
{
}

GepardSoftware::~GepardSoftware()
{
}

/*!
 * \brief GepardSoftware::closePath
 *
 * \todo unimplemented function
 */
void GepardSoftware::closePath()
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::moveTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardSoftware::moveTo(Float x, Float y)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::lineTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardSoftware::lineTo(Float x, Float y)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::quadraticCurveTo
 * \param cpx  X-axis value of _control_ poin
 * \param cpy  Y-axis value of _control_ poin
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardSoftware::quadraticCurveTo(Float cpx, Float cpy, Float x, Float y)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::bezierCurveTo
 * \param cp1x  X-axis value of _first control_ point
 * \param cp1y  Y-axis value of _first control_ point
 * \param cp2x  X-axis value of _second control_ point
 * \param cp2y  Y-axis value of _second control_ point
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardSoftware::bezierCurveTo(Float cp1x, Float cp1y, Float cp2x, Float cp2y, Float x, Float y)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::arcTo
 * \param x1  X-axis value of _tangent_ point
 * \param y1  Y-axis value of _tangent_ point
 * \param x2  X-axis value of _end_ point
 * \param y2  Y-axis value of _end_ point
 * \param radius  size of arc
 *
 * \todo unimplemented function
 */
void GepardSoftware::arcTo(Float x1, Float y1, Float x2, Float y2, Float radius)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::rect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \todo unimplemented function
 */
void GepardSoftware::rect(Float x, Float y, Float w, Float h)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::arc
 * \param x  X-axis value of _center_ point
 * \param y  Y-axis value of _center_ point
 * \param radius  size of arc
 * \param startAngle  specify the _start_ point on arc
 * \param endAngle  specify the _end_ point on arc
 * \param counterclockwise  specify the draw direction on arc
 *
 * \todo unimplemented function
 */
void GepardSoftware::arc(Float x, Float y, Float radius, Float startAngle, Float endAngle, bool counterclockwise)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::beginPath
 *
 * \todo unimplemented function
 */
void GepardSoftware::beginPath()
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::fill
 *
 * \todo unimplemented function
 */
void GepardSoftware::fill()
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::stroke
 *
 * \todo unimplemented function
 */
void GepardSoftware::stroke()
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::drawFocusIfNeeded
 *
 * \todo unimplemented function
 */
void GepardSoftware::drawFocusIfNeeded(/*Element element*/)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::clip
 *
 * \todo unimplemented function
 */
void GepardSoftware::clip()
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::isPointInPath
 * \param x  X-axis value of the given point
 * \param y  Y-axis value of the given point
 * \return  true if the given _point_ is in the current path
 *
 * \todo unimplemented function
 */
bool GepardSoftware::isPointInPath(Float x, Float y)
{
    GD_NOT_IMPLEMENTED();
    return false;
}

/*!
 * \brief GepardSoftware::fillRect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \todo unimplemented function
 */
void GepardSoftware::fillRect(Float x, Float y, Float w, Float h)
{
}

} // namespace software
} // namespace gepard

#endif // GD_USE_SOFTWARE
