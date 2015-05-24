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
#include "gepard-types.h"

namespace gepard {


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
    PathElement() : _next(nullptr), _type(PathElementTypes::Undefined) {}
    PathElement(PathElementType type) : _next(nullptr), _type(type) {}
    PathElement(PathElementType type, FloatPoint to)
        : _next(nullptr)
        , _type(type)
        , _to(to)
    {}

    bool isMoveTo() const { return _type == PathElementTypes::MoveTo; }
    virtual std::ostream& output(std::ostream& os) const = 0;

    PathElement* _next;
    PathElementType _type;
    FloatPoint _to;
};

struct MoveToElement : public PathElement {
    MoveToElement(FloatPoint to) : PathElement(PathElementTypes::MoveTo, to) {}

    virtual std::ostream& output(std::ostream& os) const
    {
        return os << "M(" << _to << ")";
    }
};

struct LineToElement : public PathElement {
    LineToElement(FloatPoint to) : PathElement(PathElementTypes::LineTo, to) {}

    virtual std::ostream& output(std::ostream& os) const
    {
        return os << "L(" << _to << ")";
    }
};

struct CloseSubpathElement : public PathElement {
    CloseSubpathElement(FloatPoint to) : PathElement(PathElementTypes::CloseSubpath, to) {}

    virtual std::ostream& output(std::ostream& os) const
    {
        return os << "Z(" << _to << ")";
    }
};

struct PathData {
    PathData()
        : _firstElement(nullptr)
        , _lastElement(nullptr)
        , _lastMoveToElement(nullptr)
    {}

    void addMoveToElement(FloatPoint);
    void addLineToElement(FloatPoint);
    void addCloseSubpath();
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
