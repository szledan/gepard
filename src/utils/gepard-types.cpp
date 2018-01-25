/* Copyright (C) 2015-2016, Gepard Graphics
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
 */

#include "gepard-types.h"

namespace gepard {

/* BoundingBox */

void BoundingBox::stretchX(const Float x)
{
    if (x < minX) {
        minX = x;
    }
    if (x > maxX) {
        maxX = x;
    }
}
void BoundingBox::stretchY(const Float y)
{
    if (y < minY) {
        minY = y;
    }
    if (y > maxY) {
        maxY = y;
    }
}
void BoundingBox::stretch(const FloatPoint& p)
{
    stretchX(p.x);
    stretchY(p.y);
}

/* Vec4 */

Float& Vec4::operator[](std::size_t idx)
{
    switch (idx) {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    case 3: return w;
    default:
        GD_CRASH("Index out of bound!");
    }
}

/* Color */

const Color Color::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::WHITE(1.0f, 1.0f, 1.0f, 1.0f);

Color Color::fromRawDataABGR(uint32_t raw)
{
    const Float red = Float(raw & 0x000000ff) / 255.0f;
    const Float green = Float((raw & 0x00ff00) >> 8) / 255.0f;
    const Float blue = Float((raw & 0x00ff0000) >> 16) / 255.0f;
    const Float alpha = Float((raw & 0xff000000) >> 24) / 255.0f;
    return Color(red, green, blue, alpha);
}

uint32_t Color::toRawDataABGR(Color color)
{
    const uint32_t alphaByte = (uint32_t(color.a * 255.0f) & 0xff) << 24;
    const uint32_t blueByte = (uint32_t(color.b * 255.0f) & 0xff) << 16;
    const uint32_t greenByte = (uint32_t(color.g * 255.0f) & 0xff) << 8;
    const uint32_t redByte = (uint32_t(color.r * 255.0f) & 0xff);
    return alphaByte | blueByte | greenByte | redByte;
}

Color& Color::operator*=(const Float& rhs)
{
    this->r = clamp(this->r * rhs, Float(0.0f), Float(1.0f));
    this->g = clamp(this->g * rhs, Float(0.0f), Float(1.0f));
    this->b = clamp(this->b * rhs, Float(0.0f), Float(1.0f));
    this->a = clamp(this->a * rhs, Float(0.0f), Float(1.0f));

    return *this;
}

LineCapTypes strToLineCap(const std::string& value)
{
    if (value == "round") return RoundCap;
    if (value == "square") return SquareCap;
    return ButtCap;
}

LineJoinTypes strToLineJoin(const std::string& value)
{
    if (value == "round") return RoundJoin;
    if (value == "bevel") return BevelJoin;
    return MiterJoin;
}

} // namespace gepard
