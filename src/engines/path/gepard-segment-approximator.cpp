/* Copyright (C) 2019, Gepard Graphics
 * Copyright (C) 2015-2019, Szilard Ledan <szledan@gmail.com>
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

#include "gepard-segment-approximator.h"

#include "gepard-defs.h"
#include "gepard-float-point.h"
#include "gepard-float.h"
#include "gepard-logging.h"
#include "gepard-old-segment-approximator.h"
#include "gepard-segment.h"
#include "gepard-transform.h"
#include <cmath>
#include <list>
#include <set>

namespace gepard {

SegmentApproximator::SegmentApproximator(const int antiAliasLevel)
    : kAntiAliasLevel(antiAliasLevel > 0 ? antiAliasLevel : GD_ANTIALIAS_LEVEL)
    , kTolerance(1.0)
{
}

SegmentApproximator::~SegmentApproximator()
{
    for (OldSegmentTree::iterator it = _segments.begin(); it != _segments.end(); ++it) {
        delete it->second;
    }
}

void SegmentApproximator::insertLine(const FloatPoint& from, const FloatPoint& to)
{
    if (from.y == to.y)
        return;

    GD_LOG(TRACE) << "Insert line: " << from << "->" << to;
    insertSegment(FloatPoint(from.x * kAntiAliasLevel, std::floor(from.y * kAntiAliasLevel)), FloatPoint(to.x * kAntiAliasLevel, std::floor(to.y * kAntiAliasLevel)));
}

const bool SegmentApproximator::quadCurveIsLineSegment(FloatPoint points[])
{
    const Float x0 = points[0].x;
    const Float y0 = points[0].y;
    const Float x1 = points[1].x;
    const Float y1 = points[1].y;
    const Float x2 = points[2].x;
    const Float y2 = points[2].y;

    const Float dt = std::fabs((x2 - x0) * (y0 - y1) - (x0 - x1) * (y2 - y0));

    if (dt > kTolerance)
        return false;

    Float minX, minY, maxX, maxY;

    if (x0 < x2) {
        minX = x0 - kTolerance;
        maxX = x2 + kTolerance;
    } else {
        minX = x2 - kTolerance;
        maxX = x0 + kTolerance;
    }

    if (y0 < y2) {
        minY = y0 - kTolerance;
        maxY = y2 + kTolerance;
    } else {
        minY = y2 - kTolerance;
        maxY = y0 + kTolerance;
    }

    return !(x1 < minX || x1 > maxX || y1 < minY || y1 > maxY);
}

void SegmentApproximator::splitQuadraticCurve(FloatPoint points[])
{
    const FloatPoint a = points[0];
    const FloatPoint b = points[1];
    const FloatPoint c = points[2];
    const FloatPoint ab = (a + b) / 2.0;
    const FloatPoint bc = (b + c) / 2.0;
    const FloatPoint mid = (ab + bc) / 2.0;

    points[0] = a;
    points[1] = ab;
    points[2] = mid;
    points[3] = bc;
    points[4] = c;
}

void SegmentApproximator::insertQuadCurve(const FloatPoint& from, const FloatPoint& control, const FloatPoint& to)
{
    // De Casteljau algorithm.
    const int kNumberOfParts = 16;
    const int max = kNumberOfParts * 2 + 1;
    FloatPoint buffer[max];
    FloatPoint* points = buffer;

    points[0] = from;
    points[1] = control;
    points[2] = to;

    do {
        if (quadCurveIsLineSegment(points)) {
            insertLine(points[0], points[2]);
            points -= 2;
            continue;
        }

        splitQuadraticCurve(points);
        points += 2;

        if (points >= buffer + max - 3) {
            // This recursive code path is rarely executed.
            insertQuadCurve(points[0], points[1], points[2]);
            points -= 2;
        }
    } while (points >= buffer);
}

const bool SegmentApproximator::curveIsLineSegment(FloatPoint points[])
{
    const Float x0 = points[0].x;
    const Float y0 = points[0].y;
    const Float x1 = points[1].x;
    const Float y1 = points[1].y;
    const Float x2 = points[2].x;
    const Float y2 = points[2].y;
    const Float x3 = points[3].x;
    const Float y3 = points[3].y;

    const Float dt1 = std::fabs((x3 - x0) * (y0 - y1) - (x0 - x1) * (y3 - y0));
    const Float dt2 = std::fabs((x3 - x0) * (y0 - y2) - (x0 - x2) * (y3 - y0));

    if (dt1 > kTolerance || dt2 > kTolerance)
        return false;

    Float minX, minY, maxX, maxY;

    if (x0 < x3) {
        minX = x0 - kTolerance;
        maxX = x3 + kTolerance;
    } else {
        minX = x3 - kTolerance;
        maxX = x0 + kTolerance;
    }

    if (y0 < y3) {
        minY = y0 - kTolerance;
        maxY = y3 + kTolerance;
    } else {
        minY = y3 - kTolerance;
        maxY = y0 + kTolerance;
    }

    return !(x1 < minX || x1 > maxX || y1 < minY || y1 > maxY
             || x2 < minX || x2 > maxX || y2 < minY || y2 > maxY);
}

const bool SegmentApproximator::collinear(const FloatPoint& p0, const FloatPoint& p1, const FloatPoint& p2)
{
    return std::fabs((p2.x - p0.x) * (p0.y - p1.y) - (p0.x - p1.x) * (p2.y - p0.y)) <= kTolerance;
}

const bool SegmentApproximator::curveIsLineSegment(const FloatPoint& p0, const FloatPoint& p1, const FloatPoint& p2)
{
    if (!collinear(p0, p1, p2))
        return false;

    float minX, minY, maxX, maxY;

    if (p0.x < p2.x) {
        minX = p0.x - kTolerance;
        maxX = p2.x + kTolerance;
    } else {
        minX = p2.x - kTolerance;
        maxX = p0.x + kTolerance;
    }

    if (p0.y < p2.y) {
        minY = p0.y - kTolerance;
        maxY = p2.y + kTolerance;
    } else {
        minY = p2.y - kTolerance;
        maxY = p0.y + kTolerance;
    }

    return !(p1.x < minX || p1.x > maxX || p1.y < minY || p1.y > maxY);
}

void SegmentApproximator::splitCubeCurve(FloatPoint points[])
{
    const FloatPoint a = points[0];
    const FloatPoint b = points[1];
    const FloatPoint c = points[2];
    const FloatPoint d = points[3];
    const FloatPoint ab = (a + b) / 2.0;
    const FloatPoint bc = (b + c) / 2.0;
    const FloatPoint cd = (c + d) / 2.0;
    const FloatPoint abbc = (ab + bc) / 2.0;
    const FloatPoint bccd = (bc + cd) / 2.0;
    const FloatPoint mid = (abbc + bccd) / 2.0;

    points[0] = a;
    points[1] = ab;
    points[2] = abbc;
    points[3] = mid;
    points[4] = bccd;
    points[5] = cd;
    points[6] = d;
}

void SegmentApproximator::insertBezierCurve(const FloatPoint& from, const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& to)
{
    // De Casteljau algorithm.
    const int kNumberOfParts = 16;
    const int max = kNumberOfParts * 3 + 1;
    FloatPoint buffer[max];
    FloatPoint* points = buffer;

    points[0] = from;
    points[1] = control1;
    points[2] = control2;
    points[3] = to;

    do {
        if (curveIsLineSegment(points)) {
            insertLine(points[0], points[3]);
            points -= 3;
            continue;
        }

        splitCubeCurve(points);
        points += 3;

        if (points >= buffer + max - 4) {
            // This recursive code path is rarely executed.
            insertBezierCurve(points[0], points[1], points[2], points[3]);
            points -= 3;
        }
    } while (points >= buffer);
}

const int SegmentApproximator::calculateArcSegments(const Float& angle, const Float& radius)
{
    const Float epsilon = kTolerance / radius;
    Float angleSegment;
    Float error;

    int i = 1;
    do {
        angleSegment = piFloat / i++;
        error = 2.0 / 27.0 * std::pow(std::sin(angleSegment / 4.0), 6) / std::pow(std::cos(angleSegment / 4.0), 2);
    } while (error > epsilon);

    return std::ceil(fabs(angle) / angleSegment);
}

void SegmentApproximator::arcToCurve(FloatPoint result[], const Float& startAngle, const Float& endAngle)
{
    const Float sinStartAngle = std::sin(startAngle);
    const Float cosStartAngle = std::cos(startAngle);
    const Float sinEndAngle = std::sin(endAngle);
    const Float cosEndAngle = std::cos(endAngle);

    const Float height = 4.0 / 3.0 * std::tan((endAngle - startAngle) / 4.0);

    result[0].x = cosStartAngle - height * sinStartAngle;
    result[0].y = sinStartAngle + height * cosStartAngle;
    result[1].x = cosEndAngle + height * sinEndAngle;
    result[1].y = sinEndAngle - height * cosEndAngle;
    result[2].x = cosEndAngle;
    result[2].y = sinEndAngle;
}

void SegmentApproximator::insertArc(const FloatPoint& lastEndPoint, const ArcElement* arcElement, const Transform& globalTransform)
{
    Float startAngle = arcElement->startAngle;
    const Float endAngle = arcElement->endAngle;
    const bool antiClockwise = arcElement->counterClockwise;

    Transform arcTransform = globalTransform;
    Transform axesTransform = { arcElement->radius.x, 0.0, 0.0, arcElement->radius.y, arcElement->center.x, arcElement->center.y };
    arcTransform *= arcElement->transform * axesTransform;

    FloatPoint startPoint = arcTransform.apply(FloatPoint(std::cos(startAngle), std::sin(startAngle)));
    insertLine(lastEndPoint, startPoint);

    GD_ASSERT(startAngle != endAngle);

    const Float deltaAngle = antiClockwise ? startAngle - endAngle : endAngle - startAngle;

    const int segments = calculateArcSegments(deltaAngle, std::max(arcElement->radius.x * 2, arcElement->radius.y * 2));
    Float step = deltaAngle / segments;

    if (antiClockwise) {
        step = -step;
    }

    FloatPoint bezierPoints[3];
    for (int i = 0; i < segments; i++, startAngle += step) {
        arcToCurve(bezierPoints, startAngle, (i == segments - 1) ? endAngle : startAngle + step);
        bezierPoints[0] = arcTransform.apply(bezierPoints[0]);
        bezierPoints[1] = arcTransform.apply(bezierPoints[1]);
        if (i == segments - 1) {
            bezierPoints[2] = globalTransform.apply(arcElement->to);
        } else {
            bezierPoints[2] = arcTransform.apply(bezierPoints[2]);
        }
        insertBezierCurve(startPoint, bezierPoints[0], bezierPoints[1], bezierPoints[2]);
        startPoint = bezierPoints[2];
    }
}

inline void SegmentApproximator::splitSegments()
{
    for (OldSegmentTree::iterator currentSegments = _segments.begin(); currentSegments != _segments.end(); ++currentSegments) {
        OldSegmentTree::iterator newSegments = currentSegments;
        ++newSegments;
        if (newSegments != _segments.end()) {
            OldSegmentList* currentList = currentSegments->second;
            OldSegmentList* newList = newSegments->second;
            int splitY = std::floor(newSegments->first);
            GD_ASSERT(currentList && newList);
            GD_ASSERT(currentSegments->first < newSegments->first);

            for (OldSegment& segment : *currentList) {
                if (segment.isOnSegment(splitY)) {
                    newList->push_front(segment.splitSegment(splitY));
                }
            }
        }
    }
}

OldSegmentList* SegmentApproximator::segments()
{
    // Split segments with all y lines.
    splitSegments();

    // Find all intersection points.
    std::set<int> ys;
    for (auto& currentSegments : _segments) {
        OldSegmentList* currentList = currentSegments.second;
        currentList->sort();
        OldSegmentList::iterator currentSegment = currentList->begin();
        for (OldSegmentList::iterator segment = currentSegment; currentSegment != currentList->end(); ++segment) {
            if (segment != currentList->end()) {
                Float y = 0;
                if (currentSegment->computeIntersectionY(&(*segment), y)) {
                    const int intersectionY = std::floor(y);
                    ys.insert(intersectionY);
                    if ((Float)intersectionY != y) {
                        GD_ASSERT((Float)intersectionY < y);
                        ys.insert(intersectionY + 1);
                    }
                }
            } else {
                segment = ++currentSegment;
            }
        }
    }

    for (auto y : ys) {
        insertSegmentList(y);
    }

    // Split segments with all y lines.
    splitSegments();

    // Merge and sort all segment list.
    OldSegmentList* segments = new OldSegmentList();
    for (OldSegmentTree::iterator currentSegments = _segments.begin(); currentSegments != _segments.end(); ++currentSegments) {
        OldSegmentList* currentList = currentSegments->second;
        currentList->sort();

        bool needSorting = false;
        // Fix intersection pairs.
        for (OldSegmentList::iterator segment = currentList->begin(); segment != currentList->end(); ++segment) {
            GD_ASSERT(segment->to.y - segment->from.y >= 1.0);
            if (segment->to.y - segment->from.y == 1.0) {
                for (OldSegmentList::iterator furtherSegment = segment; furtherSegment != currentList->end() ; ++furtherSegment) {
                    GD_ASSERT(segment->from.y == furtherSegment->from.y);
                    GD_ASSERT(segment->to.y == furtherSegment->to.y);
                    //! \todo(szledan): need to test this assert:
                    //! GD_ASSERT(furtherSegment->from.x >= segment->from.x)
                    if (furtherSegment->to.x < segment->to.x) {
                        if (furtherSegment->from.x - segment->from.x < segment->to.x - furtherSegment->to.x) {
                            furtherSegment->from.x = segment->from.x;
                            needSorting = true;
                        } else {
                            furtherSegment->to.x = segment->to.x;
                        }
                    }
                }
            }
        }
        if (needSorting) {
            --currentSegments;
        } else {
            // Merge segment lists.
            segments->merge(*currentList);
        }
    }

    // Return independent segments.
    return segments;
}

void SegmentApproximator::insertSegment(const FloatPoint& from, const FloatPoint& to)
{
    if (from.y == to.y)
        return;

    OldSegment segment(from, to);

    // Update bounding-box.
    _boundingBox.stretch(segment.from);
    _boundingBox.stretch(segment.to);

    // Insert segment.
    const int topY = segment.topY();
    const int bottomY = segment.bottomY();

    _trapezoids.addSegment(Segment(from, to));
    insertSegmentList(topY)->push_front(segment);
    insertSegmentList(bottomY);
}

OldSegmentList* SegmentApproximator::insertSegmentList(const int y)
{
    return _segments[y] ? _segments[y] : _segments[y] = new OldSegmentList();
}

} // namespace gepard
