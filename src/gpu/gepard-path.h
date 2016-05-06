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

#ifndef GEPARD_PATH_H
#define GEPARD_PATH_H

#include "gepard-types.h"
#include <iostream>

namespace gepard {

typedef enum PathElementTypes {
    Undefined = 0,
    MoveTo,
    LineTo,
    QuadraticCurve,
    BezierCurve,
    Arc,
    CloseSubpath,
} PathElementType;

struct PathElement {
    explicit PathElement() : next(nullptr), type(PathElementTypes::Undefined) {}
    explicit PathElement(PathElementType type) : next(nullptr), type(type) {}
    explicit PathElement(PathElementType type, FloatPoint to)
        : next(nullptr)
        , type(type)
        , to(to)
    {}

    bool isMoveTo() const { return this->type == PathElementTypes::MoveTo; }
    bool isCloseSubpath() const { return this->type == PathElementTypes::CloseSubpath; }
    virtual std::ostream& output(std::ostream& os) const
    {
        return os << this->to;
    }

    PathElement* next;
    PathElementType type;
    FloatPoint to;
};

inline std::ostream& operator<<(std::ostream& os, const PathElement& ps)
{
    return ps.output(os);
}

struct MoveToElement : public PathElement {
    explicit MoveToElement(FloatPoint to) : PathElement(PathElementTypes::MoveTo, to) {}

    std::ostream& output(std::ostream& os) const { return PathElement::output(os << "M"); }
};

struct LineToElement : public PathElement {
    explicit LineToElement(FloatPoint to) : PathElement(PathElementTypes::LineTo, to) {}

    std::ostream& output(std::ostream& os) const { return PathElement::output(os << "L"); }
};

struct CloseSubpathElement : public PathElement {
    explicit CloseSubpathElement(FloatPoint to) : PathElement(PathElementTypes::CloseSubpath, to) {}

    std::ostream& output(std::ostream& os) const { return PathElement::output(os << "Z"); }
};

struct QuadraticCurveToElement : public PathElement {
    explicit QuadraticCurveToElement(FloatPoint control, FloatPoint to)
        : PathElement(PathElementTypes::QuadraticCurve, to)
        , control(control)
    {}

    std::ostream& output(std::ostream& os) const
    {
        return PathElement::output(os << "Q" << this->control << " ");
    }

    FloatPoint control;
};

struct BezierCurveToElement : public PathElement {
    explicit BezierCurveToElement(FloatPoint control1, FloatPoint control2, FloatPoint to)
        : PathElement(PathElementTypes::BezierCurve, to)
        , control1(control1)
        , control2(control2)
    {}

    std::ostream& output(std::ostream& os) const
    {
        return PathElement::output(os << "C" << this->control1 << " " << this->control2 << " ");
    }

    FloatPoint control1;
    FloatPoint control2;
};

struct ArcElement : public PathElement {
    explicit ArcElement(FloatPoint center, FloatPoint radius, Float startAngle, Float endAngle, bool antiClockwise = true)
        : PathElement(PathElementTypes::Arc, FloatPoint(center.x + cos(endAngle) * radius.x, center.y + sin(endAngle) * radius.y))
        , center(center)
        , radius(radius)
        , startAngle(startAngle)
        , endAngle(endAngle)
        , antiClockwise(antiClockwise)
    {
        ASSERT(radius.x >= 0 && radius.y >= 0);
    }

    std::ostream& output(std::ostream& os) const
    {
        // TODO(szledan): generate SVG representation
        os << "A";
        return PathElement::output(os);
    }

    FloatPoint center;
    FloatPoint radius;
    Float startAngle;
    Float endAngle;
    bool antiClockwise;
};

struct PathData {
    explicit PathData()
        : _firstElement(nullptr)
        , _lastElement(nullptr)
        , _lastMoveToElement(nullptr)
    {}

    void addMoveToElement(FloatPoint);
    void addLineToElement(FloatPoint);
    void addQuadaraticCurveToElement(FloatPoint, FloatPoint);
    void addBezierCurveToElement(FloatPoint, FloatPoint, FloatPoint);
    void addArcElement(FloatPoint, FloatPoint, Float, Float, bool = true);
    void addArcToElement(const FloatPoint&, const FloatPoint&, const Float&);
    void addCloseSubpathElement();
#ifndef NDEBUG
    void dump();
#endif

    PathElement* firstElement() const { return _firstElement; }
    PathElement* lastElement() const { return _lastElement; }

private:
    Region<> _region;
    PathElement* _firstElement;
    PathElement* _lastElement;
    PathElement* _lastMoveToElement;
};

/* Path */

class Path {
public:
    explicit Path()
    {}

    PathData& pathData() { return _pathData; }
    void fillPath();

private:
    PathData _pathData;
};

} // namespace gepard

#endif // GEPARD_PATH_H
