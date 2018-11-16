/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2015-2018, Szilard Ledan <szledan@gmail.com>
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

#include "gepard-trapezoid-tessellator.h"

#include "gepard-defs.h"
#include "gepard-float-point.h"
#include "gepard-float.h"
#include "gepard-logging.h"
#include "gepard-transform.h"
#include <cmath>
#include <list>
#include <set>

namespace gepard {

/* Segment */

static unsigned s_segmentIds = 0;

Segment::Segment(FloatPoint from, FloatPoint to, unsigned sameId, Float slope)
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

const int Segment::topY() const
{
    return std::floor(this->from.y);
}

const int Segment::bottomY() const
{
    return std::floor(this->to.y);
}

const Float Segment::slopeInv() const
{
    return (this->to.x - this->from.x) / (this->to.y - this->from.y);
}

const Float Segment::factor() const
{
    return this->slopeInv() * this->from.y - this->from.x;
}

const bool Segment::isOnSegment(const Float y) const
{
    return y < this->to.y && y > this->from.y;
}

const Segment Segment::splitSegment(const Float y)
{
    GD_ASSERT(this->from.y < this->to.y);
    GD_ASSERT(y > this->from.y && y < this->to.y);

    const Float x = this->slopeInv() * (y - this->from.y) + this->from.x;
    FloatPoint to = this->to;
    this->to = FloatPoint(x, y);
    FloatPoint newPoint = this->to;

    if (this->direction == Negative) {
        newPoint = to;
        to = this->to;
    }

    GD_ASSERT(this->from.y != newPoint.y);
    GD_ASSERT(newPoint.y != to.y);

    return Segment(newPoint, to, this->id, this->realSlope);
}

const bool Segment::computeIntersectionY(Segment* segment, Float& y) const
{
    if (this == segment)
        return false;

    if (this->from.x != segment->from.x && this->to.x != segment->to.x) {
        Float denom = (this->slopeInv() - segment->slopeInv());
        if (denom) {
            y = (this->factor() - segment->factor()) / denom;
        }
        if (!isOnSegment(y)) {
            y = INFINITY;
            return false;
        }
    } //! \todo: else: y is NaN.
    return true;
}

std::ostream& operator<<(std::ostream& os, const Segment& s)
{
    return os << s.from << ((s.direction < 0) ? "<" : ((s.direction > 0) ? ">" : "=")) << s.to;
}

bool operator<(const Segment& lhs, const Segment& rhs)
{
    GD_ASSERT(lhs.from <= lhs.to && rhs.from <= rhs.to);
    return (lhs.from < rhs.from) || (lhs.from == rhs.from && lhs.to < rhs.to);
}

bool operator==(const Segment& lhs, const Segment& rhs)
{
    return (lhs.from == rhs.from) && (lhs.to == rhs.to);
}

bool operator<=(const Segment& lhs, const Segment& rhs)
{
    return (lhs < rhs) || (lhs == rhs);
}

/* SegmentApproximator */

SegmentApproximator::SegmentApproximator(const int antiAliasLevel, const Float factor)
    : kAntiAliasLevel(antiAliasLevel > 0 ? antiAliasLevel : GD_ANTIALIAS_LEVEL)
    , kTolerance((factor > 0.0 ? factor : 1.0 ) / ((Float)kAntiAliasLevel))
{
}

SegmentApproximator::~SegmentApproximator()
{
    for (SegmentTree::iterator it = _segments.begin(); it != _segments.end(); ++it) {
        delete it->second;
    }
}

void SegmentApproximator::insertLine(const FloatPoint& from, const FloatPoint& to)
{
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

void SegmentApproximator::printSegments()
{
    for (auto& currentSegments : _segments) {
        SegmentList currentList = *(currentSegments.second);
        for (Segment& segment : currentList) {
            std::cout << segment << std::endl;
        }
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
            int splitY = std::floor(newSegments->first);
            GD_ASSERT(currentList && newList);
            GD_ASSERT(currentSegments->first < newSegments->first);

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
    // Split segments with all y lines.
    splitSegments();

    // Find all intersection points.
    std::set<int> ys;
    for (auto& currentSegments : _segments) {
        SegmentList* currentList = currentSegments.second;
        currentList->sort();
        SegmentList::iterator currentSegment = currentList->begin();
        for (SegmentList::iterator segment = currentSegment; currentSegment != currentList->end(); ++segment) {
            if (segment != currentList->end()) {
                Float y;
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
    SegmentList* segments = new SegmentList();
    for (SegmentTree::iterator currentSegments = _segments.begin(); currentSegments != _segments.end(); ++currentSegments) {
        SegmentList* currentList = currentSegments->second;
        currentList->sort();

        bool needSorting = false;
        // Fix intersection pairs.
        for (SegmentList::iterator segment = currentList->begin(); segment != currentList->end(); ++segment) {
            GD_ASSERT(segment->to.y - segment->from.y >= 1.0);
            if (segment->to.y - segment->from.y == 1.0) {
                for (SegmentList::iterator furtherSegment = segment; furtherSegment != currentList->end() ; ++furtherSegment) {
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

    Segment segment(from, to);

    // Update bounding-box.
    _boundingBox.stretch(segment.from);
    _boundingBox.stretch(segment.to);

    // Insert segment.
    const int topY = segment.topY();
    const int bottomY = segment.bottomY();

    insertSegmentList(topY)->push_front(segment);
    insertSegmentList(bottomY);
}

SegmentList* SegmentApproximator::insertSegmentList(const int y)
{
    return _segments[y] ? _segments[y] : _segments[y] = new SegmentList();
}

/* Trapezoid */

const bool Trapezoid::isMergableInTo(const Trapezoid* other) const
{
    GD_ASSERT(this->bottomY == other->topY);

    if (this->bottomLeftX == other->topLeftX && this->bottomRightX == other->topRightX) {
        if (this->leftId == other->leftId && this->rightId == other->rightId)
            return true;

        if (this->leftSlope == other->leftSlope && this->rightSlope == other->rightSlope)
            return true;
    }

    return false;
}

bool operator<(const Trapezoid& lhs, const Trapezoid& rhs)
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

bool operator==(const Trapezoid& lhs, const Trapezoid& rhs)
{
    return lhs.topY == rhs.topY && lhs.topLeftX == rhs.topLeftX && lhs.topRightX == rhs.topRightX
            && lhs.bottomY == rhs.bottomY && lhs.bottomLeftX == rhs.bottomLeftX &&  lhs.bottomRightX == rhs.bottomRightX;
}

bool operator<=(const Trapezoid& lhs, const Trapezoid& rhs)
{
    return lhs < rhs || lhs == rhs;
}

/* TrapezoidTessellator */

TrapezoidTessellator::TrapezoidTessellator(PathData& pathData, FillRule fillRule, int antiAliasingLevel)
    : _pathData(pathData)
    , _fillRule(fillRule)
    , _antiAliasingLevel(antiAliasingLevel)
{
}

const TrapezoidList TrapezoidTessellator::trapezoidList(const GepardState& state)
{
    PathElement* element = _pathData.firstElement();

    if (!element || !element->next)
        return TrapezoidList();

    GD_ASSERT(element->type == PathElementTypes::MoveTo);

    const Float subPixelPrecision = 1.0;
    SegmentApproximator segmentApproximator(_antiAliasingLevel, subPixelPrecision);
    FloatPoint from;
    FloatPoint to = element->to;
    FloatPoint lastMoveTo = to;
    Transform at = state.transform;

    // 1. Insert path elements.
    do {
        from = to;
        element = element->next;
        to = element->to;
        switch (element->type) {
        case PathElementTypes::MoveTo: {
            segmentApproximator.insertLine(at.apply(from), at.apply(lastMoveTo));
            lastMoveTo = to;
            break;
        }
        case PathElementTypes::LineTo: {
            segmentApproximator.insertLine(at.apply(from), at.apply(to));
            break;
        }
        case PathElementTypes::CloseSubpath: {
            segmentApproximator.insertLine(at.apply(from), at.apply(lastMoveTo));
            lastMoveTo = to;
            break;
        }
        case PathElementTypes::QuadraticCurve: {
            QuadraticCurveToElement* qe = reinterpret_cast<QuadraticCurveToElement*>(element);
            segmentApproximator.insertQuadCurve(at.apply(from), at.apply(qe->control), at.apply(to));
            break;
        }
        case PathElementTypes::BezierCurve: {
            BezierCurveToElement* be = reinterpret_cast<BezierCurveToElement*>(element);
            segmentApproximator.insertBezierCurve(at.apply(from), at.apply(be->control1), at.apply(be->control2), at.apply(to));
            break;
        }
        case PathElementTypes::Arc: {
            ArcElement* ae = reinterpret_cast<ArcElement*>(element);
            segmentApproximator.insertArc(at.apply(from), ae, at);
            break;
        }
        case PathElementTypes::Undefined:
        default:
            // unreachable
            break;
        }
    } while (element->next != nullptr);

    segmentApproximator.insertLine(at.apply(element->to), at.apply(lastMoveTo));

    // 2. Use approximator to generate the list of segments.
    SegmentList* segmentList = segmentApproximator.segments();
    TrapezoidList trapezoids;

    // 3. Generate trapezoids.
    const Float denom = _antiAliasingLevel * 1 + 0;
    if (segmentList) {
        Trapezoid trapezoid;
        int fill = 0;
        bool isInFill = false;
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
            //! \todo(szledan): we need this assert in the future,
            //! but the TT doesn't work correctly now with that.
            // GD_ASSERT(trapezoid.topY == (fixPrecision(segment.topY() / denom)));
        }

        delete segmentList;

        //! \todo(szledan): check the boundingBox calculation:
        // NOTE:  maxX = (maxX + (_antiAliasingLevel - 1)) / _antiAliasingLevel;
        _boundingBox.minX = (fixPrecision(segmentApproximator.boundingBox().minX) / _antiAliasingLevel);
        _boundingBox.minY = (fixPrecision(segmentApproximator.boundingBox().minY) / _antiAliasingLevel);
        _boundingBox.maxX = (fixPrecision(segmentApproximator.boundingBox().maxX) / _antiAliasingLevel);
        _boundingBox.maxY = (fixPrecision(segmentApproximator.boundingBox().maxY) / _antiAliasingLevel);
    }

    trapezoids.sort();

    // 4. Vertical merge trapezoids.
    //! \todo(szledan): use MovePtr:
    TrapezoidList trapezoidList;
    for (TrapezoidList::iterator current = trapezoids.begin(); current != trapezoids.end(); ++current) {
        const Float bottomY = current->bottomY;
        TrapezoidList::iterator ft = current;
        for (; (ft != trapezoids.end() && ft->bottomY == bottomY); ++ft);

        GD_ASSERT(current->leftId != 0 && current->rightId != 0);
        GD_ASSERT(current->leftSlope != NAN && current->rightSlope != NAN);
        for (TrapezoidList::iterator further = current; (further != trapezoids.end() && further->topY <= bottomY); ++further) {
            GD_ASSERT(further->leftId != 0 && further->rightId != 0);
            GD_ASSERT(further->leftSlope != NAN && further->rightSlope != NAN);
            if (further->topY == current->bottomY && current->isMergableInTo(&*further)) {
                further->topY = current->topY;
                further->topLeftX = current->topLeftX;
                further->topRightX = current->topRightX;
                current->leftId = 0;
                current->rightId = 0;
                break;
            }
        }
        if (current->leftId) {
            trapezoidList.push_back(*current);
        }
    }

    return trapezoidList;
}

} // namespace gepard
