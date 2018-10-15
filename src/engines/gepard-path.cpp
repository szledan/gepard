/* Copyright (C) 2016-2018, Gepard Graphics
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
 */

#include "gepard-path.h"

#include "gepard-defs.h"
#include "gepard-float-point.h"
#include "gepard-float.h"
#include "gepard-logging.h"
#include "gepard-transform.h"
#include <cmath>
#include <ostream>

namespace gepard {

/* PathElement */

PathElement::PathElement()
    : next(nullptr)
    , type(PathElementTypes::Undefined)
{
}

PathElement::PathElement(const PathElementType type)
    : next(nullptr)
    , type(type)
{
}

PathElement::PathElement(const PathElementType type, const FloatPoint& to)
    : next(nullptr)
    , type(type)
    , to(to)
{}

const bool PathElement::isMoveTo() const
{
    return this->type == PathElementTypes::MoveTo;
}

const bool PathElement::isCloseSubpath() const
{
    return this->type == PathElementTypes::CloseSubpath;
}

std::ostream& PathElement::output(std::ostream& os) const
{
    return os << this->to;
}

std::ostream& operator<<(std::ostream& os, const PathElement& ps)
{
    return ps.output(os);
}

MoveToElement::MoveToElement(const FloatPoint& to)
    : PathElement(PathElementTypes::MoveTo, to)
{
}

std::ostream& MoveToElement::output(std::ostream& os) const
{
    return PathElement::output(os << "M");
}

LineToElement::LineToElement(const FloatPoint& to)
    : PathElement(PathElementTypes::LineTo, to)
{
}

std::ostream& LineToElement::output(std::ostream& os) const
{
    return PathElement::output(os << "L");
}

CloseSubpathElement::CloseSubpathElement(const FloatPoint& to)
    : PathElement(PathElementTypes::CloseSubpath, to)
{
}

std::ostream& CloseSubpathElement::output(std::ostream& os) const
{
    return PathElement::output(os << "Z");
}

QuadraticCurveToElement::QuadraticCurveToElement(const FloatPoint& control, const FloatPoint& to)
    : PathElement(PathElementTypes::QuadraticCurve, to)
    , control(control)
{
}

std::ostream& QuadraticCurveToElement::output(std::ostream& os) const
{
    return PathElement::output(os << "Q" << this->control << " ");
}

BezierCurveToElement::BezierCurveToElement(const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& to)
    : PathElement(PathElementTypes::BezierCurve, to)
    , control1(control1)
    , control2(control2)
{
}

std::ostream& BezierCurveToElement::output(std::ostream& os) const
{
    return PathElement::output(os << "C" << this->control1 << " " << this->control2 << " ");
}

ArcElement::ArcElement(const FloatPoint& center, const FloatPoint& radius, const Float startAngle, const Float endAngle, const bool counterClockwise)
    : PathElement(PathElementTypes::Arc, FloatPoint(center.x + std::cos(endAngle) * radius.x, center.y + std::sin(endAngle) * radius.y))
    , center(center)
    , radius(radius)
    , startAngle(startAngle)
    , endAngle(endAngle)
    , counterClockwise(counterClockwise)
{
    GD_ASSERT(radius.x >= 0 && radius.y >= 0);
}

std::ostream& ArcElement::output(std::ostream& os) const
{
    // TODO(szledan): generate SVG representation
    os << "A";
    return PathElement::output(os);
}

/* PathData */

PathData::PathData()
    : _firstElement(nullptr)
    , _lastElement(nullptr)
    , _lastMoveToElement(nullptr)
{}

void PathData::addMoveToElement(FloatPoint to)
{
    if (_lastElement && _lastElement->isMoveTo()) {
        _lastElement->to = to;
        return;
    }

    PathElement* currentElement = static_cast<PathElement*>(new (_region.alloc(sizeof(MoveToElement))) MoveToElement(to));
    GD_LOG(TRACE) << "Add path element: " << (*currentElement);

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
    GD_LOG(TRACE) << "Add path element: " << (*_lastElement);
}

void PathData::addQuadaraticCurveToElement(FloatPoint control, FloatPoint to)
{
    if (!_lastElement) {
        addMoveToElement(to);
        return;
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(QuadraticCurveToElement))) QuadraticCurveToElement(control, to));
    _lastElement = _lastElement->next;
    GD_LOG(TRACE) << "Add path element: " << (*_lastElement);
}

void PathData::addBezierCurveToElement(FloatPoint control1, FloatPoint control2, FloatPoint to)
{
    if (!_lastElement) {
        addMoveToElement(to);
        return;
    }

    _lastElement->next = static_cast<PathElement*>(new (_region.alloc(sizeof(BezierCurveToElement))) BezierCurveToElement(control1, control2, to));
    _lastElement = _lastElement->next;
    GD_LOG(TRACE) << "Add path element: " << (*_lastElement);
}

