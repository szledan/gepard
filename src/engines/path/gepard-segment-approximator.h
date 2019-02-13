/* Copyright (C) 2019, Gepard Graphics
 * Copyright (C) 2019, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_SEGMENT_APPROXIMATOR_H
#define GEPARD_SEGMENT_APPROXIMATOR_H

#include "gepard-defs.h"
#include "gepard-float-point.h"
#include "gepard-float.h"
#include "gepard-path.h"
#include "gepard-segment.h"
#include "gepard-old-segment-approximator.h"
#include "gepard-transform.h"
#include "gepard-trapezoid-container.h"
#include <list>
#include <map>
#include <vector>

namespace gepard {

class SegmentApproximator {
public:
    SegmentApproximator(const int antiAliasLevel = GD_ANTIALIAS_LEVEL);
    ~SegmentApproximator();

    void insertLine(const FloatPoint& from, const FloatPoint& to);
    void insertQuadCurve(const FloatPoint& from, const FloatPoint& control, const FloatPoint& to);
    void insertBezierCurve(const FloatPoint& from, const FloatPoint& control1, const FloatPoint& control2, const FloatPoint& to);
    void insertArc(const FloatPoint& lastEndPoint, const ArcElement* arcElement, const Transform& transform);

    OldSegmentList* segments();
    const BoundingBox& boundingBox() const { return _boundingBox; }

    inline void splitSegments();

    const bool quadCurveIsLineSegment(FloatPoint[]);
    void splitQuadraticCurve(FloatPoint[]);

    const bool curveIsLineSegment(FloatPoint[]);
    const bool curveIsLineSegment(const FloatPoint& p0, const FloatPoint& p1, const FloatPoint& p2);
    void splitCubeCurve(FloatPoint[]);

    const int calculateArcSegments(const Float& angle, const Float& radius);
    void arcToCurve(FloatPoint[], const Float&, const Float&);

    const bool collinear(const FloatPoint& p0, const FloatPoint& p1, const FloatPoint& p2);

    const int kAntiAliasLevel;
    const Float kTolerance;
private:
    void insertSegment(const FloatPoint& from, const FloatPoint& to);
    OldSegmentList* insertSegmentList(const int y);

    OldSegmentTree _segments;
    BoundingBox _boundingBox;

    TrapezoidContainer _trapezoids;
};

} // namespace gepard

#endif // GEPARD_SEGMENT_APPROXIMATOR_H