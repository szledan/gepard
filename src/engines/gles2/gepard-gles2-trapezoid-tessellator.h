/* Copyright (C) 2015-2017, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_GLES2_TRAPEZOID_TESSELLATOR_H
#define GEPARD_GLES2_TRAPEZOID_TESSELLATOR_H

#include "gepard-defs.h"
#include "gepard-path.h"
#include "gepard-types.h"
#include <list>
#include <map>

namespace gepard {
namespace gles2 {

/* Segment */

struct Segment {
    enum {
        Negative = -1,
        EqualOrNonExist = 0,
        Positive = 1,
    };

    Segment(FloatPoint from, FloatPoint to, unsigned sameId = 0, Float slope = NAN);

    const int topY() const;
    const int bottomY() const;
    const Float slopeInv() const;
    const Float factor() const;
    const bool isOnSegment(const Float y) const;

    const Segment splitSegment(const Float y);
    const Float computeIntersectionY(Segment* segment) const;

    FloatPoint from;
    FloatPoint to;
    Float id;
    Float realSlope;
    int direction;
};

inline std::ostream& operator<<(std::ostream& os, const Segment& s);

inline bool operator<(const Segment& lhs, const Segment& rhs);
inline bool operator==(const Segment& lhs, const Segment& rhs);
inline bool operator<=(const Segment& lhs, const Segment& rhs);

/* SegmentList, SegmentTree */

typedef std::list<Segment> SegmentList;
typedef std::map<const int, SegmentList*> SegmentTree;

/* SegmentApproximator */

class SegmentApproximator {
public:
    SegmentApproximator(const int antiAliasLevel = 16, const Float factor = 1.0);
    ~SegmentApproximator();

    void insertLine(const FloatPoint& from, const FloatPoint& to);
    void insertQuadCurve(const FloatPoint& from, const FloatPoint& control, const FloatPoint& to);
    void insertBezierCurve(const FloatPoint& from, const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& to);
    void insertArc(const FloatPoint& lastEndPoint, const ArcElement* arcElement);

    SegmentList* segments();
    const BoundingBox boundingBox() const { return _boundingBox; }

    inline void splitSegments();
    void printSegements();

private:
    void insertSegment(const FloatPoint& from, const FloatPoint& to);

    const bool quadCurveIsLineSegment(FloatPoint[]);
    void splitQuadraticCurve(FloatPoint[]);
    const bool curveIsLineSegment(FloatPoint[]);
    void splitCubeCurve(FloatPoint[]);
    const int calculateSegments(const Float& angle, const Float& radius);
    void arcToCurve(FloatPoint[], const Float&, const Float&);

    const int _kAntiAliasLevel;
    const Float _kTolerance;
    SegmentTree _segments;

    BoundingBox _boundingBox;
};

/* Trapezoid */

struct Trapezoid {
    const bool isMergableInTo(const Trapezoid* other) const;

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

inline std::ostream& operator<<(std::ostream& os, const Trapezoid& t);

inline bool operator<(const Trapezoid& lhs, const Trapezoid& rhs);
inline bool operator==(const Trapezoid& lhs, const Trapezoid& rhs);
inline bool operator<=(const Trapezoid& lhs, const Trapezoid& rhs);

/* TrapezoidList */

typedef std::list<Trapezoid> TrapezoidList;

/* TrapezoidTessellator */

class TrapezoidTessellator {
public:
    enum FillRule {
        EvenOdd,
        NonZero,
    };

    TrapezoidTessellator(Path* path, FillRule fillRule = NonZero, int antiAliasingLevel = 16);

    const FillRule fillRule() const { return _fillRule; }
    const TrapezoidList trapezoidList();
    const BoundingBox boundingBox() const { return _boundingBox; }
    const int antiAliasingLevel() const { return _antiAliasingLevel; }

private:
    Path* _path;
    const FillRule _fillRule;
    const int _antiAliasingLevel;

    BoundingBox _boundingBox;
};

} // namespace gles2
} // namespace gepard

#endif // GEPARD_GLES2_TRAPEZOID_TESSELLATOR_H
