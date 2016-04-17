/* Copyright (C) 2015, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_TYPES_H
#define GEPARD_TYPES_H

#include "gepard-defs.h"
#include <iostream>
#include <math.h>

namespace gepard {

// TODO: #ifdef double-precision:
typedef double Float;
// TODO: use standard:
constexpr double precisionOfFloat = 1000 * 1000 * 1000;
//#else
//typedef float Float;
//constexpr float precisionOfFloat = 100 * 1000;
//#endif

inline Float fixPrecision(Float f) { return floor(f * precisionOfFloat) / precisionOfFloat; }

static const Float piFloat = 2.0 * asin(1.0);

template<class T>
static inline const T& min(const T& lhs, const T& rhs) { return !(rhs < lhs) ? lhs : rhs; }

template<class T>
static inline const T& max(const T& lhs, const T& rhs) { return (lhs < rhs) ? rhs : lhs; }

template<class T>
static inline const T& clamp(const T& value, const T& min, const T& max)
{
    return (value < min) ? min : ((value > max) ? max : value);
}

/* Region */

#define REGION_BLOCK_SIZE (2048 - (int)sizeof(void*))

class Region {
public:
    Region()
    {
        _first = new RegionElement();
        _last = _first;
        _fill = 0;
    }
    ~Region()
    {
        while (_first) {
            _last = _first;
            _first = _first->next;
            delete _last;
        }
    }

    void* alloc(int size);

private:
    struct RegionElement {
        RegionElement() : next(nullptr) {}

        RegionElement* next;
        uint8_t value[REGION_BLOCK_SIZE];
    };

    RegionElement* _first;
    RegionElement* _last;
    int _fill;
};

/* FloatPoint */

struct FloatPoint {
    FloatPoint() : x(0), y(0) {}
    FloatPoint(Float x, Float y) : x(x), y(y) {}

    Float lengthSquared() const { return x * x + y * y; }
    Float dot(const FloatPoint& p) const { return x * p.x + y * p.y; }

    Float x;
    Float y;
};

inline std::ostream& operator<<(std::ostream& os, const FloatPoint& p)
{
    return os << p.x << "," << p.y;
}

inline bool operator==(const FloatPoint& a, const FloatPoint& b)
{
    return a.x == b.x && a.y == b.y;
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

inline FloatPoint operator/(const FloatPoint& fp, const Float& f)
{
    ASSERT(f);
    return FloatPoint(fp.x / f, fp.y / f);
}

inline FloatPoint operator*(const FloatPoint& fp, const FloatPoint& de)
{
    return FloatPoint(fp.x * de.x, fp.y * de.y);
}

/* IntPoint */

struct IntPoint {
    IntPoint() : x(0), y(0) {}
    IntPoint(int x, int y) : x(x), y(y) {}

    int x;
    int y;
};

inline std::ostream& operator<<(std::ostream& os, const IntPoint& p)
{
    return os << p.x << "," << p.y;
}

inline bool operator==(const IntPoint& a, const IntPoint& b)
{
    return a.x == b.x && a.y == b.y;
}

/* BoundingBox */

struct BoundingBox {
    BoundingBox()
        : minX(INFINITY)
        , minY(INFINITY)
        , maxX(-INFINITY)
        , maxY(-INFINITY)
    {}

    void stretchX(Float x)
    {
        if (x < minX) {
            minX = x;
        }
        if (x > maxX) {
            maxX = x;
        }
    }
    void stretchY(Float y)
    {
        if (y < minY) {
            minY = y;
        }
        if (y > maxY) {
            maxY = y;
        }
    }
    void stretch(FloatPoint p)
    {
        stretchX(p.x);
        stretchY(p.y);
    }

    Float minX, minY, maxX, maxY;
};

inline std::ostream& operator<<(std::ostream& os, const BoundingBox& bb)
{
    return os << bb.minX << "," << bb.minY << "," << bb.maxX << "," << bb.maxY;
}

// Color

struct Color {
    Color() : r(0), g(0), b(0), a(0) {}
    Color(const int red, const int green, const int blue, const int alpha)
        : r(clamp(red, 0, 255))
        , g(clamp(green, 0, 255))
        , b(clamp(blue, 0, 255))
        , a(clamp(alpha, 0, 1))
    {}

    int r;
    int g;
    int b;
    int a;
};

} // namespace gepard

#endif // GEPARD_TYPES_H
