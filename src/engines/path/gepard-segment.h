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

#ifndef GEPARD_SEGMENT_H
#define GEPARD_SEGMENT_H

#include "gepard-float-point.h"
#include "gepard-float.h"
#include <ostream>

namespace gepard {

typedef unsigned long int UIntID;
static UIntID s_uid = 0;

struct Segment {
    Segment(const FloatPoint from, const FloatPoint to, const UIntID uid = 0, const Float slope = NAN);

    enum Direction {
        Negative = -1,
        EqualOrNonExist = 0,
        Positive = 1,
    };

    const FloatPoint& from() const { return _from; }
    const FloatPoint& to() const { return _to; }
    const FloatPoint top() const { return FloatPoint(_from.x, topY()); }
    const FloatPoint bottom() const { return FloatPoint(_to.x, bottomY()); }
    const int topY() const { return std::floor(_from.y); }
    const int bottomY() const { return std::floor(_to.y); }

    const Float& slope() const { return _slope; }
    const Direction direction() const { return _direction; }
    const UIntID uid() const { return _uid; }

    const Float factor() const { return _slope * _from.y - _from.x; }

    const Segment split(int y)
    {
        GD_ASSERT(y > topY());
        GD_ASSERT(y <= bottomY());
        if (y == bottomY())
            return *this;

        FloatPoint oldFrom = _from;
        _from.x = _to.x - _slope * (_to.y - y);
        _from.y = y;

        return Segment(oldFrom, _from, uid(), slope());
    }

private:
    FloatPoint _from, _to;
    Float _slope;
    Direction _direction = EqualOrNonExist;
    UIntID _uid = 0;
};

inline std::ostream& operator<<(std::ostream& os, const Segment& s)
{
    return os << s.from() << ((s.direction() < 0) ? "<" : ((s.direction() > 0) ? ">" : "=")) << s.to();
}

inline bool operator<(const Segment& lhs, const Segment& rhs)
{
    GD_ASSERT(lhs.topY() == rhs.topY());
    return (lhs.top().x < rhs.top().x
            || (lhs.top().x == rhs.top().x
                && (lhs.slope() < rhs.slope()
                    || (lhs.slope() == rhs.slope() && lhs.uid() < rhs.uid()))));
}

inline bool operator==(const Segment& lhs, const Segment& rhs)
{
    return !(lhs < rhs) && !(rhs < lhs);
}

inline bool operator<=(const Segment& lhs, const Segment& rhs)
{
    return lhs < rhs || lhs == rhs;
}

} // namespace gepard

#endif // GEPARD_SEGMENT_H
