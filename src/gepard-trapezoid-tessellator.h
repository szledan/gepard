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

#ifndef gepard_trapezoid_tessellator_h
#define gepard_trapezoid_tessellator_h

#include "config.h"

#include "gepard-path.h"
#include "gepard-types.h"
#include <list>
#include <map>
#include <math.h>

namespace gepard {

/* Segment */

static unsigned s_segmentIds = 0;

// TODO: Use class instead of struct.
struct Segment {
    enum {
        Negative = -1,
        EqualOrNonExist = 0,
        Positive = 1,
    };

    Segment(FloatPoint from, FloatPoint to, unsigned sameId = 0, Float slope = NAN)
        : from(from)
        , to(to)
        , id((sameId) ? sameId : ++s_segmentIds)
    {
        Float slopeInv = NAN;
        Float denom = this->to.y - this->from.y;
        if (denom) {
            if (denom < 0) {
                this->from = to;
                this->to = from;
                this->direction = Negative;
                denom *= -1;
            } else {
                this->direction = Positive;
            }

            slopeInv = (this->to.x - this->from.x) / (denom);
        } else {
            this->direction = EqualOrNonExist;
            slopeInv = INFINITY;
        }
        realSlope = (slope == NAN) ? slopeInv : slope;
    }

    int topY() const { return floor(this->from.y); }
    int bottomY() const { return floor(this->to.y); }
    Float slopeInv() const { return (this->to.x - this->from.x) / (this->to.y - this->from.y); }
    // TODO: find a good function name:
    Float factor() const { return this->slopeInv() * this->from.y - this->from.x; }
    bool isOnSegment(Float y) const { return y < this->to.y && y > this->from.y; }

    Segment splitSegment(Float y)
    {
        ASSERT(this->from.y < this->to.y);
        ASSERT(y > this->from.y && y < this->to.y);

        Float x = this->slopeInv() * (y - this->from.y) + this->from.x;
        FloatPoint to = this->to;
        this->to = FloatPoint(x, y);
        FloatPoint newPoint = this->to;

        if (this->direction == Negative) {
            newPoint = to;
            to = this->to;
        }

        ASSERT(this->from.y != newPoint.y);
        ASSERT(newPoint.y != to.y);

        return Segment(newPoint, to, this->id, this->realSlope);
    }
    Float computeIntersectionY(Segment* segment) const
    {
        Float y = NAN;
        if (this == segment)
            return y;

        if (this->from.x != segment->from.x && this->to.x != segment->to.x) {
            Float denom = (this->slopeInv() - segment->slopeInv());
            if (denom) {
                y = (this->factor() - segment->factor()) / denom;
            }
            if (!isOnSegment(y)) {
                y = INFINITY;
            }
        } // TODO: else: y is NaN.
        return y;
    }

    FloatPoint from;
    FloatPoint to;
    Float id;
    Float realSlope;
    int direction;
};

inline std::ostream& operator<<(std::ostream& os, const Segment& s)
{
    return os << s.from << ((s.direction < 0) ? "<" : ((s.direction > 0) ? ">" : "=")) << s.to;
}

inline bool operator<(const Segment& lhs, const Segment& rhs)
{
    ASSERT(lhs.from <= lhs.to && rhs.from <= rhs.to);
    return (lhs.from < rhs.from) || (lhs.from == rhs.from && lhs.to < rhs.to);
}

inline bool operator==(const Segment& lhs, const Segment& rhs)
{
    return (lhs.from == rhs.from) && (lhs.to == rhs.to);
}

inline bool operator<=(const Segment& lhs, const Segment& rhs)
{
    return (lhs < rhs) || (lhs == rhs);
}

/* SegmentList, SegmentTree */

typedef std::list<Segment> SegmentList;
typedef std::map<const int, SegmentList*> SegmentTree;

/* SegmentApproximator */

class SegmentApproximator {
public:
    SegmentApproximator(const int antiAliasLevel = 16, const Float factor = 1.0)
        : _kAntiAliasLevel(antiAliasLevel > 0 ? antiAliasLevel : 16)
        , _kTolerance((factor > 0.0 ? factor : 1.0 ) / ((Float)_kAntiAliasLevel))
    {
    }
    ~SegmentApproximator()
    {
        for (SegmentTree::iterator it = _segments.begin(); it != _segments.end(); ++it) {
            delete it->second;
        }
    }

