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

Color::Color(const Float red, const Float green, const Float blue, const Float alpha)
    : Vec4(clamp(red, Float(0.0f), Float(1.0f)), clamp(green, Float(0.0f), Float(1.0f)), clamp(blue, Float(0.0f), Float(1.0f)), clamp(alpha, Float(0.0f), Float(1.0f)))
{
}

Color::Color(const std::string& color)
    : Color(Float(0.0f), Float(0.0f), Float(0.0f), Float(1.0f))
{
    const size_t length = color.length();
    int n;

    // Convert string hex to unsgined int.
    std::stringstream ss;
    ss << std::hex << color.substr(1);
    ss >> n;
    GD_LOG3("Convert '" << color << "' string to hex number: " << std::hex << n);

    a = 1.0;
    if (length == 7) {
        r = ((n & 0xff0000) >> 16) / 255.0;
        g = ((n & 0x00ff00) >> 8) / 255.0;
        b = (n & 0x0000ff) / 255.0;
    } else if (length == 4) {
        r = ((n & 0xf00) >> 8) / 15.0;
        g = ((n & 0x0f0) >> 4) / 15.0;
        b =  (n & 0x00f) / 15.0;
    }
    GD_LOG3("Color is: " << r << ", " << g << ", " << b << ", " << a << ".");
}

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

/* Transform */

Transform::Transform(const Float a, const Float b, const Float c, const Float d, const Float e, const Float f)
{
    data[0] = a;
    data[1] = b;
    data[2] = c;
    data[3] = d;
    data[4] = e;
    data[5] = f;
}

Transform&Transform::rotate(const Float angle)
{
    const Float cosAngle = cos(angle);
    const Float sinAngle = sin(angle);
    Transform matrix(cosAngle, sinAngle, -sinAngle, cosAngle, 0.0, 0.0);
    multiply(matrix);
    return *this;
}

Transform&Transform::scale(const Float sx, const Float sy)
{
    Transform matrix(sx, 0.0, 0.0, sy, 0.0, 0.0);
    multiply(matrix);
    return *this;
}

Transform&Transform::translate(const Float x, const Float y)
{
    Transform matrix(1.0, 0.0, 0.0, 1.0, x, y);
    multiply(matrix);
    return *this;
}

const FloatPoint Transform::apply(const FloatPoint p) const
{
    const Float x = p.x * data[0] + p.y * data[2] + data[4];
    const Float y = p.x * data[1] + p.y * data[3] + data[5];
    return FloatPoint(x, y);
}

Transform&Transform::multiply(const Transform& transform)
{
    const Float a = data[0];
    const Float b = data[1];
    const Float c = data[2];
    const Float d = data[3];

    data[0] = a * transform.data[0] + c * transform.data[1];
    data[1] = b * transform.data[0] + d * transform.data[1];
    data[2] = a * transform.data[2] + c * transform.data[3];
    data[3] = b * transform.data[2] + d * transform.data[3];
    if (transform.data[4] || transform.data[5]) {
        data[4] += a * transform.data[4] + c * transform.data[5];
        data[5] += b * transform.data[4] + d * transform.data[5];
    }
    return *this;
}

void Transform::operator*=(const Transform& transform)
{
    multiply(transform);
}

const Transform Transform::inverse() const
{
    Transform result;
    Float determinant = data[0] * data[3] - data[1] * data[2];
    if (!determinant)
        return result;

    result.data[0] = data[3] / determinant;
    result.data[1] = -data[1] / determinant;
    result.data[2] = -data[2] / determinant;
    result.data[3] = data[0] / determinant;
    result.data[4] = (data[2] * data[5] - data[3] * data[4]) / determinant;
    result.data[5] = (data[1] * data[4] - data[0] * data[5]) / determinant;

    return result;
}

Transform operator*(const Transform& lhs, const Transform& rhs)
{
    Transform transform = lhs;
    transform.multiply(rhs);
    return transform;
}

} // namespace gepard
