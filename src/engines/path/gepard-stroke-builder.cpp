/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2018, Szilard Ledan <szledan@gmail.com>
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
 */

#include "gepard-stroke-builder.h"

#include "gepard-defs.h"
#include "gepard-float-point.h"
#include "gepard-float.h"
#include "gepard-line-types.h"
#include "gepard-path.h"
#include "gepard-trapezoid-tessellator.h"
#include <cmath>

namespace gepard {

static inline FloatPoint normalSize(const FloatPoint& size, Float halfWidth)
{
    const FloatPoint normalVector = size.normal();
    Float lenght = normalVector.length();

    if (!lenght)
        return FloatPoint(0.0, 0.0);

    return (halfWidth / lenght) * normalVector;
}

void StrokePathBuilder::LineAttributes::set(const FloatPoint& from, const FloatPoint& to, const Float halfWidth)
{
    location = from;
    vector = to - from;

    if (vector.isZero())
        return;

    length = vector.length();
    unit = vector / length;

    thicknessOffsets.x = halfWidth * unit.x;
    thicknessOffsets.y = halfWidth * unit.y;

    startTop.set(from.x + thicknessOffsets.y, from.y - thicknessOffsets.x);
    startBottom.set(from.x - thicknessOffsets.y, from.y + thicknessOffsets.x);
    endBottom.set(to.x - thicknessOffsets.y, to.y + thicknessOffsets.x);
    endTop.set(to.x + thicknessOffsets.y, to.y - thicknessOffsets.x);
}

StrokePathBuilder::LineAttributes::Direction StrokePathBuilder::LineAttributes::vectorCompare(const LineAttributes* line) const
{
    const Float direction = unit.cross(line->unit);

    if (direction > 0.0)
        return Direction::Positive;
    if (direction < 0.0)
        return Direction::Negative;

    if (!(unit.y + line->unit.y))
        return Direction::Reverse;

    return Direction::Same;
}

StrokePathBuilder::StrokePathBuilder(const Float width, const Float miterLimit, const LineJoinType joinMode, const LineCapType lineCap)
    : _hasShapeFirstLine(false)
    , _halfWidth(width / 2.0)
    , _miterLimitSquared(miterLimit * miterLimit)
    , _joinMode(joinMode)
    , _lineCap(lineCap)
{
    _shapeFirstLine = &_lines[0];
    _lastLine = &_lines[1];
    _currentLine = &_lines[2];
    _lastLine->next = _currentLine;
    _currentLine->next = _lastLine;
}

void StrokePathBuilder::convertStrokeToFill(const PathData* path)
{
    PathElement* element = path->firstElement();

    FloatPoint from;
    while (element) {
        FloatPoint to = element->to;

        switch (element->type) {
        case MoveTo:
            addMoveToShape(to);
            break;
        case CloseSubpath:
            addCloseSubpathShape(from, reinterpret_cast<CloseSubpathElement*>(element));
            break;
        case LineTo:
            addLineShape(from, reinterpret_cast<LineToElement*>(element));
            break;
        case QuadraticCurve:
            addBezierCurveShape(from, reinterpret_cast<QuadraticCurveToElement*>(element));
            break;
        case BezierCurve:
            addBezierCurveShape(from, reinterpret_cast<BezierCurveToElement*>(element));
            break;
        case Arc:
            addArcShape(from, reinterpret_cast<ArcElement*>(element));
            break;
        case Undefined:
        default:
            GD_ASSERT(false);
            break;
        }

        from = to;
        element = element->next;
    }

    addCapShapeIfNeeded();
}

inline void StrokePathBuilder::addMoveToShape(const FloatPoint& to)
{
    addCapShapeIfNeeded();
    _shapeFirstLine->set(to, to, _halfWidth);
    _currentLine->set(to, to, _halfWidth);
    _lastLine->set(to, to, _halfWidth);
    _hasShapeFirstLine = false;
}

inline void StrokePathBuilder::addLineShape(const FloatPoint& start, const LineToElement* element)
{
    if (start == element->to)
        return;

    _currentLine->set(start, element->to, _halfWidth);
    if (!_hasShapeFirstLine) {
        _shapeFirstLine->set(start, element->to, _halfWidth);
        _hasShapeFirstLine = true;
    } else {
        addJoinShape(_lastLine, _currentLine);
    }

    addQuadShape(_currentLine->startTop, _currentLine->startBottom, _currentLine->endBottom, _currentLine->endTop);

    setCurrentLineAttribute();
}

inline void StrokePathBuilder::addCloseSubpathShape(const FloatPoint& start, const CloseSubpathElement* element)
{
    if (!_hasShapeFirstLine || start == element->to)
        return;

    _currentLine->set(start, element->to, _halfWidth);
    addJoinShape(_lastLine, _currentLine);
    addJoinShape(_currentLine, _shapeFirstLine);

    addQuadShape(_currentLine->startTop, _currentLine->startBottom, _currentLine->endBottom, _currentLine->endTop);

    _hasShapeFirstLine = false;
}

void StrokePathBuilder::bezierCurveShape(const FloatPoint& start, const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& end)
{
    // De Casteljau algorithm.
    static const int max = 16 * 3 + 1;
    FloatPoint buffer[max];
    FloatPoint* points = buffer;

    FloatPoint startNormal, halfNormal, endNormal;
    FloatPoint involuteStart, involuteHalf, involuteEnd;
    FloatPoint evoluteStart, evoluteHalf, evoluteEnd;

    points[0] = start;
    points[1] = control1;
    points[2] = control2;
    points[3] = end;

    do {
        _segmentApproximator.splitCubeCurve(points);

        if (points[0] != points[1]) {
            startNormal = normalSize(points[1] - points[0], _halfWidth);
        } else if (points[0] != points[2]) {
            startNormal = normalSize(points[2] - points[0], _halfWidth);
        } else {
            startNormal = normalSize(points[3] - points[0], _halfWidth);
        }

        if (points[3] != points[4]) {
            halfNormal = normalSize(points[4] - points[3], _halfWidth);
        } else if (points[3] != points[5]) {
            halfNormal = normalSize(points[5] - points[3], _halfWidth);
        } else {
            halfNormal = normalSize(points[6] - points[3], _halfWidth);
        }

        if (points[6] != points[5]) {
            endNormal = normalSize(points[6] - points[5], _halfWidth);
        } else if (points[6] != points[4]) {
            endNormal = normalSize(points[6] - points[4], _halfWidth);
        } else {
            endNormal = normalSize(points[6] - points[3], _halfWidth);
        }

        involuteStart = points[0] - startNormal;
        involuteHalf = points[3] - halfNormal;
        involuteEnd = points[6] - endNormal;

        if ((points[2] == points[3]) && (points[3] == points[4])) {
            FloatPoint halfWidth = normalSize(points[3] - points[1], _halfWidth);
            FloatPoint miter = points[3] + FloatPoint(-halfWidth).normal();
            addRoundShape(points[3], points[3] + halfWidth, miter + halfWidth, miter);
            addRoundShape(points[3], miter, miter - halfWidth, points[3] - halfWidth);
        } else if (((points[6] - points[0]).lengthSquared() < _segmentApproximator.kTolerance)
            && ((points[5] - points[0]).lengthSquared() < _segmentApproximator.kTolerance)
            && ((points[1] - points[0]).lengthSquared() < _segmentApproximator.kTolerance)) {
            FloatPoint miter = points[0] + FloatPoint(-startNormal).normal();
            addRoundShape(points[0], points[0] + startNormal, miter + startNormal, miter);
            addRoundShape(points[0], miter, miter - startNormal, points[0] - startNormal);
            points -= 3;
            continue;
        }

        if (_segmentApproximator.curveIsLineSegment(involuteStart, involuteHalf, involuteEnd)) {
            evoluteStart = points[0] + startNormal;
            evoluteHalf = points[3] + halfNormal;
            evoluteEnd = points[6] + endNormal;

            if(_segmentApproximator.curveIsLineSegment(evoluteStart, evoluteHalf, evoluteEnd)) {
                addQuadShape(evoluteStart, involuteStart, involuteHalf, evoluteHalf);
                addQuadShape(evoluteHalf, involuteHalf, involuteEnd, evoluteEnd);
                points -= 3;
                continue;
            }
        }

        points += 3;

        if (points >= buffer + max - 7) {
            // This recursive code path is rarely executed.
            bezierCurveShape(points[0], points[1], points[2], points[3]);
            points -= 3;
        }
    } while (points >= buffer);
}

inline void StrokePathBuilder::addBezierCurveShape(const FloatPoint& start, const QuadraticCurveToElement* element)
{
    static const Float twoThird = static_cast<Float>(2) / 3;
    const FloatPoint control1(twoThird * (element->control.x - start.x) + start.x, twoThird * (element->control.y - start.y) + start.y);
    const FloatPoint control2(twoThird * (element->control.x - element->to.x) + element->to.x, twoThird * (element->control.y - element->to.y) + element->to.y);
    const BezierCurveToElement cubicCurve(control1, control2, element->to);
    addBezierCurveShape(start, &cubicCurve);
}

inline void StrokePathBuilder::addBezierCurveShape(const FloatPoint& start, const BezierCurveToElement* element)
{
    const FloatPoint control1 = element->control1;
    const FloatPoint control2 = element->control2;
    const FloatPoint end = element->to;

    FloatPoint to = control1;
    FloatPoint from = control2;
    if (control1 == start) {
        if (control1 == control2) {
            if (control1 == end)
                return;
            to = end;
        } else {
            to = control2;
        }
    }
    if (control2 == end) {
        if (control2 == control1) {
            from = start;
        } else {
            from = control1;
        }
    }
    // Set the continuous vector to join or cap shape method.
    _currentLine->set(start, to, _halfWidth);

    if (!_hasShapeFirstLine) {
        _shapeFirstLine->set(start, to, _halfWidth);
        _hasShapeFirstLine = true;
    } else {
        addJoinShape(_lastLine, _currentLine);
    }

    // Set the continuous vector to join or cap shape method.
    _currentLine->set(from, end, _halfWidth);
    setCurrentLineAttribute();

    // TODO Empty curve: GD_ASSERT()
    bezierCurveShape(start, control1, control2, end);
}

inline void StrokePathBuilder::addArcShape(const FloatPoint& start, const ArcElement* element)
{
    const Transform& at = element->transform;
    const FloatPoint centerPoint(at.apply(element->center));
    const FloatPoint end(element->to);
    const Float direction = element->counterClockwise ? 1.0 : -1.0;
    const FloatPoint normalPoint = direction * FloatPoint(std::sin(element->startAngle), -std::cos(element->startAngle));

    // Set the continuous vector to join or cap shape method.
    _currentLine->set(start, start + normalPoint, _halfWidth);

    if (!_hasShapeFirstLine) {
        _shapeFirstLine->set(start, start + normalPoint, _halfWidth);
        _hasShapeFirstLine = true;
    } else {
        addJoinShape(_lastLine, _currentLine);
    }

    LineToElement lt(FloatPoint(centerPoint.x + element->radius.x * std::cos(element->startAngle), centerPoint.y + element->radius.y * sin(element->startAngle)));
    addLineShape(start, &lt);
    addJoinShape(_lastLine, _currentLine);

    // Set the continuous vector to join or cap shape method.
    _currentLine->set(end - direction * FloatPoint(std::sin(element->endAngle), -std::cos(element->endAngle)), end, _halfWidth);

    const FloatPoint firstRadius = element->radius + direction *  FloatPoint(_halfWidth, _halfWidth);
    const FloatPoint secondRadius = element->radius - direction * FloatPoint(_halfWidth, _halfWidth);

    const FloatPoint startPoint = FloatPoint(centerPoint.x + firstRadius.x * std::cos(element->startAngle), centerPoint.y + firstRadius.y * sin(element->startAngle));
    const FloatPoint endPoint = FloatPoint(centerPoint.x + secondRadius.x * std::cos(element->endAngle), centerPoint.y + secondRadius.y * sin(element->endAngle));

    _path.addMoveToElement(startPoint);
    _path.addArcElement(centerPoint, firstRadius, element->startAngle, element->endAngle, element->counterClockwise);
    _path.addLineToElement(endPoint);
    _path.addArcElement(centerPoint, secondRadius, element->endAngle, element->startAngle, !element->counterClockwise);
    _path.addCloseSubpathElement();

    setCurrentLineAttribute();
}

inline void StrokePathBuilder::addCapShapeIfNeeded()
{
    if (_hasShapeFirstLine) {
        addCapShape(_lineCap);
    }
}

inline void StrokePathBuilder::addTriangle(const FloatPoint& p0, const FloatPoint& p1, const FloatPoint& p2, const Float crossProduct)
{
    if (!crossProduct)
        return;

    _path.addMoveToElement(p0);
    if (crossProduct > 0) {
        _path.addLineToElement(p1);
        _path.addLineToElement(p2);
    } else {
        _path.addLineToElement(p2);
        _path.addLineToElement(p1);
    }
    _path.addCloseSubpathElement();
}

inline void StrokePathBuilder::addTriangle(const FloatPoint& p0, const FloatPoint& p1, const FloatPoint& p2)
{
    const Float crossProduct = (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);

    addTriangle(p0, p1, p2, -crossProduct);
}

inline void StrokePathBuilder::addQuadShape(const FloatPoint& p0, const FloatPoint& p1, const FloatPoint& p2, const FloatPoint& p3)
{
    const FloatPoint p1p0 = p1 - p0;
    const FloatPoint p2p0 = p2 - p0;
    const FloatPoint p3p0 = p3 - p0;

    // Handle the most frequent case first: no intersection.
    const Float p2p0Xp1p0 = p2p0.cross(p1p0);
    const Float p2p0Xp3p0 = p2p0.cross(p3p0);

    if (p2p0Xp1p0 * p2p0Xp3p0 < 0) {
        addTriangle(p0, p1, p2, p2p0Xp1p0);
        addTriangle(p0, p3, p2, p2p0Xp3p0);
        return;
    }

    const FloatPoint p3p1 = p3 - p1;
    const FloatPoint p2p1 = p2 - p1;
    const Float p3p1Xp0p1 = p3p1.cross(-p1p0);
    const Float p3p1Xp2p1 = p3p1.cross(p2p1);

    if (p3p1Xp0p1 * p3p1Xp2p1 < 0) {
        addTriangle(p1, p0, p3, p3p1Xp0p1);
        addTriangle(p1, p2, p3, p3p1Xp2p1);
        return;
    }

    // Intersection computation:
    //  intersectionPoint = p0 + k1 * p1p0
    //  intersectionPoint = p2 + k2 * p3p2
    //    where k1, k2 are constant values
    //
    // k1 and k2 (one of them is enough) can be computed from:
    //  p0.x + k1 * p1p0.x = p2.x + k2 * p3p2.x
    //  p0.y + k1 * p1p0.y = p2.y + k2 * p3p2.y

    const Float p1p0Xp2p0 = -p2p0Xp1p0;
    const Float p1p0Xp3p0 = p1p0.cross(p3p0);

    if (p1p0Xp2p0 * p1p0Xp3p0 < 0) {
        FloatPoint intersection;
        if (p3.y - p2.y) {
            const Float m = (p3.x - p2.x) / (p3.y - p2.y);
            const Float k1 = (m * (p2.y - p0.y) - p2.x + p0.x) / (m * (p1.y - p0.y) - p1.x + p0.x);
            intersection.set(p0.x + k1 * p1p0.x, p0.y + k1 * p1p0.y);
        } else if (p0.y - p1.y) {
            const Float m = (p0.x - p1.x) / (p0.y - p1.y);
            const Float k2 = (m * (p1.y - p3.y) - p1.x + p3.x) / (- p2.x + p3.x);
            intersection.set(p3.x + k2 * (p2.x - p3.x), p3.y + k2 * (p2.y - p3.y));
        }
        addTriangle(intersection, p2, p1, p1p0Xp2p0);
        addTriangle(p0, p3, intersection, p1p0Xp3p0);
        return;
    }

    FloatPoint intersection;
    if (p2.y - p1.y) {
        Float m = (p2.x - p1.x) / (p2.y - p1.y);
        Float k1 = (m * (p1.y - p0.y) - p1.x + p0.x) / (m * (p3.y - p0.y) - p3.x + p0.x);
        intersection.set(p0.x + k1 * p3p0.x, p0.y + k1 * p3p0.y);
    } else if (p3.y - p0.y) {
        Float m = (p2.x - p1.x) / (p3.y - p0.y);
        Float k2 = (m * (p0.y - p1.y) - p0.x + p1.x) / (- p2.x + p1.x);
        intersection.set(p1.x + k2 * (p2p1.x), p1.y + k2 * (p2p1.y));
    }
    addTriangle(p0, p1, intersection, p3p0.cross(p1p0));
    addTriangle(intersection, p2, p3, -p2p0Xp3p0);
}

inline void StrokePathBuilder::addRoundShape(const FloatPoint& location, const FloatPoint& from, const FloatPoint& miter, const FloatPoint& to)
{
    if (from == to || location == from || location == to)
        return;

    Float crossProduct = (from.x - location.x) * (to.y - location.y) - (from.y - location.y) * (to.x - location.x);

    _path.addMoveToElement(location);
    if (crossProduct < 0) {
        _path.addLineToElement(from);
        _path.addArcToElement(miter, to, _halfWidth);
    } else {
        _path.addLineToElement(to);
        _path.addArcToElement(miter, from, _halfWidth);
    }
    _path.addCloseSubpathElement();
}

inline FloatPoint StrokePathBuilder::miterLength(const FloatPoint& u1, const FloatPoint& u2)
{
    const FloatPoint miterDirection(u1 + u2);
    const Float directionLength = miterDirection.length();

    GD_ASSERT(directionLength > 0 && directionLength < 2);

    // Calculating the miterLength:
    // miterLength = halfWidth / sin(phi / 2),
    // 2 * (sin(phi / 2) ^ 2) = 1 - cos(phi),
    // dotProduct = cos(phi) = u1 * u2. Then u1 and u2 are unit vectors.
    Float length = _halfWidth / std::sqrt((1 - u1.dot(u2)) * 0.5);

    return (length / directionLength) * miterDirection;
}

void StrokePathBuilder::addJoinShape(const LineAttributes* fromLine, const LineAttributes* toLine)
{
    if (!fromLine->length || !toLine->length)
        return;

    LineAttributes::Direction direction = fromLine->vectorCompare(toLine);
    if (direction == LineAttributes::Direction::Same)
        return;

    if (_joinMode == RoundJoin) {
        const FloatPoint unitFrom(fromLine->unit);
        const FloatPoint unitTo(-toLine->unit);

        if (direction == LineAttributes::Direction::Reverse) {
            addCapShape(RoundCap, true);
            return;
        }

        FloatPoint miterPoint(toLine->location + miterLength(unitFrom, unitTo));
        if (direction == LineAttributes::Direction::Negative) {
            addRoundShape(toLine->location, fromLine->endBottom, miterPoint, toLine->startBottom);
            return;
        }

        addRoundShape(toLine->location, toLine->startTop, miterPoint, fromLine->endTop);
        return;
    }

    if (direction == LineAttributes::Direction::Reverse)
        return;

    if (_joinMode == MiterJoin) {
        const FloatPoint unitFrom(fromLine->unit);
        const FloatPoint unitTo(-toLine->unit);

        // Check the miter-limit: miterLimit * miterLimit * (1 - cos(Phi)) >= 2.
        // Where dotProduct = cos(Phi) = unitFrom * unitTo.
        if (_miterLimitSquared * (1 - unitFrom.dot(unitTo)) >= 2) {
            FloatPoint miterPoint =  toLine->location + miterLength(unitFrom, unitTo);

            if (direction == LineAttributes::Direction::Negative) {
                addQuadShape(toLine->location, fromLine->endBottom, miterPoint, toLine->startBottom);
                return;
            }

            addQuadShape(toLine->location, toLine->startTop, miterPoint, fromLine->endTop);
            return;
        }
    }

    // The BevelJoin mode.
    if (direction == LineAttributes::Direction::Negative) {
        addTriangle(toLine->location, fromLine->endBottom, toLine->startBottom);
        return;
    }

    addTriangle(toLine->location, fromLine->endTop, toLine->startTop);
}

void StrokePathBuilder::addCapShape(const LineCapType lineCap, const bool intermediateCap)
{
    if (lineCap == ButtCap)
        return;

    const FloatPoint startTopMargin(_shapeFirstLine->startTop.x - _shapeFirstLine->thicknessOffsets.x, _shapeFirstLine->startTop.y - _shapeFirstLine->thicknessOffsets.y);
    const FloatPoint startBottomMargin(_shapeFirstLine->startBottom.x - _shapeFirstLine->thicknessOffsets.x, _shapeFirstLine->startBottom.y - _shapeFirstLine->thicknessOffsets.y);
    const FloatPoint endTopMargin(_lastLine->endTop.x + _lastLine->thicknessOffsets.x, _lastLine->endTop.y + _lastLine->thicknessOffsets.y);
    const FloatPoint endBottomMargin(_lastLine->endBottom.x + _lastLine->thicknessOffsets.x, _lastLine->endBottom.y + _lastLine->thicknessOffsets.y);

    if (lineCap == SquareCap) {
        if (_shapeFirstLine->length) {
            addQuadShape(_shapeFirstLine->startBottom, startBottomMargin, startTopMargin, _shapeFirstLine->startTop);
        }
        if (_lastLine->length) {
            addQuadShape(_lastLine->endTop, endTopMargin, endBottomMargin, _lastLine->endBottom);
        }
        return;
    }

    GD_ASSERT(lineCap == RoundCap);
    FloatPoint miter;

    if (!intermediateCap || _shapeFirstLine->length) {
        miter = startTopMargin + startBottomMargin;
        miter.scale(0.5, 0.5);
        addRoundShape(_shapeFirstLine->location, _shapeFirstLine->startBottom, startBottomMargin, miter);
        addRoundShape(_shapeFirstLine->location, miter, startTopMargin, _shapeFirstLine->startTop);
    }

    if (_lastLine->length) {
        miter = endTopMargin + endBottomMargin;
        miter.scale(0.5, 0.5);
        addRoundShape(_lastLine->location + _lastLine->vector, _lastLine->endTop, endTopMargin, miter);
        addRoundShape(_lastLine->location + _lastLine->vector, miter, endBottomMargin, _lastLine->endBottom);
    }
}

void StrokePathBuilder::setCurrentLineAttribute()
{
    _lastLine = _lastLine->next;
    _currentLine = _currentLine->next;
}

} // namespace gepard
