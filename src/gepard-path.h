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
#include <list>
#include <map>
#include <math.h>
#include <memory>

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
    PathElement() : _next(nullptr), _type(PathElementTypes::Undefined) {}
    PathElement(PathElementType type) : _next(nullptr), _type(type) {}
    PathElement(PathElementType type, FloatPoint to)
        : _next(nullptr)
        , _type(type)
        , _to(to)
    {
    }

    bool isMoveTo() const { return _type == PathElementTypes::MoveTo; }
    virtual std::ostream& output(std::ostream& os) const
    {
        return os << _to;
    }

    PathElement* _next;
    PathElementType _type;
    FloatPoint _to;
};

struct MoveToElement : public PathElement {
    MoveToElement(FloatPoint to) : PathElement(PathElementTypes::MoveTo, to) {}

    std::ostream& output(std::ostream& os) const { return PathElement::output(os << "M"); }
};

struct LineToElement : public PathElement {
    LineToElement(FloatPoint to) : PathElement(PathElementTypes::LineTo, to) {}

    std::ostream& output(std::ostream& os) const { return PathElement::output(os << "L"); }
};

struct CloseSubpathElement : public PathElement {
    CloseSubpathElement(FloatPoint to) : PathElement(PathElementTypes::CloseSubpath, to) {}

    std::ostream& output(std::ostream& os) const { return PathElement::output(os << "Z"); }
};

struct QuadraticCurveToElement : public PathElement {
    QuadraticCurveToElement(FloatPoint control, FloatPoint to)
        : PathElement(PathElementTypes::QuadraticCurve, to)
        , _control(control)
    {
    }

    std::ostream& output(std::ostream& os) const
    {
        return PathElement::output(os << "Q" << _control << " ");
    }

    FloatPoint _control;
};

struct BezierCurveToElement : public PathElement {
    BezierCurveToElement(FloatPoint control1, FloatPoint control2, FloatPoint to)
        : PathElement(PathElementTypes::BezierCurve, to)
        , _control1(control1)
        , _control2(control2)
    {
    }

    std::ostream& output(std::ostream& os) const
    {
        return PathElement::output(os << "C" << _control1 << " " << _control2 << " ");
    }

    FloatPoint _control1;
    FloatPoint _control2;
};

struct ArcElement : public PathElement {
    ArcElement(FloatPoint center, FloatPoint radius, Float startAngle, Float endAngle, bool antiClockwise = true)
        : PathElement(PathElementTypes::Arc, FloatPoint (cos(endAngle) * radius._x, sin(endAngle) * radius._y))
        , _center(center)
        , _radius(radius)
        , _startAngle(startAngle)
        , _endAngle(endAngle)
        , _antiClockwise(antiClockwise)
    {
    }

    std::ostream& output(std::ostream& os) const
    {
        // FIXME: generate SVG representation
        os << "A";
        return PathElement::output(os);
    }

    FloatPoint _center;
    FloatPoint _radius;
    Float _startAngle;
    Float _endAngle;
    bool _antiClockwise;
};

struct PathData {
    PathData()
        : _firstElement(nullptr)
        , _lastElement(nullptr)
        , _lastMoveToElement(nullptr)
    {
    }

    void addMoveToElement(FloatPoint);
    void addLineToElement(FloatPoint);
    void addQuadaraticCurveToElement(FloatPoint, FloatPoint);
    void addBezierCurveToElement(FloatPoint, FloatPoint, FloatPoint);
    void addArcElement(FloatPoint, FloatPoint, Float, Float, bool = true);
    void addCloseSubpath();
    void dump();

    Region _region;
    PathElement* _firstElement;
    PathElement* _lastElement;
    PathElement* _lastMoveToElement;
};

/* Path */

class Path {
public:
    Path(const GepardSurface* surface)
        : _surface(surface)
    {
    }

    PathData& pathData() { return _pathData; }
    void fillPath();

private:
    PathData _pathData;
    const GepardSurface* _surface;
};

/* Segment */

struct Segment {
    enum {
        Negative = -1,
        EqualOrNonExist = 0,
        Positive = 1,
    };

    Float topY() const { return _from._y; }
    Float bottomY() const { return _to._y; }

