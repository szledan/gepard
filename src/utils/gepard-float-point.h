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

#ifndef GEPARD_FLOAT_POINT_H
#define GEPARD_FLOAT_POINT_H

#include "gepard-defs.h"
#include "gepard-float.h"
#include <limits>
#include <ostream>

namespace gepard {

/*!
 * \brief The FloatPoint struct
 *
 * This is a type for a 2D point.
 *
 * \internal
 */
struct FloatPoint {
    FloatPoint();
    FloatPoint(const Float x_, const Float y_);

    const Float lengthSquared() const;
    const Float length() const;
    const Float dot(const FloatPoint& p) const;
    const Float cross(const FloatPoint& p) const;
    const FloatPoint normal() const;

    const bool isZero() const;

    void set(const Float newX, const Float newY);
    void scale(const Float scaleX, const Float scaleY);

    Float x;
    Float y;
};

inline std::ostream& operator<<(std::ostream& os, const FloatPoint& p)
{
    return os << p.x << "," << p.y;
}

inline bool operator==(const FloatPoint& a, const FloatPoint& b)
{
    return std::abs(a.x - b.x) < std::numeric_limits<float>::epsilon() && std::abs(a.y - b.y) < std::numeric_limits<float>::epsilon();
}

inline bool operator!=(const FloatPoint& a, const FloatPoint& b)
{
    return !(a == b);
}

inline bool operator<(const FloatPoint& lhs, const FloatPoint& rhs)
{
    return (lhs.y < rhs.y) || (lhs.y == rhs.y && lhs.x < rhs.x);
}

inline bool operator>(const FloatPoint& lhs, const FloatPoint& rhs)
{
    return rhs < lhs;
}

inline bool operator<=(const FloatPoint& lhs, const FloatPoint& rhs)
{
    return !(lhs > rhs);
}

inline FloatPoint operator+(const FloatPoint& lhs, const FloatPoint& rhs)
{
    return FloatPoint(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline FloatPoint operator-(const FloatPoint& lhs, const FloatPoint& rhs)
{
    return FloatPoint(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline FloatPoint operator-(const FloatPoint& fp)
{
    return FloatPoint(-fp.x, -fp.y);
}

inline FloatPoint operator/(const FloatPoint& fp, const Float& f)
{
    GD_ASSERT(f);
    return FloatPoint(fp.x / f, fp.y / f);
}

inline FloatPoint operator*(const Float& f, const FloatPoint& fp)
{
    return FloatPoint(f * fp.x, f * fp.y);
}

inline FloatPoint operator*(const FloatPoint& fp, const FloatPoint& de)
{
    return FloatPoint(fp.x * de.x, fp.y * de.y);
}

} // namespace gepard

#endif // GEPARD_FLOAT_POINT_H
