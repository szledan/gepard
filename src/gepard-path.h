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
    Float _x;
    Float _y;
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

    PathElement* _next;
    PathElementType _type;
};

struct MoveToElement : public PathElement {
    MoveToElement(FloatPoint to)
        : PathElement(PathElementTypes::MoveTo)
        , _to(to)
    {
    };

    FloatPoint _to;
};

class PathData {
public:
private:
};

class Path {
public:
private:
    PathData m_pathData;
};

class Tessalator {
public:
private:
};

} // namespace gepard

#endif // gepard_path_h