    void insertLine(const FloatPoint& from, const FloatPoint& to);
    void insertQuadCurve(const FloatPoint& from, const FloatPoint& control, const FloatPoint& to);
    void insertBezierCurve(const FloatPoint& from, const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& to);
    void insertArc(const FloatPoint& lastEndPoint, const ArcElement* arcElement);

    SegmentList* segments();
    BoundingBox boundingBox() const { return _boundingBox; }

    inline void splitSegments();
    void printSegements();

private:
    void insertSegment(const FloatPoint& from, const FloatPoint& to);

    bool quadCurveIsLineSegment(FloatPoint[]);
    void splitQuadraticCurve(FloatPoint[]);
    bool curveIsLineSegment(FloatPoint[]);
    void splitCubeCurve(FloatPoint[]);
    int calculateSegments(const Float& angle, const Float& radius);
    void arcToCurve(FloatPoint[], const Float&, const Float&);

    SegmentTree _segments;
    const int _kAntiAliasLevel;
    const Float _kTolerance;

    BoundingBox _boundingBox;
};

/* Trapezoid */

struct Trapezoid {
    bool isMergableInTo(const Trapezoid* other) const
    {
        ASSERT(this->bottomY == other->topY);

        if (this->bottomLeftX == other->topLeftX && this->bottomRightX == other->topRightX) {
            if (this->leftId == other->leftId && this->rightId == other->rightId)
                return true;

            if (this->leftSlope == other->leftSlope && this->rightSlope == other->rightSlope)
                return true;
        }

        return false;
    }

    Float topY;
    Float topLeftX;
    Float topRightX;
    Float bottomY;
    Float bottomLeftX;
    Float bottomRightX;

    unsigned leftId;
    unsigned rightId;
    Float leftSlope;
    Float rightSlope;
};

inline std::ostream& operator<<(std::ostream& os, const Trapezoid& t)
{
    return os << t.topY << "," << t.topLeftX << "," << t.topRightX << "," << t.bottomY << "," << t.bottomLeftX << "," << t.bottomRightX;
}

inline bool operator<(const Trapezoid& lhs, const Trapezoid& rhs)
{
    if (lhs.topY < rhs.topY)
        return true;

    if (lhs.topY == rhs.topY) {
        if (lhs.topLeftX < rhs.topLeftX)
            return true;

        if (lhs.topLeftX == rhs.topLeftX) {
            if (lhs.bottomLeftX < rhs.bottomLeftX)
                return true;
        }
    }

    return false;
}

inline bool operator==(const Trapezoid& lhs, const Trapezoid& rhs)
{
    return lhs.topY == rhs.topY && lhs.topLeftX == rhs.topLeftX && lhs.topRightX == rhs.topRightX
           && lhs.bottomY == rhs.bottomY && lhs.bottomLeftX == rhs.bottomLeftX &&  lhs.bottomRightX == rhs.bottomRightX;
}

inline bool operator<=(const Trapezoid& lhs, const Trapezoid& rhs)
{
    return lhs < rhs || lhs == rhs;
}

/* TrapezoidList */

typedef std::list<Trapezoid> TrapezoidList;

/* TrapezoidTessellator */

class TrapezoidTessellator {
public:
    enum FillRule {
        EvenOdd,
        NonZero,
    };

    TrapezoidTessellator(Path* path, FillRule fillRule = NonZero, int antiAliasingLevel = 16)
        : _path(path)
        , _fillRule(fillRule)
        , _antiAliasingLevel(antiAliasingLevel)
    {}

    FillRule fillRule() const { return _fillRule; }
    TrapezoidList trapezoidList();
    BoundingBox boundingBox() const { return _boundingBox; }
    int antiAliasingLevel() const { return _antiAliasingLevel; }

private:
    Path* _path;
    FillRule _fillRule;

    BoundingBox _boundingBox;
    int _antiAliasingLevel;
};

} // namespace gepard

#endif // gepard_trapezoid_tessellator_h
