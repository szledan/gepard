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
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#ifdef USE_GLES2

#include "gepard-gles2.h"

namespace gepard {

void GepardGLES2::cloeePath()
{
/** @todo unimplemented function */
}

void GepardGLES2::moveTo(Float x, Float y)
{
/** @todo unimplemented function */
}

void GepardGLES2::lineTo(Float x, Float y)
{
/** @todo unimplemented function */
}

void GepardGLES2::quadraticCurveTo(Float cpx, Float cpy, Float x, Float y)
{
/** @todo unimplemented function */
}

void GepardGLES2::bezierCurveTo(Float cp1x, Float cp1y, Float cp2x, Float cp2y, Float x, Float y)
{
/** @todo unimplemented function */
}

void GepardGLES2::arcTo(Float x1, Float y1, Float x2, Float y2, Float radius)
{
/** @todo unimplemented function */
}

void GepardGLES2::rect(Float x, Float y, Float w, Float h)
{
/** @todo unimplemented function */
}

void GepardGLES2::arc(Float x, Float y, Float radius, Float startAngle, Float endAngle, bool counterclockwise)
{
/** @todo unimplemented function */
}

void GepardGLES2::beginPath()
{
/** @todo unimplemented function */
}

void GepardGLES2::fill()
{
/** @todo unimplemented function */
}

void GepardGLES2::stroke()
{
/** @todo unimplemented function */
}

void GepardGLES2::drawFocusIfNeeded(/*Element element*/)
{
/** @todo unimplemented function */
}

void GepardGLES2::clip()
{
/** @todo unimplemented function */
}

bool GepardGLES2::isPointInPath(Float x, Float y)
{
/** @todo unimplemented function */
    ASSERT(0 && "Unimplemented function!");
    return false;
}

} // namespace gepard

#endif // USE_GLES2
