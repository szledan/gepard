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

#include "gepard-float-point.h"
#include "gepard-float.h"
#include <cmath>
#include <limits>

namespace gepard {

FloatPoint::FloatPoint()
    : x(0.0)
    , y(0.0)
{}

FloatPoint::FloatPoint(const Float x_, const Float y_)
    : x(x_)
    , y(y_)
{}

const Float FloatPoint::lengthSquared() const
{
    return x * x + y * y;
}

const Float FloatPoint::length() const
{
    return std::sqrt(lengthSquared());
}

const Float FloatPoint::dot(const FloatPoint& p) const
{
    return x * p.x + y * p.y;
}

const Float FloatPoint::cross(const FloatPoint& p) const
{
    return x * p.y - y * p.x;
}

const FloatPoint FloatPoint::normal() const
{
    return FloatPoint(y, -x);
}

const bool FloatPoint::isZero() const
{
    return (std::fabs(x) < std::numeric_limits<Float>::epsilon())
            && (std::fabs(y) < std::numeric_limits<Float>::epsilon());
}

void FloatPoint::set(const Float newX, const Float newY)
{
    x = newX;
    y = newY;
}

void FloatPoint::scale(const Float scaleX, const Float scaleY)
{
    x *= scaleX;
    y *= scaleY;
}

} // namespace gepard
