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

#ifndef gepard_types_h
#define gepard_types_h

#include "config.h"

namespace gepard {

// FIXME: #ifdef double-precision:
typedef double Float;
// FIXME: use standard:
constexpr double precisionOfFloat = 1000 * 1000 * 1000;
//#else
//typedef float Float;
//constexpr float precisionOfFloat = 100 * 1000;
//#endif

inline Float fixPrecision(Float f) { return ceil(f * precisionOfFloat) / precisionOfFloat; }

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
            _first = _first->_next;
            delete _last;
        }
    }

    void* alloc(int size);

private:
    struct RegionElement {
        RegionElement* _next;
        uint8_t _value[REGION_BLOCK_SIZE];

        RegionElement()
            : _next(nullptr)
        {
        }
    };

    RegionElement* _first;
    RegionElement* _last;
    int _fill;
};

/* FloatPoint */

struct FloatPoint {
    FloatPoint() : _x(0), _y(0) {}
    FloatPoint(Float x, Float y) : _x(x), _y(y) {}

    Float _x;
    Float _y;
};

inline std::ostream& operator<<(std::ostream& os, const FloatPoint& p)
{
    return os << p._x << "," << p._y;
}

inline bool operator==(const FloatPoint& a, const FloatPoint& b)
{
    return a._x == b._x && a._y == b._y;
}

inline bool operator<(const FloatPoint& lhs, const FloatPoint& rhs)
{
    return (lhs._y < rhs._y) || (lhs._y == rhs._y && lhs._x < rhs._x);
}

inline bool operator>(const FloatPoint& lhs, const FloatPoint& rhs) { return rhs < lhs; }
inline bool operator<=(const FloatPoint& lhs, const FloatPoint& rhs) { return !(lhs > rhs); }

inline FloatPoint operator+(const FloatPoint& lhs, const FloatPoint& rhs)
{
    return FloatPoint(lhs._x + rhs._x, lhs._y + rhs._y);
}

inline FloatPoint operator/(const FloatPoint& fp, const Float& f)
{
    ASSERT(f);
    return FloatPoint(fp._x / f, fp._y / f);
}

/* IntPoint */

struct IntPoint {
    IntPoint() : _x(0), _y(0) {}
    IntPoint(int x, int y) : _x(x), _y(y) {}

    int _x;
    int _y;
};

inline std::ostream& operator<<(std::ostream& os, const IntPoint& p)
{
    return os << p._x << "," << p._y;
}

inline bool operator==(const IntPoint& a, const IntPoint& b)
{
    return a._x == b._x && a._y == b._y;
}

/* BoundingBox */

struct BoundingBox {
    BoundingBox()
        : _minX(INFINITY)
        , _minY(INFINITY)
        , _maxX(-INFINITY)
        , _maxY(-INFINITY)
    {
    }

    void stretchX(Float x)
    {
        if (x < _minX)
            _minX = x;
        if (x > _maxX)
            _maxX = x;
    }
    void stretchY(Float y)
    {
        if (y < _minY)
            _minY = y;
        if (y > _maxY)
            _maxY = y;
    }
    void stretch(FloatPoint p)
    {
        stretchX(p._x);
        stretchY(p._y);
    }

    Float _minX, _minY, _maxX, _maxY;
};

inline std::ostream& operator<<(std::ostream& os, const BoundingBox& bb)
{
    return os << bb._minX << "," << bb._minY << "," << bb._maxX << "," << bb._maxY;
}

} // namespace gepard

#endif // gepard_types_h
