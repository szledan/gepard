/* Copyright (C) 2015-2016, 2018, Gepard Graphics
 * Copyright (C) 2015, 2018, Szilard Ledan <szledan@gmail.com>
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

#include "gepard-color.h"

#include "gepard-float.h"
#include "gepard-logging.h"

namespace gepard {

const Color Color::BLACK(0.0f, 0.0f, 0.0f, 1.0f);
const Color Color::WHITE(1.0f, 1.0f, 1.0f, 1.0f);

Color::Color()
    : Vec4()
{}

Color::Color(const Float red, const Float green, const Float blue, const Float alpha)
    : Vec4(clamp(red, Float(0.0f), Float(1.0f)),
           clamp(green, Float(0.0f), Float(1.0f)),
           clamp(blue, Float(0.0f), Float(1.0f)),
           clamp(alpha, Float(0.0f), Float(1.0f)))
{}

Color::Color(const std::string& color)
    : Color(Float(0.0f), Float(0.0f), Float(0.0f), Float(1.0f))
{
    const size_t length = color.length();
    int n;

    // Convert string hex to unsgined int.
    std::stringstream ss;
    ss << std::hex << color.substr(1);
    ss >> n;
    GD_LOG(DEBUG) << "Convert '" << color << "' string to hex number: " << std::hex << n;

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
    GD_LOG(DEBUG) << "Color is: " << r << ", " << g << ", " << b << ", " << a;
}

Color::Color(const Color &color)
    : Color(color.r, color.g, color.b, color.a)
{}

Color Color::fromRawDataABGR(const uint32_t raw)
{
    const Float red = Float(raw & 0x000000ff) / 255.0f;
    const Float green = Float((raw & 0x00ff00) >> 8) / 255.0f;
    const Float blue = Float((raw & 0x00ff0000) >> 16) / 255.0f;
    const Float alpha = Float((raw & 0xff000000) >> 24) / 255.0f;
    return Color(red, green, blue, alpha);
}

uint32_t Color::toRawDataABGR(const Color& color)
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

} // namespace gepard
