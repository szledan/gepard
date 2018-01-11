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

#ifndef GEPARD_PATH_H
#define GEPARD_PATH_H

#include "gepard-types.h"
#include <ostream>

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
    explicit PathElement();
    explicit PathElement(const PathElementType type);
    explicit PathElement(const PathElementType type, const FloatPoint& to);

    const bool isMoveTo() const;
    const bool isCloseSubpath() const;
    virtual std::ostream& output(std::ostream& os) const;

    PathElement* next;
    PathElementType type;
    FloatPoint to;
};

inline std::ostream& operator<<(std::ostream& os, const PathElement& ps);

struct MoveToElement : public PathElement {
    explicit MoveToElement(const FloatPoint& to);

    std::ostream& output(std::ostream& os) const;
};

struct LineToElement : public PathElement {
    explicit LineToElement(const FloatPoint& to);

    std::ostream& output(std::ostream& os) const;
};

struct CloseSubpathElement : public PathElement {
    explicit CloseSubpathElement(const FloatPoint& to);

    std::ostream& output(std::ostream& os) const;
};

struct QuadraticCurveToElement : public PathElement {
    explicit QuadraticCurveToElement(const FloatPoint& control, const FloatPoint& to);

    std::ostream& output(std::ostream& os) const;

    FloatPoint control;
};

struct BezierCurveToElement : public PathElement {
    explicit BezierCurveToElement(const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& to);

    std::ostream& output(std::ostream& os) const;

    FloatPoint control1;
    FloatPoint control2;
};

struct ArcElement : public PathElement {
    explicit ArcElement(const FloatPoint& center, const FloatPoint& radius, const Float startAngle, const Float endAngle, const bool counterClockwise = false);

    std::ostream& output(std::ostream& os) const;

    FloatPoint center;
    FloatPoint radius;
    Float startAngle;
    Float endAngle;
    bool counterClockwise;
};

struct PathData {
    explicit PathData();

    void addMoveToElement(FloatPoint);
    void addLineToElement(FloatPoint);
    void addQuadaraticCurveToElement(FloatPoint, FloatPoint);
    void addBezierCurveToElement(FloatPoint, FloatPoint, FloatPoint);
    void addArcElement(FloatPoint, FloatPoint, Float, Float, bool = true);
    void addArcToElement(const FloatPoint&, const FloatPoint&, const Float&);
    void addCloseSubpathElement();

    PathElement* firstElement() const { return _firstElement; }
    PathElement* lastElement() const { return _lastElement; }

    const PathElement* operator[](std::size_t idx) const;

private:
    Region<> _region;
    PathElement* _firstElement;
    PathElement* _lastElement;
    PathElement* _lastMoveToElement;
};

/* Path */

class Path {
public:
    explicit Path();
    ~Path();

    PathData* pathData() { return _pathData; }
    void clear();

private:
    PathData* _pathData;
};

} // namespace gepard

#endif // GEPARD_PATH_H
