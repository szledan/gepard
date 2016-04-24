/* Copyright (C) 2015-2016, Gepard Graphics
 * Copyright (C) 2015-2016, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
 * Copyright (C) 2015-2016, Szilard Ledan <szledan@gmail.com>
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

/**
 * @brief Gepard::cloeePath
 *
 * @todo unimplemented function
 *
 */
void Gepard::cloeePath()
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::moveTo
 * @param x
 * @param y
 *
 * @todo unimplemented function
 *
 */
void Gepard::moveTo(float x, float y)
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::lineTo
 * @param x
 * @param y
 *
 * @todo unimplemented function
 *
 */
void Gepard::lineTo(float x, float y)
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::quadraticCurveTo
 * @param cpx
 * @param cpy
 * @param x
 * @param y
 *
 * @todo unimplemented function
 *
 */
void Gepard::quadraticCurveTo(float cpx, float cpy, float x, float y)
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::bezierCurveTo
 * @param cp1x
 * @param cp1y
 * @param cp2x
 * @param cp2y
 * @param x
 * @param y
 *
 * @todo unimplemented function
 *
 */
void Gepard::bezierCurveTo(float cp1x, float cp1y, float cp2x, float cp2y, float x, float y)
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::arcTo
 * @param x1
 * @param y1
 * @param x2
 * @param y2
 * @param radius
 *
 * @todo unimplemented function
 *
 */
void Gepard::arcTo(float x1, float y1, float x2, float y2, float radius)
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::rect
 * @param x
 * @param y
 * @param w
 * @param h
 *
 * @todo unimplemented function
 *
 */
void Gepard::rect(float x, float y, float w, float h)
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::arc
 * @param x
 * @param y
 * @param radius
 * @param startAngle
 * @param endAngle
 * @param counterclockwise
 *
 * @todo unimplemented function
 *
 */
void Gepard::arc(float x, float y, float radius, float startAngle, float endAngle, bool counterclockwise)
{
    NOT_IMPLEMENTED();
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

/**
 * @brief Gepard::beginPath
 *
 * @todo unimplemented function
 *
 */
void Gepard::beginPath()
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::fill
 *
 * @todo unimplemented function
 *
 */
void Gepard::fill()
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::stroke
 *
 * @todo unimplemented function
 *
 */
void Gepard::stroke()
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::drawFocusIfNeeded
 *
 * @todo unimplemented function
 *
 */
void Gepard::drawFocusIfNeeded(/*Element element*/)
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::clip
 *
 * @todo unimplemented function
 *
 */
void Gepard::clip()
{
    NOT_IMPLEMENTED();
}

/**
 * @brief Gepard::isPointInPath
 * @param x
 * @param y
 * @return
 *
 * @todo unimplemented function
 *
 */
bool Gepard::isPointInPath(float x, float y)
{
    NOT_IMPLEMENTED();
    return false;
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
    NOT_IMPLEMENTED();
    return Image();
}

Image Gepard::createImageData(Image /*imagedata*/)
{
/** @todo unimplemented function */
    NOT_IMPLEMENTED();
    return Image();
}

Image Gepard::getImageData(double sx, double sy, double sw, double sh)
{
/** @todo unimplemented function */
    NOT_IMPLEMENTED();
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
