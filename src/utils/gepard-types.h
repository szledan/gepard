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

#ifndef GEPARD_TYPES_H
#define GEPARD_TYPES_H

#include "gepard-defs.h"
#include <iostream>
#include <math.h>

namespace gepard {

/* Float */

// TODO(szledan): use 'guards' for choosing precision of Float type
// #ifdef double-precision:
typedef double Float;
const double precisionOfFloat = 1000 * 1000 * 1000;
// #else
//   typedef float Float;
//   const float precisionOfFloat = 100 * 1000;
// #endif

inline Float fixPrecision(Float f) { return floor(f * precisionOfFloat) / precisionOfFloat; }

static const Float piFloat = 2.0 * asin(1.0);

/* Basic math functions */

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

/*!
 * \brief REGION_BLOCK_SIZE
 * REGION_BLOCK_SIZE = 2048 bytes minus size of a pointer
 *
 * \internal
 */
#define REGION_BLOCK_SIZE (2048 - (int)sizeof(void*))

/*!
 * \brief The Region class
 * \tparam BLOCK_SIZE  defines the region size; the default size is the
 * REGION_BLOCK_SIZE = 2048 bytes minus size of a pointer.
 *
 * This is a simple class for memory allocation.  It doesn't have free() or
 * realloc(), only alloc() for allocation.  It's used to allocate lots of
 * regions with variating sizes, but which are usually small, and are kept
 * until the whole Region is freed.
 *
 * The Region model determines free space in blocks (which is less than 2 KiB
 * by default).
 *
 * \internal
 */
template<const uint32_t BLOCK_SIZE = REGION_BLOCK_SIZE>
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

    /*!
     * \brief Region::alloc
     *
     * \param size  size of required memory in bytes
     * \return  pointer to allocated memory or nullptr if allocation failed.
     *
     * \internal
     */
    void* alloc(uint32_t size)
    {
        if (size <= BLOCK_SIZE) {

            if (_fill + size > BLOCK_SIZE) {
                _last->next = new RegionElement();
                _last = _last->next;
                _fill = 0;
            }

            void* ptr = _last->value + _fill;
            _fill += size;

            return ptr;
        }

        return nullptr;
    }

private:
    struct RegionElement {
        RegionElement() : next(nullptr) {}

        RegionElement* next;
        uint8_t value[BLOCK_SIZE];
    };

    RegionElement* _first;
    RegionElement* _last;
    uint32_t _fill;
};

/* FloatPoint */

/*!
 * \brief The FloatPoint struct
 *
 * This is a type for a 2D point.
 *
 * \internal
 */
struct FloatPoint {
    FloatPoint() : x(0.0), y(0.0) {}
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

/* BoundingBox */

/*!
 * \brief The BoundingBox struct
 *
 * The BoundignBox struct describes a rectangle with four Float values.
 * Each value determines either minimum or maximum values from
 * the given points.
 *
 * \internal
 */
struct BoundingBox {
    BoundingBox()
        : minX(INFINITY)
        , minY(INFINITY)
        , maxX(-INFINITY)
        , maxY(-INFINITY)
    {}

    void stretchX(const Float x);
    void stretchY(const Float y);
    void stretch(const FloatPoint p);

    Float minX, minY, maxX, maxY;
};

inline std::ostream& operator<<(std::ostream& os, const BoundingBox& bb)
{
    return os << bb.minX << "," << bb.minY << "," << bb.maxX << "," << bb.maxY;
}

/* Color */

/*!
 * \brief The Color struct
 *
 * Describes a simple RGBA color chanel struct
 * where each chanel is an unsigned byte (0-255).
 *
 * \internal
 */
struct Color {
    Color() : r(0), g(0), b(0), a(0) {}
    Color(const int red, const int green, const int blue, const int alpha)
        : r(clamp(red, 0, 255))
        , g(clamp(green, 0, 255))
        , b(clamp(blue, 0, 255))
        , a(clamp(alpha, 0, 1))
    {
    }

    uint8_t r; /*!< red chanel */
    uint8_t g; /*!< green chanel */
    uint8_t b; /*!< blue chanel */
    uint8_t a; /*!< alpha chanel */
};

} // namespace gepard

#endif // GEPARD_TYPES_H
