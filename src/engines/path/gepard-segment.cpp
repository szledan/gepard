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
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#include "gepard-segment.h"

#include "gepard-float-point.h"
#include "gepard-float.h"

namespace gepard {

Segment::Segment(const FloatPoint from, const FloatPoint to, const UIntID uid, const Float slope)
    : _from(from)
    , _to(to)
    , _uid(uid ? uid : s_uid++)
{
    Float denom = to.y - from.y;
    if (denom) {
        if (denom < (Float)0.0) {
            _from = to;
            _to = from;
            _direction = Negative;
            denom *= (Float)(-1.0);
        } else {
            _direction = Positive;
        }
    } else {
        this->_direction = EqualOrNonExist;
    }

    _slope = (std::isnan(slope)) ? ((_to.x - _from.x) / (denom)) : slope;
    GD_ASSERT(!std::isnan(_slope));
}

#if 0

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

    } else {
        this->direction = EqualOrNonExist;
    }
    slopeInv = (this->to.x - this->from.x) / (denom);

    realSlope = (std::isnan(slope)) ? slopeInv : slope;
    GD_ASSERT(!std::isnan(realSlope));
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

    GD_ASSERT(this->from.y == segment->from.y);
    GD_ASSERT(this->to.y == segment->to.y);

    if (this->from.x == segment->from.x) {
        return true;
    }

    if (this->to.x == segment->to.x) {
        return true;
    }

    Float denom = (this->slopeInv() - segment->slopeInv());
    y = (this->factor() - segment->factor()) / denom;

    if (std::isnan(y))
        return false;

    if (!isOnSegment(y)) {
        y = INFINITY;
        return false;
    }

    return true;
}

std::ostream& operator<<(std::ostream& os, const Segment& s)
{
    return os << s.from << ((s.direction < 0) ? "<" : ((s.direction > 0) ? ">" : "=")) << s.to;
}

bool operator<(const Segment& lhs, const Segment& rhs)
{
    GD_ASSERT(lhs.from.y <= lhs.to.y);
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

#endif

} // namespace gepard
