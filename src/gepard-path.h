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
    PathElement() : next(nullptr), type(PathElementTypes::Undefined) {}
    PathElement(PathElementType type) : next(nullptr), type(type) {}
    PathElement(PathElementType type, FloatPoint to)
        : next(nullptr)
        , type(type)
        , to(to)
    {}

    bool isMoveTo() const { return this->type == PathElementTypes::MoveTo; }
    virtual std::ostream& output(std::ostream& os) const
    {
        return os << this->to;
    }

    PathElement* next;
    PathElementType type;
    FloatPoint to;
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
        , control(control)
    {}

    std::ostream& output(std::ostream& os) const
    {
        return PathElement::output(os << "Q" << this->control << " ");
    }

    FloatPoint control;
};

struct BezierCurveToElement : public PathElement {
    BezierCurveToElement(FloatPoint control1, FloatPoint control2, FloatPoint to)
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
    ArcElement(FloatPoint center, FloatPoint radius, Float startAngle, Float endAngle, bool antiClockwise = true)
        : PathElement(PathElementTypes::Arc, FloatPoint (cos(endAngle) * radius.x, sin(endAngle) * radius.y))
        , center(center)
        , radius(radius)
        , startAngle(startAngle)
        , endAngle(endAngle)
        , antiClockwise(antiClockwise)
    {}

    std::ostream& output(std::ostream& os) const
    {
        // FIXME: generate SVG representation
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
    PathData()
        : _firstElement(nullptr)
        , _lastElement(nullptr)
        , _lastMoveToElement(nullptr)
    {}

    void addMoveToElement(FloatPoint);
    void addLineToElement(FloatPoint);
    void addQuadaraticCurveToElement(FloatPoint, FloatPoint);
    void addBezierCurveToElement(FloatPoint, FloatPoint, FloatPoint);
    void addArcElement(FloatPoint, FloatPoint, Float, Float, bool = true);
    void addCloseSubpath();
    void dump();

    PathElement* firstElement() const { return _firstElement; }
    PathElement* lastElement() const { return _lastElement; }

private:
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
    {}

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

    Segment()
        : from(FloatPoint())
        , to(FloatPoint())
        , slopeInv(NAN)
        , direction(EqualOrNonExist)
    {}
    Segment(FloatPoint from, FloatPoint to)
        : from(from)
        , to(to)
    {
        Float denom = this->to.y - this->from.y;
        if (denom) {
            if (from.y > to.y) {
                this->from = to;
                this->to = from;
                this->direction = Negative;
                denom *= -1;
            } else {
                this->direction = Positive;
            }

            this->slopeInv = (this->to.x - this->from.x) / (denom);
        } else {
            this->direction = EqualOrNonExist;
            this->slopeInv = INFINITY;
        }
    }

    Float topY() const { return this->from.y; }
    Float bottomY() const { return this->to.y; }
    // FIXME: find a good function name:
    Float factor() const { return this->slopeInv * this->from.y - this->from.x; }
    bool isOnSegment(Float y) const { return y < this->to.y && y > this->from.y; }

    Segment splitSegment(Float y)
    {
        ASSERT(y > this->from.y && y < this->to.y);

        Float x = this->slopeInv * (y - this->from.y) + this->from.x;
        FloatPoint to = this->to;
        this->to = FloatPoint(x, y);
        FloatPoint newPoint = this->to;

        if (this->direction == Negative) {
            newPoint = to;
            to = this->to;
        }

        return Segment(newPoint, to);
    }
    Float computeIntersectionY(Segment* segment) const
    {
        Float y = NAN;
        if (this == segment)
            return y;

        if (this->from.x != segment->from.x && this->to.x != segment->to.x) {
            Float denom = (this->slopeInv - segment->slopeInv);
            if (denom) {
                y = (this->factor() - segment->factor()) / denom;
            }
            if (y <= this->from.y || y >= this->to.y) {
                y = INFINITY;
            }
        } // FIXME: else: y is NaN.
        return y;
    }

    FloatPoint from;
    FloatPoint to;
    Float slopeInv;
    int direction;
};

inline std::ostream& operator<<(std::ostream& os, const Segment& s)
{
    return os << s.from << ((s.direction < 0) ? ">" : ((s.direction > 0) ? "<" : "=")) << s.to;
}

inline bool operator<(const Segment& lhs, const Segment& rhs)
{
    assert(lhs.from <= lhs.to && rhs.from <= rhs.to);

    return (lhs.from < rhs.from) || (lhs.from == rhs.from && lhs.to < rhs.to);
}

/* SegmentList, SegmentTree */

typedef std::list<Segment> SegmentList;
typedef std::map<const Float, SegmentList*> SegmentTree;

/* SegmentApproximator */

class SegmentApproximator {
public:
    SegmentApproximator(Float epsilon = 1)
        : _epsilon(epsilon)
    {}
    ~SegmentApproximator()
    {
        for (SegmentTree::iterator it = _segments.begin(); it != _segments.end(); ++it) {
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
    Float bottom;
    Float bottomLeft;
    Float bottomRight;
    Float top;
    Float topLeft;
    Float topRight;
};

inline std::ostream& operator<<(std::ostream& os, const Trapezoid& t)
{
    return os << t.bottom << "," << t.bottomLeft << "," << t.bottomRight << ","<< t.top << "," << t.topLeft << "," << t.topRight;
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
    {}

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
