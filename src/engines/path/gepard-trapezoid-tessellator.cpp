/* Copyright (C) 2018-2019, Gepard Graphics
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

#include "gepard-trapezoid-tessellator.h"

#include "gepard-defs.h"
#include "gepard-float-point.h"
#include "gepard-float.h"
#include "gepard-logging.h"
#include "gepard-old-segment-approximator.h"
#include "gepard-segment-approximator.h"
#include "gepard-transform.h"
#include <cmath>
#include <cstring>
#include <list>
#include <set>

namespace gepard {

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

    SegmentApproximator segmentApproximator(_antiAliasingLevel);
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
    OldSegmentList* segmentList = segmentApproximator.segments();
    TrapezoidList trapezoids;

    // 3. Generate trapezoids.
    const Float denom = _antiAliasingLevel * 1 + 0;
    if (segmentList) {
        Trapezoid trapezoid;
        int fill = 0;
        bool isInFill = false;
        for (OldSegment& segment : *segmentList) {
            if (segment.from.y == segment.to.y)
                continue;
            if (fillRule() == EvenOdd) {
                fill = !fill;
            } else {
                fill += segment.direction;
            }

            if (fill) {
                if (!isInFill) {
                    trapezoid.topY = ((segment.topY() / denom));
                    trapezoid.bottomY = ((segment.bottomY() / denom));
                    trapezoid.topLeftX = ((segment.from.x) / denom);
                    trapezoid.bottomLeftX = ((segment.to.x) / denom);
                    trapezoid.leftId = segment.id;
                    trapezoid.leftSlope = segment.realSlope;
                    if (trapezoid.topY != trapezoid.bottomY)
                        isInFill = true;
                }
            } else {
                // TODO: Horizontal merge trapezoids.
                trapezoid.topRightX = ((segment.from.x) / denom);
                trapezoid.bottomRightX = ((segment.to.x) / denom);
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
        _boundingBox.minX = ((segmentApproximator.boundingBox().minX) / float(_antiAliasingLevel));
        _boundingBox.minY = ((segmentApproximator.boundingBox().minY) / float(_antiAliasingLevel));
        _boundingBox.maxX = ((segmentApproximator.boundingBox().maxX) / float(_antiAliasingLevel));
        _boundingBox.maxY = ((segmentApproximator.boundingBox().maxY) / float(_antiAliasingLevel));
    }

    trapezoids.sort();
    GD_LOG(TRACE) << "Number of horizontal merged trapezoids: " << trapezoids.size();

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
    GD_LOG(TRACE) << "Number of vertical merged trapezoids: " << trapezoidList.size();

    return trapezoidList;
}

} // namespace gepard
