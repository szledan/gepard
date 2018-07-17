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
#include <cmath>
#include <limits>
#include <string>
#include <vector>

namespace gepard {

class Surface;

/* Float */

// TODO(szledan): use 'guards' for choosing precision of Float type
// #ifdef double-precision:
typedef double Float;
const double precisionOfFloat = 0x1L << 50;
// #else
//   typedef float Float;
//   const float precisionOfFloat = 0x1 << 20;
// #endif

inline const Float strToFloat(const std::string& str) { return std::stod(str); }
inline Float fixPrecision(Float f) { return std::floor(f * precisionOfFloat) / precisionOfFloat; }

static const Float piFloat = 2.0 * std::asin(1.0);

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
 * \brief GD_REGION_BLOCK_SIZE
 * GD_REGION_BLOCK_SIZE = 2048 bytes minus size of a pointer
 *
 * \internal
 */
#define GD_REGION_BLOCK_SIZE (2048 - (int)sizeof(void*))

/*!
 * \brief The Region class
 * \tparam BLOCK_SIZE  defines the region size; the default size is the
 * GD_REGION_BLOCK_SIZE = 2048 bytes minus size of a pointer.
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
template<const uint32_t BLOCK_SIZE = GD_REGION_BLOCK_SIZE>
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
    FloatPoint(Float x_, Float y_) : x(x_), y(y_) {}

    const Float lengthSquared() const { return x * x + y * y; }
    const Float length() const { return std::sqrt(lengthSquared()); }
    const Float dot(const FloatPoint& p) const { return x * p.x + y * p.y; }
    const Float cross(const FloatPoint& p) const { return x * p.y - y * p.x; }
    const FloatPoint normal() const { return FloatPoint(y, -x); }

    const bool isZero() const { return (std::fabs(x) < std::numeric_limits<Float>::epsilon()) && (std::fabs(y) < std::numeric_limits<Float>::epsilon()); }

    void set(const Float newX, const Float newY)
    {
        x = newX;
        y = newY;
    }
    void scale(const Float scaleX, const Float scaleY)
    {
        x *= scaleX;
        y *= scaleY;
    }

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
    void stretch(const FloatPoint& p);

    Float minX, minY, maxX, maxY;
};

inline std::ostream& operator<<(std::ostream& os, const BoundingBox& bb)
{
    return os << bb.minX << "," << bb.minY << "," << bb.maxX << "," << bb.maxY;
}

/* Vec4 */

/*!
 * \brief The Vec4 struct
 *
 * \internal
 */
struct Vec4 {
    Vec4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
    Vec4(Float x_, Float y_, Float z_, Float w_) : x(x_), y(y_), z(z_), w(w_) {}
    Vec4(const Vec4& vec4) : x(vec4.x), y(vec4.y), z(vec4.z), w(vec4.w) {}

    Float& operator[](std::size_t idx);

    /*!
     * \brief The x/r/s component union
     */
    union {
        Float x; /*!< the x coordinate */
        Float r; /*!< red channel */
        Float s; /*!< the s parameter */
    };
    /*!
     * \brief The y/g/t component union
     */
    union {
        Float y; /*!< the y coordinate */
        Float g; /*!< green channel */
        Float t; /*!< the t parameter */
    };
    /*!
     * \brief The z/b/p component union
     */
    union {
        Float z; /*!< the z coordinate */
        Float b; /*!< blue channel */
        Float p; /*!< the p parameter */
    };
    /*!
     * \brief The w/a/q component union
     */
    union {
        Float w; /*!< the w coordinate */
        Float a; /*!< alpha channel */
        Float q; /*!< the q parameter */
    };
};

/* Color */

/*!
 * \brief The Color struct
 *
 * Describes a simple RGBA color chanel struct
 * where each chanel is a gepard::Float [0.0, 1.0].
 *
 * \todo reference: https://www.w3.org/TR/css-color-3/
 * \internal
 */
struct Color : public Vec4 {
    Color() : Vec4() {}
    Color(const Float red, const Float green, const Float blue, const Float alpha);
    Color(const std::string& color);
    Color(const Color& color) : Color(color.r, color.g, color.b, color.a) {}

    /*!
     * \brief fromRawDataABGR
     * \param raw
     * \return
     *
     * \internal
     * \todo doc missing
     * \todo unit-test missing
     */
    static Color fromRawDataABGR(uint32_t raw);
    /*!
     * \brief toRawDataABGR
     * \param color
     * \return
     *
     * \internal
     * \todo doc missing
     * \todo unit-test missing
     */
    static uint32_t toRawDataABGR(Color color);

    /*!
     * \brief operator *=
     * \param rhs
     * \return
     *
     * \internal
     * \todo doc missing
     * \todo unit-test missing
     */
    Color& operator*=(const Float& rhs);

    static const Color BLACK;
    static const Color WHITE;
};

/*!
 * \internal
 * \todo unit-test missing
 */
inline Color operator+(const Color& lhs, const Color& rhs)
{
    return Color(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a);
}

typedef enum LineCapTypes {
    ButtCap,
    RoundCap,
    SquareCap,
} LineCapType;

LineCapTypes strToLineCap(const std::string& value);

typedef enum LineJoinTypes {
    RoundJoin,
    BevelJoin,
    MiterJoin,
} LineJoinType;

LineJoinTypes strToLineJoin(const std::string& value);

/* Transform */

struct Transform {
    Float data[6] = { 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 };

    Transform (const Float a = 1.0, const Float b = 0.0, const Float c = 0.0, const Float d = 1.0, const Float e = 0.0, const Float f = 0.0);

    Transform& rotate(float angle);
    Transform& scale(float sx, float sy);
    Transform& translate(const Float x, const Float y);

    const FloatPoint apply(const FloatPoint p) const;

    Transform& multiply(const Transform& transform);
    void operator*=(const Transform& transform);

    const Transform inverse() const;
};

Transform operator*(const Transform& lhs , const Transform& rhs);

/* GepardState */

/*!
 * \brief The GepardState struct
 *
 * Describes the Drawing state.
 * -- <a href="https://www.w3.org/TR/2dcontext/#the-canvas-state">[W3C-2DContext]</a>
 *
 * \internal
 */
struct GepardState {
    Color fillColor = Color(Color::BLACK);

    Color strokeColor = Color(Color::BLACK);
    //! \brief CanvasDrawingStyles
    Float lineWitdh = 1.0;
    LineJoinTypes lineJoinMode = BevelJoin;
    LineCapTypes lineCapMode = ButtCap;
    Float miterLimit = 10;
    Transform transform;
};

} // namespace gepard

#endif // GEPARD_TYPES_H
