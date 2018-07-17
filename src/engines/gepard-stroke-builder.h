/* Copyright (C) 2018, Gepard Graphics
 * Copyright (C) 2013, 2018 Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_STROKE_BUILDER_H
#define GEPARD_STROKE_BUILDER_H

#include "gepard-defs.h"
#include "gepard-trapezoid-tessellator.h"
#include "gepard-path.h"
#include "gepard-types.h"

namespace gepard {

class StrokePathBuilder {
    struct LineAttributes {
        enum Direction {
            Same,
            Reverse,
            Positive,
            Negative,
        };

        LineAttributes() : next(nullptr) { }

        void set(const FloatPoint&, const FloatPoint&, const Float);
        Direction vectorCompare(const LineAttributes*) const;

        LineAttributes* next = nullptr;

        FloatPoint location;
        FloatPoint vector;
        FloatPoint unit;
        Float length;

        FloatPoint thicknessOffsets;
        FloatPoint startTop, startBottom;
        FloatPoint endTop, endBottom;
    };

public:
    StrokePathBuilder(const Float width, const Float miterLimit, const LineJoinType joinMode, const LineCapType lineCap);

    void convertStrokeToFill(const PathData* path);
    PathData* pathData() { return &_path; }

private:
    inline void addMoveToShape();
    inline void addCloseSubpathShape(const FloatPoint&, const CloseSubpathElement*);
    inline void addLineShape(const FloatPoint&, const LineToElement*);
    inline void addBezierCurveShape(const FloatPoint&, const QuadraticCurveToElement*);
    inline void addBezierCurveShape(const FloatPoint&, const BezierCurveToElement*);
    inline void addArcShape(const FloatPoint&, const ArcElement*);

    void addCapShapeIfNeeded()
    {
        if (_hasShapeFirstLine)
            addCapShape(_lineCap);
    }

    inline FloatPoint miterLength(const FloatPoint&, const FloatPoint&);

    inline void addTriangle(const FloatPoint&, const FloatPoint&, const FloatPoint&);
    inline void addTriangle(const FloatPoint&, const FloatPoint&, const FloatPoint&, const Float);
    inline void addQuadShape(const FloatPoint&, const FloatPoint&, const FloatPoint&, const FloatPoint&);
    inline void addRoundShape(const FloatPoint&, const FloatPoint&, const FloatPoint&, const FloatPoint&);
    void addJoinShape(const LineAttributes*, const LineAttributes*);
    void addCapShape(const LineCapType, const bool = false);
    void bezierCurveShape(const FloatPoint&, const FloatPoint&, const FloatPoint&, const FloatPoint&);

    void setCurrentLineAttribute();

    LineAttributes _lines[3];
    LineAttributes* _shapeFirstLine;
    LineAttributes* _lastLine;
    LineAttributes* _currentLine;
    bool _hasShapeFirstLine;

    Float _halfWidth;
    Float _miterLimitSquared;
    LineJoinType _joinMode;
    LineCapType _lineCap;

    PathData _path;
    SegmentApproximator _segmentApproximator;
};

} // namespace gepard

#endif // GEPARD_STROKE_BUILDER_H
