/* Copyright (C) 2016-2017, Gepard Graphics
 * Copyright (C) 2015-2017, Szilard Ledan <szledan@gmail.com>
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

#include "gepard-path.h"

#include "gepard-defs.h"
#include "gepard-types.h"
#include <math.h>
#include <ostream>

namespace gepard {

/* PathData */

void PathData::addMoveToElement(FloatPoint to)
{
    if (_lastElement && _lastElement->isMoveTo()) {
        _lastElement->to = to;
        return;
    }

    PathElement* currentElement = static_cast<PathElement*>(new (_region.alloc(sizeof(MoveToElement))) MoveToElement(to));

    if (!_firstElement) {
        _firstElement = currentElement;
        _lastElement = _firstElement;
    } else {
        _lastElement->next = currentElement;
        _lastElement = _lastElement->next;
    }

    _lastMoveToElement = _lastElement;
}

void PathData::addLineToElement(FloatPoint to)
{
    if (!_lastElement) {
        addMoveToElement(to);
        return;
    }

    if (_lastElement->to == to)
        return;

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(LineToElement))) LineToElement(to));
    _lastElement = _lastElement->next;
}

void PathData::addQuadaraticCurveToElement(FloatPoint control, FloatPoint to)
{
    if (!_lastElement) {
        addMoveToElement(to);
        return;
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(QuadraticCurveToElement))) QuadraticCurveToElement(control, to));
    _lastElement = _lastElement->next;
}

void PathData::addBezierCurveToElement(FloatPoint control1, FloatPoint control2, FloatPoint to)
{
    if (!_lastElement) {
        addMoveToElement(to);
        return;
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(BezierCurveToElement))) BezierCurveToElement(control1, control2, to));
    _lastElement = _lastElement->next;
}

void PathData::addArcElement(FloatPoint center, FloatPoint radius, Float startAngle, Float endAngle, bool antiClockwise)
{
    FloatPoint start = FloatPoint(center.x + cos(startAngle) * radius.x, center.y + sin(startAngle) * radius.y);

    if (!_lastElement) {
        addMoveToElement(center);
        return;
    }

    if (!radius.x || !radius.y || startAngle == endAngle) {
        addLineToElement(start);
        return;
    }

    if (_lastElement->to != start) {
        addLineToElement(start);
    }

    const Float twoPiFloat = 2.0 * piFloat;
    if (antiClockwise && startAngle - endAngle >= twoPiFloat) {
        startAngle = fmod(startAngle, twoPiFloat);
        endAngle = startAngle - twoPiFloat;
    } else if (!antiClockwise && endAngle - startAngle >= twoPiFloat) {
        startAngle = fmod(startAngle, twoPiFloat);
        endAngle = startAngle + twoPiFloat;
    } else {
        bool equal = startAngle == endAngle;

        startAngle = fmod(startAngle, twoPiFloat);
        if (startAngle < 0) {
            startAngle += twoPiFloat;
        }

        endAngle = fmod(endAngle, twoPiFloat);
        if (endAngle < 0) {
            endAngle += twoPiFloat;
        }

        if (!antiClockwise) {
            if (startAngle > endAngle || (startAngle == endAngle && !equal)) {
                endAngle += twoPiFloat;
            }
            GD_ASSERT(0 <= startAngle && startAngle <= twoPiFloat);
            GD_ASSERT(startAngle <= endAngle && endAngle - startAngle <= twoPiFloat);
        } else {
            if (startAngle < endAngle || (startAngle == endAngle && !equal)) {
                endAngle -= twoPiFloat;
            }
            GD_ASSERT(0 <= startAngle && startAngle <= twoPiFloat);
            GD_ASSERT(endAngle <= startAngle && startAngle - endAngle <= twoPiFloat);
        }
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(ArcElement))) ArcElement(center, radius, startAngle, endAngle, antiClockwise));
    _lastElement = _lastElement->next;
}

void PathData::addArcToElement(const FloatPoint& control, const FloatPoint& end, const Float& radius)
{
    if (!_lastElement) {
        addMoveToElement(control);
        return;
    }

    if (_lastElement->to == control || control == end || !radius) {
        addLineToElement(control);
    }

    FloatPoint start(_lastElement->to);

    FloatPoint delta1(start - control);
    FloatPoint delta2(end - control);
    Float delta1Length = sqrtf(delta1.lengthSquared());
    Float delta2Length = sqrtf(delta2.lengthSquared());

    // Normalized dot product.
    GD_ASSERT(delta1Length && delta2Length);
    Float cosPhi = delta1.dot(delta2) / (delta1Length * delta2Length);

    // All three points are on the same straight line (HTML5, 4.8.11.1.8).
    if (fabs(cosPhi) >= 0.9999) {
        addLineToElement(control);
        return;
    }

    Float tangent = radius / tan(acos(cosPhi) / 2.0);
    Float delta1Factor = tangent / delta1Length;
    FloatPoint arcStartPoint((control.x + delta1Factor * delta1.x), (control.y + delta1Factor * delta1.y));

    FloatPoint orthoStartPoint(delta1.y, -delta1.x);
    Float orthoStartPointLength = sqrtf(orthoStartPoint.lengthSquared());
    Float radiusFactor = radius / orthoStartPointLength;

    GD_ASSERT(orthoStartPointLength);
    Float cosAlpha = (orthoStartPoint.x * delta2.x + orthoStartPoint.y * delta2.y) / (orthoStartPointLength * delta2Length);
    if (cosAlpha < 0.0)
        orthoStartPoint = FloatPoint(-orthoStartPoint.x, -orthoStartPoint.y);

    FloatPoint center((arcStartPoint.x + radiusFactor * orthoStartPoint.x), (arcStartPoint.y + radiusFactor * orthoStartPoint.y));

    // Calculate angles for addArc.
    orthoStartPoint = FloatPoint(-orthoStartPoint.x, -orthoStartPoint.y);
    Float startAngle = acos(orthoStartPoint.x / orthoStartPointLength);
    if (orthoStartPoint.y < 0.0)
        startAngle = 2.0 * piFloat - startAngle;

    bool antiClockwise = false;

    Float delta2Factor = tangent / delta2Length;
    FloatPoint arcEndPoint((control.x + delta2Factor * delta2.x), (control.y + delta2Factor * delta2.y));
    FloatPoint orthoEndPoint(arcEndPoint - center);
    Float orthoEndPointLength = sqrtf(orthoEndPoint.lengthSquared());
    Float endAngle = acos(orthoEndPoint.x / orthoEndPointLength);
    if (orthoEndPoint.y < 0.0) {
        endAngle = 2 * piFloat - endAngle;
    }
    if ((startAngle > endAngle) && ((startAngle - endAngle) < piFloat)) {
        antiClockwise = true;
    }
    if ((startAngle < endAngle) && ((endAngle - startAngle) > piFloat)) {
        antiClockwise = true;
    }

    addArcElement(center, FloatPoint(radius, radius), startAngle, endAngle, antiClockwise);
}

void PathData::addCloseSubpathElement()
{
    if (!_lastElement || _lastElement->isCloseSubpath())
        return;

    if (_lastElement->isMoveTo()) {
        addLineToElement(_lastElement->to);
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(CloseSubpathElement))) CloseSubpathElement(_lastMoveToElement->to));
    _lastElement = _lastElement->next;
}

const PathElement* PathData::operator[](std::size_t idx) const
{
    PathElement* element = _firstElement;

    std::size_t i = 0;
    while (element) {
        if (i == idx)
            break;
        i++;
        element = element->next;
    }

    if (!element)
        element = _lastElement;

    return element;
}

/* Path */

} // namespace gepard
