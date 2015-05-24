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

#ifndef gepard_path_h
#define gepard_path_h

#include "config.h"

#include "gepard-surface.h"

namespace gepard {

typedef float Float;

struct FloatSize {
    Float _width;
    Float _heigth;

    Float squaredMagnitude() { return _width * _width + _heigth * _heigth; }
    Float magnitude() { return sqrt(squaredMagnitude()); }

    FloatSize& operator+=(const FloatSize& rhs)
    {
        this->_width += rhs._width;
        this->_heigth += rhs._heigth;
        return *this;
    }
    FloatSize& operator-=(const FloatSize& rhs)
    {
        this->_width -= rhs._width;
        this->_heigth -= rhs._heigth;
        return *this;
    }
    FloatSize& operator*=(const FloatSize& rhs)
    {
        this->_width *= rhs._width;
        this->_heigth *= rhs._heigth;
        return *this;
    }
    FloatSize& operator/=(const FloatSize& rhs)
    {
        ASSERT(rhs._width && rhs._heigth);

        this->_width /= rhs._width;
        this->_heigth /= rhs._heigth;
        return *this;
    }
    friend FloatSize operator+(FloatSize lhs, const FloatSize& rhs) { return lhs += rhs; }
    friend FloatSize operator-(FloatSize lhs, const FloatSize& rhs) { return lhs -= rhs; }
    friend FloatSize operator*(FloatSize lhs, const FloatSize& rhs) { return lhs *= rhs; }
    friend FloatSize operator/(FloatSize lhs, const FloatSize& rhs) { return lhs /= rhs; }
};

struct FloatPoint {
    FloatPoint() : _x(0), _y(0) {}
    FloatPoint(float x, float y) : _x(x), _y(y) {}

    Float _x;
    Float _y;
};

std::ostream& operator<<(std::ostream& os, const FloatPoint& fp);

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

    RegionElement *_first;
    RegionElement *_last;
    int _fill;
};

typedef enum PathElementTypes {
    Undefined = 0,
    MoveTo,
    LineTo,
    QuadraticCurve,
    CubicCurve,
    Arc,
    CloseSubpath,
} PathElementType;

struct PathElement {
    PathElement() : _next(nullptr), _type(PathElementTypes::Undefined) {};
    PathElement(PathElementType type) : _next(nullptr), _type(type) {};

    virtual std::ostream& output(std::ostream& os) const = 0;

    PathElement* _next;
    PathElementType _type;
};

struct MoveToElement : public PathElement {
    MoveToElement(FloatPoint to)
        : PathElement(PathElementTypes::MoveTo)
        , _to(to)
    {}

    virtual std::ostream& output(std::ostream& os) const
    {
        return os << "M(" << _to << ")";
    }

    FloatPoint _to;
};

struct PathData {
    PathData()
        : _firstElement(nullptr)
        , _lastElement(nullptr)
        , _lastMoveToElement(nullptr)
    {}

    void addMoveToElement(FloatPoint);
    void dump();

    Region _region;
    PathElement* _firstElement;
    PathElement* _lastElement;
    PathElement* _lastMoveToElement;
};

class Path {
public:
    Path(const GepardSurface* surface)
        : _surface(surface)
    {}

    PathData& pathData() { return _pathData; }
    void fillPath();

private:
    PathData _pathData;
    const GepardSurface* _surface;
};

class Tessalator {
public:
private:
};

} // namespace gepard

#endif // gepard_path_h
