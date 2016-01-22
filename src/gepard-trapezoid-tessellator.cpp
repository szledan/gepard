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

#include "config.h"
#include "gepard-trapezoid-tessellator.h"

#include <list>
#include <math.h>
#include <set>

namespace gepard {

/* SegmentApproximator */

void SegmentApproximator::insertSegment(FloatPoint from, FloatPoint to)
{
    ASSERT(from.y != to.y);

    Segment segment(from, to);

    // Update bounding-box.
    _boundingBox.stretch(segment.from);
    _boundingBox.stretch(segment.to);

    // Insert segment.
    const int topY = segment.topY();
    const int bottomY = segment.bottomY();

    if (_segments.find(topY) == _segments.end()) {
        _segments.emplace(topY, new SegmentList()).first->second->push_front(segment);
        // TODO: log: std::cout << *(_segments[topY].begin()) << std::endl;
    } else {
        _segments[topY]->push_front(segment);
    }

    if (_segments.find(bottomY) == _segments.end()) {
        _segments.emplace(bottomY, new SegmentList());
    }
}

void SegmentApproximator::insertLine(FloatPoint from, FloatPoint to)
{
    from.x *= _kAntiAliasLevel;
    from.y = floor(from.y * _kAntiAliasLevel);
    to.x *= _kAntiAliasLevel;
    to.y = floor(to.y * _kAntiAliasLevel);

    if (from.y == to.y)
        return;

    insertSegment(from, to);
}

bool SegmentApproximator::quadCurveIsLineSegment(FloatPoint points[])
{
    const Float x0 = points[0].x;
    const Float y0 = points[0].y;
    const Float x1 = points[1].x;
    const Float y1 = points[1].y;
    const Float x2 = points[2].x;
    const Float y2 = points[2].y;

    const Float dt = abs((x2 - x0) * (y0 - y1) - (x0 - x1) * (y2 - y0));

    if (dt > _kTolerance)
        return false;

    Float minX, minY, maxX, maxY;

    if (x0 < x2) {
        minX = x0 - _kTolerance;
        maxX = x2 + _kTolerance;
    } else {
        minX = x2 - _kTolerance;
        maxX = x0 + _kTolerance;
    }

    if (y0 < y2) {
        minY = y0 - _kTolerance;
        maxY = y2 + _kTolerance;
    } else {
        minY = y2 - _kTolerance;
        maxY = y0 + _kTolerance;
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

void SegmentApproximator::insertQuadCurve(const FloatPoint from, const FloatPoint control, const FloatPoint to)
{
    // De Casteljau algorithm.
    const int max = 16 * 2 + 1;
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

bool SegmentApproximator::curveIsLineSegment(FloatPoint points[])
{
    const Float x0 = points[0].x;
    const Float y0 = points[0].y;
    const Float x1 = points[1].x;
    const Float y1 = points[1].y;
    const Float x2 = points[2].x;
    const Float y2 = points[2].y;
    const Float x3 = points[3].x;
    const Float y3 = points[3].y;

    const Float dt1 = abs((x3 - x0) * (y0 - y1) - (x0 - x1) * (y3 - y0));
    const Float dt2 = abs((x3 - x0) * (y0 - y2) - (x0 - x2) * (y3 - y0));

    if (dt1 > _kTolerance || dt2 > _kTolerance)
        return false;

    Float minX, minY, maxX, maxY;

    if (x0 < x3) {
        minX = x0 - _kTolerance;
        maxX = x3 + _kTolerance;
    } else {
        minX = x3 - _kTolerance;
        maxX = x0 + _kTolerance;
    }

    if (y0 < y3) {
        minY = y0 - _kTolerance;
        maxY = y3 + _kTolerance;
    } else {
        minY = y3 - _kTolerance;
        maxY = y0 + _kTolerance;
    }

    return !(x1 < minX || x1 > maxX || y1 < minY || y1 > maxY
        || x2 < minX || x2 > maxX || y2 < minY || y2 > maxY);
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

void SegmentApproximator::insertBezierCurve(const FloatPoint from, const FloatPoint control1, const FloatPoint control2, const FloatPoint to)
{
    // De Casteljau algorithm.
    const int max = 16 * 3 + 1;
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

int SegmentApproximator::calculateSegments(const Float& angle, const Float& radius)
{
    const Float epsilon = _kTolerance / radius;
    Float angleSegment;
    Float error;

    int i = 1;
    do {
        angleSegment = piFloat / i++;
        error = 2.0 / 27.0 * pow(sin(angleSegment / 4.0), 6) / pow(cos(angleSegment / 4.0), 2);
    } while (error > epsilon);

    return ceil(abs(angle) / angleSegment);
}

void SegmentApproximator::arcToCurve(FloatPoint result[], const Float& startAngle, const Float& endAngle)
{
    const Float sinStartAngle = sin(startAngle);
    const Float cosStartAngle = cos(startAngle);
    const Float sinEndAngle = sin(endAngle);
    const Float cosEndAngle = cos(endAngle);

    const Float height = 4.0 / 3.0 * tan((endAngle - startAngle) / 4.0);

    result[0].x = cosStartAngle - height * sinStartAngle;
    result[0].y = sinStartAngle + height * cosStartAngle;
    result[1].x = cosEndAngle + height * sinEndAngle;
    result[1].y = sinEndAngle - height * cosEndAngle;
    result[2].x = cosEndAngle;
    result[2].y = sinEndAngle;
}

void SegmentApproximator::insertArc(const FloatPoint lastEndPoint, const ArcElement* arcElement)
{
    Float startAngle = arcElement->startAngle;
    const Float endAngle = arcElement->endAngle;
    const bool antiClockwise = arcElement->antiClockwise;

    FloatPoint startPoint = FloatPoint(cos(startAngle) * arcElement->radius.x, sin(startAngle) * arcElement->radius.y) + arcElement->center;
    insertLine(lastEndPoint, startPoint);

    ASSERT(startAngle != endAngle);

    const Float deltaAngle = antiClockwise ? startAngle - endAngle : endAngle - startAngle;

    const int segments = calculateSegments(deltaAngle, max(arcElement->radius.x * 2, arcElement->radius.y * 2));
    Float step = deltaAngle / segments;

    if (antiClockwise)
        step = -step;

    FloatPoint bezierPoints[3];
    for (int i = 0; i < segments; i++, startAngle += step) {
        arcToCurve(bezierPoints, startAngle, (i == segments - 1) ? endAngle : startAngle + step);
        bezierPoints[0] = bezierPoints[0] * arcElement->radius + arcElement->center;
        bezierPoints[1] = bezierPoints[1] * arcElement->radius + arcElement->center;
        if (i == segments - 1) {
            bezierPoints[2] = arcElement->to;
        } else {
            bezierPoints[2] = bezierPoints[2] * arcElement->radius + arcElement->center;
        }
        insertBezierCurve(startPoint, bezierPoints[0], bezierPoints[1], bezierPoints[2]);
        startPoint = bezierPoints[2];
    }
}

void SegmentApproximator::printSegements()
{
    for (auto& currentSegments : _segments) {
//        std::cout << currentSegments.first << ": ";
        SegmentList currentList = *(currentSegments.second);
        for (Segment& segment : currentList) {
//            std::cout << segment << " ";
            std::cout << segment << std::endl;
        }
//        std::cout << std::endl;
    }
}

inline void SegmentApproximator::splitSegments()
{
    for (SegmentTree::iterator currentSegments = _segments.begin(); currentSegments != _segments.end(); ++currentSegments) {
        SegmentTree::iterator newSegments = currentSegments;
        ++newSegments;
        if (newSegments != _segments.end()) {
            SegmentList* currentList = currentSegments->second;
            SegmentList* newList = newSegments->second;
            int splitY = floor(newSegments->first);
            ASSERT(currentList && newList);

            for (Segment& segment : *currentList) {
                if (segment.isOnSegment(splitY)) {
                    newList->push_front(segment.splitSegment(splitY));
                }
            }
        }
    }
}

SegmentList* SegmentApproximator::segments()
{
    // 1. Split segments with all y lines.
    splitSegments();

    // 2. Find all intersection points.
    std::set<int> ys;
    for (auto& currentSegments : _segments) {
        SegmentList* currentList = currentSegments.second;
        currentList->sort();
        SegmentList::iterator currentSegment = currentList->begin();
        for (SegmentList::iterator segment = currentSegment; currentSegment != currentList->end(); ++segment) {
            if (segment != currentList->end()) {
                const Float y = currentSegment->computeIntersectionY(&(*segment));
                if (y != NAN && y != INFINITY) {
                    const int intersectionY = floor(y);
                    ys.insert(intersectionY);
                    if (intersectionY != y) {
                        ys.insert(intersectionY + 1);
                    }
                }
            } else {
                segment = ++currentSegment;
            }
        }
    }

    for (auto y : ys) {
        _segments.emplace(y, new SegmentList());
    }

    // 3. Split segments with all y lines.
    splitSegments();

    // 4. Merge and sort all segment list.
    // TODO: use MovePtr:
    SegmentList* segments = new SegmentList();
    for (SegmentTree::iterator currentSegments = _segments.begin(); currentSegments != _segments.end(); ++currentSegments) {
        SegmentList* currentList = currentSegments->second;
        currentList->sort();

        bool needSorting = false;
        // 4.a Fix intersection pairs.
        for (SegmentList::iterator segment = currentList->begin(); segment != currentList->end(); ++segment) {
            ASSERT(segment->to.y - segment->from.y >= 1.0);
//if (segment->to.y - segment->from.y <= 1.0) /* TODO: it is a bug? (testcase: fillmode: EvenOdd, test: "ERD" part from test of "Erdély") */
            for (SegmentList::iterator furtherSegment = segment; furtherSegment != currentList->end() ; ++furtherSegment) {
                ASSERT(segment->from.y == furtherSegment->from.y);
                ASSERT(segment->to.y == furtherSegment->to.y);
                ASSERT(furtherSegment->from.x >= segment->from.x)
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
        if (needSorting) {
            currentList->sort();
        }

        // 4.b Merge segment lists.
        segments->merge(*currentList);
    }

    // 5. Return independent segments.
    return segments;
}

/* TrapezoidTessellator */

TrapezoidList TrapezoidTessellator::trapezoidList()
{
    PathElement* element = _path->pathData().firstElement();

    if (!element)
        return TrapezoidList();

    ASSERT(element->type == PathElementTypes::MoveTo);

    const Float subPixelPrecision = 1.0;
    SegmentApproximator segmentApproximator(_antiAliasingLevel, subPixelPrecision);
    FloatPoint from;
    FloatPoint to = element->to;
    FloatPoint lastMoveTo = to;

    // 1. Insert path elements.
    do {
        from = to;
        element = element->next;
        to = element->to;
        switch (element->type) {
        case PathElementTypes::MoveTo: {
            segmentApproximator.insertLine(from, lastMoveTo);
            lastMoveTo = to;
            break;
        }
        case PathElementTypes::LineTo: {
            segmentApproximator.insertLine(from, to);
            break;
        }
        case PathElementTypes::CloseSubpath: {
            segmentApproximator.insertLine(from, lastMoveTo);
            lastMoveTo = to;
            break;
        }
        case PathElementTypes::QuadraticCurve: {
            QuadraticCurveToElement* qe = reinterpret_cast<QuadraticCurveToElement*>(element);
            segmentApproximator.insertQuadCurve(from, qe->control, to);
            break;
        }
        case PathElementTypes::BezierCurve: {
            BezierCurveToElement* be = reinterpret_cast<BezierCurveToElement*>(element);
            segmentApproximator.insertBezierCurve(from, be->control1, be->control2, to);
            break;
        }
        case PathElementTypes::Arc: {
            ArcElement* ae = reinterpret_cast<ArcElement*>(element);
            segmentApproximator.insertArc(from, ae);
            break;
        }
        case PathElementTypes::Undefined:
        default:
            // unreachable
            break;
        }
    } while (element->next);

    segmentApproximator.insertLine(lastMoveTo, element->to);

    // 2. Use approximator to generate the list of segments.
    SegmentList* segmentList = segmentApproximator.segments();
    TrapezoidList trapezoids;

    // 3. Generate trapezoids.
    const Float denom = _antiAliasingLevel * 1 + 0;
    if (segmentList) {
        Trapezoid trapezoid;
        int fill = 0;
        bool isInFill = false;
        // TODO: ASSERTs for wrong segments.
        for (Segment& segment : *segmentList) {
            if (segment.from.y == segment.to.y)
                continue;
            if (fillRule() == EvenOdd) {
                fill = !fill;
            } else {
                fill += segment.direction;
            }

            if (fill) {
                if (!isInFill) {
                    trapezoid.topY = (fixPrecision(segment.topY() / denom));
                    trapezoid.bottomY = (fixPrecision(segment.bottomY() / denom));
                    trapezoid.topLeftX = (fixPrecision(segment.from.x) / denom);
                    trapezoid.bottomLeftX = (fixPrecision(segment.to.x) / denom);
                    trapezoid.leftId = segment.id;
                    trapezoid.leftSlope = segment.realSlope;
                    if (trapezoid.topY != trapezoid.bottomY)
                        isInFill = true;
                }
            } else {
                // TODO: Horizontal merge trapezoids.
                trapezoid.topRightX = (fixPrecision(segment.from.x) / denom);
                trapezoid.bottomRightX = (fixPrecision(segment.to.x) / denom);
                trapezoid.rightId = segment.id;
                trapezoid.rightSlope = segment.realSlope;
                if (trapezoid.topY != trapezoid.bottomY) {
                    trapezoids.push_back(trapezoid);
                }
                isInFill = false;
            }
            ASSERT(trapezoid.topY == (fixPrecision(segment.topY() / denom)));
        }

        delete segmentList;

        // TODO: check the boundingBox calculation:
        // NOTE:  maxX = (maxX + (_antiAliasingLevel - 1)) / _antiAliasingLevel;
        _boundingBox.minX = (fixPrecision(segmentApproximator.boundingBox().minX) / _antiAliasingLevel);
        _boundingBox.minY = (fixPrecision(segmentApproximator.boundingBox().minY) / _antiAliasingLevel);
        _boundingBox.maxX = (fixPrecision(segmentApproximator.boundingBox().maxX) / _antiAliasingLevel);
        _boundingBox.maxY = (fixPrecision(segmentApproximator.boundingBox().maxY) / _antiAliasingLevel);
    }

    trapezoids.sort();

    // 4. Vertical merge trapezoids.
    // TODO: use MovePtr:
    TrapezoidList trapezoidList;
    for (TrapezoidList::iterator current = trapezoids.begin(); current != trapezoids.end(); ++current) {
        const Float bottomY = current->bottomY;
        TrapezoidList::iterator ft = current;
        for (; (ft != trapezoids.end() && ft->bottomY == bottomY); ++ft);

        ASSERT(current->leftId != 0 && current->rightId != 0);
        ASSERT(current->leftSlope != NAN && current->rightSlope != NAN);
        for (TrapezoidList::iterator further = current; (further != trapezoids.end() && further->topY <= bottomY); ++further) {
            ASSERT(further->leftId != 0 && further->rightId != 0);
            ASSERT(further->leftSlope != NAN && further->rightSlope != NAN);
            if (further->topY == current->bottomY && current->isMergableInTo(&*further)) {
                further->topY = current->topY;
                further->topLeftX = current->topLeftX;
                further->topRightX = current->topRightX;
                current->leftId = 0;
                current->rightId = 0;
                break;
            }
        }
        if (current->leftId)
            trapezoidList.push_back(*current);
    }

    return trapezoidList;
}

} // namespace gepard
