/* Copyright (C) 2015, Kristof Kosztyo
 * Copyright (C) 2015, Szilard Ledan <szledan@gmail.com>
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

void Gepard::save()
{
/** @todo unimplemented function */
}

void Gepard::restore()
{
/** @todo unimplemented function */
}

void Gepard::scale(float x, float y)
{
/** @todo unimplemented function */
}

void Gepard::rotate(float angle)
{
/** @todo unimplemented function */
}

void Gepard::translate(float x, float y)
{
/** @todo unimplemented function */
}

void Gepard::transform(float a, float b, float c, float d, float e, float f)
{
/** @todo unimplemented function */
}

void Gepard::setTransform(float a, float b, float c, float d, float e, float f)
{
/** @todo unimplemented function */
}

void Gepard::clearRect(float x, float y, float w, float h)
{
/** @todo unimplemented function */
}

void Gepard::fillRect(float x, float y, float w, float h)
{
/** @todo unimplemented function */
}

void Gepard::strokeRect(float x, float y, float w, float h)
{
/** @todo unimplemented function */
}

void Gepard::drawImage(Image /*image*/, float dx, float dy)
{
/** @todo unimplemented function */
}

void Gepard::drawImage(Image /*image*/, float dx, float dy, float dw, float dh)
{
/** @todo unimplemented function */
}

void Gepard::drawImage(Image /*image*/, float sx, float sy, float sw, float sh,
    float dx, float dy, float dw, float dh)
{
/** @todo unimplemented function */
}

Image Gepard::createImageData(float sw, float sh)
{
/** @todo unimplemented function */
    ASSERT(0 && "Unimplemented function!");
    return Image();
}

Image Gepard::createImageData(Image /*imagedata*/)
{
/** @todo unimplemented function */
    ASSERT(0 && "Unimplemented function!");
    return Image();
}

Image Gepard::getImageData(double sx, double sy, double sw, double sh)
{
/** @todo unimplemented function */
    ASSERT(0 && "Unimplemented function!");
    return Image();
}

void Gepard::putImageData(Image /*imagedata*/, double dx, double dy)
{
/** @todo unimplemented function */
}

void Gepard::putImageData(Image /*imagedata*/, double dx, double dy, double dirtyX, double dirtyY,
    double dirtyWidth, double dirtyHeight)
{
/** @todo unimplemented function */
}

} // namespace gepard
