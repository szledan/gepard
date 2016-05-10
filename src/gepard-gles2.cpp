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

#ifdef USE_GLES2

#include "gepard-gles2.h"

#include "gepard-defs.h"

namespace gepard {

/*!
 * \brief GepardGLES2::closePath
 *
 * \todo unimplemented function
 */
void GepardGLES2::closePath()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::moveTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardGLES2::moveTo(Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::lineTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardGLES2::lineTo(Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::quadraticCurveTo
 * \param cpx  X-axis value of _control_ poin
 * \param cpy  Y-axis value of _control_ poin
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardGLES2::quadraticCurveTo(Float cpx, Float cpy, Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::bezierCurveTo
 * \param cp1x  X-axis value of _first control_ point
 * \param cp1y  Y-axis value of _first control_ point
 * \param cp2x  X-axis value of _second control_ point
 * \param cp2y  Y-axis value of _second control_ point
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardGLES2::bezierCurveTo(Float cp1x, Float cp1y, Float cp2x, Float cp2y, Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::arcTo
 * \param x1  X-axis value of _tangent_ point
 * \param y1  Y-axis value of _tangent_ point
 * \param x2  X-axis value of _end_ point
 * \param y2  Y-axis value of _end_ point
 * \param radius  size of arc
 *
 * \todo unimplemented function
 */
void GepardGLES2::arcTo(Float x1, Float y1, Float x2, Float y2, Float radius)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::rect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \todo unimplemented function
 */
void GepardGLES2::rect(Float x, Float y, Float w, Float h)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::arc
 * \param x  X-axis value of _center_ point
 * \param y  Y-axis value of _center_ point
 * \param radius  size of arc
 * \param startAngle  specify the _start_ point on arc
 * \param endAngle  specify the _end_ point on arc
 * \param counterclockwise  specify the draw direction on arc
 *
 * \todo unimplemented function
 */
void GepardGLES2::arc(Float x, Float y, Float radius, Float startAngle, Float endAngle, bool counterclockwise)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::beginPath
 *
 * \todo unimplemented function
 */
void GepardGLES2::beginPath()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::fill
 *
 * \todo unimplemented function
 */
void GepardGLES2::fill()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::stroke
 *
 * \todo unimplemented function
 */
void GepardGLES2::stroke()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::drawFocusIfNeeded
 *
 * \todo unimplemented function
 */
void GepardGLES2::drawFocusIfNeeded(/*Element element*/)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::clip
 *
 * \todo unimplemented function
 */
void GepardGLES2::clip()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::isPointInPath
 * \param x  X-axis value of the given point
 * \param y  Y-axis value of the given point
 * \return  true if the given _point_ is in the current path
 *
 * \todo unimplemented function
 */
bool GepardGLES2::isPointInPath(Float x, Float y)
{
    NOT_IMPLEMENTED();
    return false;
}

} // namespace gepard

#endif // USE_GLES2
