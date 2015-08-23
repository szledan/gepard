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

/* Point */

union Point {
    Point () : real() {}
    Point (FloatPoint f) : real(f) {}
    Point (IntPoint i) : ceiled(i) {}

    Float x() const { return real._x; }
    Float y() const { return real._y; }
    int intX() const { return ceiled._x; }
    int intY() const { return ceiled._y; }

    FloatPoint real;
    IntPoint ceiled;
};

/* Segment */

struct Segment {
    enum {
        Negative = -1,
        EqualOrNonExist = 0,
        Positive = 1,
    };

    Float topY() const { return _from.y(); }
    Float bottomY() const { return _to.y(); }

    Segment()
        : _from(Point())
        , _to(Point())
        , _slopeInv(NAN)
        , _direction(EqualOrNonExist)
    {}
    Segment(FloatPoint from, FloatPoint to)
        : _from(from)
        , _to(to)
    {
        Float denom = _to.y() - _from.y();
        if (denom) {
            if (from._y > to._y) {
                _from = to;
                _to = from;
                _direction = Negative;
                denom *= -1;
            } else
                _direction = Positive;

            _slopeInv = (_to.x() - _from.x()) / (denom);
        } else {
            _direction = EqualOrNonExist;
            _slopeInv = INFINITY;
        }
    }

    // FIXME: find a good function name:
    Float factor() const { return _slopeInv * _from.y() - _from.x(); }
    bool isOnSegment(Float y) const { return y < _to.y() && y > _from.y(); }
    Segment splitSegment(Float y)
    {
        ASSERT(y > _from.y() && y < _to.y());

        Float x = _slopeInv * (y - _from.y()) + _from.x();
        FloatPoint newPoint = FloatPoint(x, y);
        FloatPoint to = _to.real;
        _to.real = newPoint;

        return Segment(newPoint, to);
    }
    Float computeIntersectionY(Segment* segment) const
    {
        Float y = NAN;
        if (this == segment)
            return y;
        if (_from.x() != segment->_from.x() && _to.x() != segment->_to.x()) {
            Float denom = (_slopeInv - segment->_slopeInv);
            if (denom)
                y = (this->factor() - segment->factor()) / denom;
            if (y <= _from.y() || y >= _to.y())
                y = INFINITY;
        }
        return y;
    }

    Point _from;
    Point _to;
    Float _slopeInv;
    int _direction;
};

inline std::ostream& operator<<(std::ostream& os, const Segment& s)
{
    return os << s._from.real << "-" << s._to.real;
}

inline bool operator<(const Segment& lhs, const Segment& rhs)
{
    assert(lhs._from.real <= lhs._to.real && rhs._from.real <= rhs._to.real);

    if (lhs._from.real < rhs._from.real)
        return true;

    if (lhs._from.real == rhs._from.real) {
        if (fabs(lhs._slopeInv) > fabs(rhs._slopeInv))
            return true;
    }

    return false;
}

/* SegmentApproximator */

typedef std::list<Segment> SegmentList;
typedef std::map<const Float, SegmentList*> SegmentTree;

class SegmentApproximator {
public:
    SegmentApproximator(float epsilon = 1)
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
    void insertArc(FloatPoint center, FloatPoint radius, float startAngle, float endAngle, bool antiClockwise);

    SegmentList* segments();

    inline void splitSegments();
    void printSegements();
private:
    SegmentTree _segments;
    float _epsilon;
};

/* Trapezoid */

struct Trapezoid {
    Float _top;
    Float _topLeft;
    Float _topRight;

    Float _bottom;
    Float _bottomLeft;
    Float _bottomRight;
};

/* TrapezoidList */
struct TrapezoidList {
    uint32_t numerOfTrapezoids;
    Trapezoid* trapezoid;
};

/* TrapezoidTessallator */

class TrapezoidTessallator {
public:
    TrapezoidTessallator(Path* path)
        : _path(path)
    {
    }

    TrapezoidList trapezoidList();

private:
    Path* _path;
};

} // namespace gepard

#endif // gepard_path_h