    Segment()
        : _from(FloatPoint())
        , _to(FloatPoint())
        , _slopeInv(NAN)
        , _direction(EqualOrNonExist)
    {}
    Segment(FloatPoint from, FloatPoint to)
        : _from(from)
        , _to(to)
    {
        Float denom = _to._y - _from._y;
        if (denom) {
            if (from._y > to._y) {
                _from = to;
                _to = from;
                _direction = Negative;
                denom *= -1;
            } else
                _direction = Positive;

            _slopeInv = (_to._x - _from._x) / (denom);
        } else {
            _direction = EqualOrNonExist;
            _slopeInv = INFINITY;
        }
    }

    // FIXME: find a good function name:
    Float factor() const { return _slopeInv * _from._y - _from._x; }
    bool isOnSegment(Float y) const { return y < _to._y && y > _from._y; }
    Segment splitSegment(Float y)
    {
        ASSERT(y > _from._y && y < _to._y);

        Float x = _slopeInv * (y - _from._y) + _from._x;
        FloatPoint to = _to;
        _to = FloatPoint(x, y);
        FloatPoint newPoint = _to;

        if (_direction == Negative) {
            newPoint = to;
            to = _to;
        }

        return Segment(newPoint, to);
    }
    Float computeIntersectionY(Segment* segment) const
    {
        Float y = NAN;
        if (this == segment)
            return y;
        if (_from._x != segment->_from._x && _to._x != segment->_to._x) {
            Float denom = (_slopeInv - segment->_slopeInv);
            if (denom)
                y = (this->factor() - segment->factor()) / denom;
            if (y <= _from._y || y >= _to._y)
                y = INFINITY;
        }
        return y;
    }

    FloatPoint _from;
    FloatPoint _to;
    Float _slopeInv;
    int _direction;
};

inline std::ostream& operator<<(std::ostream& os, const Segment& s)
{
    return os << s._from << ((s._direction < 0) ? ">" : ((s._direction > 0) ? "<" : "=")) << s._to;
}

inline bool operator<(const Segment& lhs, const Segment& rhs)
{
    assert(lhs._from <= lhs._to && rhs._from <= rhs._to);

    return (lhs._from < rhs._from) || (lhs._from == rhs._from && lhs._to < rhs._to);
}

/* SegmentList, SegmentTree */

typedef std::list<Segment> SegmentList;
typedef std::map<const Float, SegmentList*> SegmentTree;

/* SegmentApproximator */

class SegmentApproximator {
public:
    SegmentApproximator(Float epsilon = 1)
        : _epsilon(epsilon)
    {
    }
    ~SegmentApproximator()
    {
        for (SegmentTree::iterator it = _segments.begin(); it != _segments.end(); it++)
        {
            delete it->second;
        }
    }

    void insertSegment(Segment segment);
    void insertSegment(FloatPoint from, FloatPoint to) { insertSegment(Segment(from, to)); }
    void insertQuadCurve(FloatPoint from, FloatPoint control, FloatPoint to);
    void insertBezierCurve(FloatPoint from, FloatPoint control1, FloatPoint control2, FloatPoint to);
    void insertArc(FloatPoint center, FloatPoint radius, Float startAngle, Float endAngle, bool antiClockwise);

    SegmentList* segments();
    BoundingBox boundingBox() const { return _boundingBox; }

    inline void splitSegments();
    void printSegements();
private:
    SegmentTree _segments;
    Float _epsilon;

    BoundingBox _boundingBox;
};

/* Trapezoid */

struct Trapezoid {
    Float _bottom;
    Float _bottomLeft;
    Float _bottomRight;
    Float _top;
    Float _topLeft;
    Float _topRight;
};

inline std::ostream& operator<<(std::ostream& os, const Trapezoid& t)
{
    return os << t._bottom << "," << t._bottomLeft << "," << t._bottomRight << ","<< t._top << "," << t._topLeft << "," << t._topRight;
}

/* TrapezoidList */

typedef std::list<Trapezoid> TrapezoidList;

/* TrapezoidTessallator */

class TrapezoidTessallator {
public:
    enum FillRule {
        EvenOdd,
        NonZero,
    };

    TrapezoidTessallator(Path* path, FillRule fillRule = NonZero)
        : _path(path)
        , _fillRule(fillRule)
    {
    }

    FillRule fillRule() const { return _fillRule; }
    TrapezoidList trapezoidList();
    BoundingBox boundingBox() const { return _boundingBox; }

private:
    Path* _path;
    FillRule _fillRule;

    BoundingBox _boundingBox;
};

} // namespace gepard

#endif // gepard_path_h