void PathData::addArcElement(FloatPoint center, FloatPoint radius, Float startAngle, Float endAngle, bool antiClockwise)
{
    FloatPoint start = FloatPoint(center.x + std::cos(startAngle) * radius.x, center.y + std::sin(startAngle) * radius.y);

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
        startAngle = std::fmod(startAngle, twoPiFloat);
        endAngle = startAngle - twoPiFloat;
    } else if (!antiClockwise && endAngle - startAngle >= twoPiFloat) {
        startAngle = std::fmod(startAngle, twoPiFloat);
        endAngle = startAngle + twoPiFloat;
    } else {
        bool equal = startAngle == endAngle;

        startAngle = std::fmod(startAngle, twoPiFloat);
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
    GD_LOG(TRACE) << "Add path element: " << (*_lastElement);
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

    const FloatPoint start(_lastElement->to);

    const FloatPoint delta1(start - control);
    const FloatPoint delta2(end - control);
    const Float delta1Length = std::sqrt(delta1.lengthSquared());
    const Float delta2Length = std::sqrt(delta2.lengthSquared());

    // Normalized dot product.
    GD_ASSERT(delta1Length && delta2Length);
    const Float cosPhi = delta1.dot(delta2) / (delta1Length * delta2Length);

    // All three points are on the same straight line (HTML5, 4.8.11.1.8).
    if (std::fabs(cosPhi) >= 0.9999) {
        addLineToElement(control);
        return;
    }

    const Float tangent = radius / std::tan(std::acos(cosPhi) / 2.0);
    const Float delta1Factor = tangent / delta1Length;
    const FloatPoint arcStartPoint((control.x + delta1Factor * delta1.x), (control.y + delta1Factor * delta1.y));

    FloatPoint orthoStartPoint(delta1.y, -delta1.x);
    const Float orthoStartPointLength = std::sqrt(orthoStartPoint.lengthSquared());
    const Float radiusFactor = radius / orthoStartPointLength;

    GD_ASSERT(orthoStartPointLength);
    const Float cosAlpha = (orthoStartPoint.x * delta2.x + orthoStartPoint.y * delta2.y) / (orthoStartPointLength * delta2Length);
    if (cosAlpha < 0.0) {
        orthoStartPoint = FloatPoint(-orthoStartPoint.x, -orthoStartPoint.y);
    }

    const FloatPoint center((arcStartPoint.x + radiusFactor * orthoStartPoint.x), (arcStartPoint.y + radiusFactor * orthoStartPoint.y));

    // Calculate angles for addArc.
    orthoStartPoint = FloatPoint(-orthoStartPoint.x, -orthoStartPoint.y);
    Float startAngle = std::acos(orthoStartPoint.x / orthoStartPointLength);
    if (orthoStartPoint.y < 0.0) {
        startAngle = 2.0 * piFloat - startAngle;
    }

    bool counterClockwise = false;

    const Float delta2Factor = tangent / delta2Length;
    const FloatPoint arcEndPoint((control.x + delta2Factor * delta2.x), (control.y + delta2Factor * delta2.y));
    const FloatPoint orthoEndPoint(arcEndPoint - center);
    const Float orthoEndPointLength = std::sqrt(orthoEndPoint.lengthSquared());
    Float endAngle = std::acos(orthoEndPoint.x / orthoEndPointLength);
    if (orthoEndPoint.y < 0.0) {
        endAngle = 2.0 * piFloat - endAngle;
    }
    if ((startAngle > endAngle) && ((startAngle - endAngle) < piFloat)) {
        counterClockwise = true;
    }
    if ((startAngle < endAngle) && ((endAngle - startAngle) > piFloat)) {
        counterClockwise = true;
    }

    addArcElement(center, FloatPoint(radius, radius), startAngle, endAngle, counterClockwise);
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
    GD_LOG(TRACE) << "Add path element: " << (*_lastElement);
}

void PathData::applyTransform(const Transform& transform)
{
    PathElement* element = _firstElement;

    while (element) {
        switch (element->type) {
        case PathElementTypes::MoveTo:
        case PathElementTypes::CloseSubpath:
        case PathElementTypes::LineTo:
            element->to = transform.apply(element->to);
            break;
        case PathElementTypes::QuadraticCurve:
            QuadraticCurveToElement* quadToElement;
            quadToElement = reinterpret_cast<QuadraticCurveToElement*>(element);
            quadToElement->to = transform.apply(quadToElement->to);
            quadToElement->control = transform.apply(quadToElement->control);
            break;
        case PathElementTypes::BezierCurve:
            BezierCurveToElement* curveToElement;
            curveToElement = reinterpret_cast<BezierCurveToElement*>(element);
            curveToElement->to = transform.apply(curveToElement->to);
            curveToElement->control1 = transform.apply(curveToElement->control1);
            curveToElement->control2 = transform.apply(curveToElement->control2);
            break;
        case PathElementTypes::Arc:
            ArcElement* arcToElement;
            arcToElement = reinterpret_cast<ArcElement*>(element);
            arcToElement->to = transform.apply(arcToElement->to);
            arcToElement->multiply(transform);
            break;
        default:
            break;
        }

        element = element->next;
    }
}

const bool PathData::isEmpty() const
{
    //! \todo(szledan): Need more investigation of condition.
    return !_firstElement && _firstElement == _lastElement;
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

    if (!element) {
        element = _lastElement;
    }

    return element;
}

/* Path */

Path::Path()
    : _pathData(new PathData())
{
}

Path::~Path()
{
    if (_pathData) {
        delete _pathData;
    }
}

void Path::clear()
{
    if (_pathData) {
        delete _pathData;
    }
    _pathData = new PathData();
}

} // namespace gepard
