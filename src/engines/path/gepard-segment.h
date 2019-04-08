/* Copyright (C) 2018-2019, Gepard Graphics
 * Copyright (C) 2018-2019, Szilard Ledan <szledan@gmail.com>
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
#include <memory>

namespace gepard {

static uint32_t s_uid = 0;

/*!
 * \brief The Segment struct
 *
 *    (0,0) +--------> +x [float]
 *          | top
 *          |
 *          | bottom ( > top)  ^
 * +y [int] v POS direction    | NEG direction
 *
 *  m = (bottomY - topY) / (bottomX - topX)
 *  dx = 1 / m = (bottomX - topX) / (bottomY - topY)
 *
 *  direction == 0 -> invalid Segment
 */
struct Segment {
    Segment(const FloatPoint from = FloatPoint(), const FloatPoint to = FloatPoint())
        : uid(++s_uid)
        , topY(std::floor(from.y))
        , bottomY(std::floor(to.y))
    {
        if (bottomY == topY) {
            direction = 0;
            return;
        }

        if (bottomY < topY) {
            direction = -1;
            topX = to.x;
            bottomX = from.x;
            int tmp = topY;
            topY = bottomY;
            bottomY = tmp;
        } else {
            direction = 1;
            topX = from.x;
            bottomX = to.x;
        }
        dx = (bottomX - topX) / (bottomY - topY);
    }
    Segment(const Segment& s)
        : uid(s.uid)
        , topY(s.topY), bottomY(s.bottomY)
        , topX(s.topX), bottomX(s.bottomX)
        , dx(s.dx), direction(s.direction)
    {}

    const Float x(const int y) const { return dx * (y - topY) + topX; }

    Segment&& cutAndRemoveTop(const int y)
    {
        GD_ASSERT(y > topY);
        Segment s = *this;
        topX = (s.bottomX = x(y));
        topY = (s.bottomY = y);
        return std::move(s);
    }

    Segment&& cutAndRemoveBottom(const int y)
    {
        Segment s = *this;
        bottomX = (s.topX = x(y));
        bottomY = (s.topY = y);
        return std::move(s);
    }

    const bool intersectionY(const Segment& next, int* y) const
    {
        GD_ASSERT(topX <= next.topX);
        GD_ASSERT(topY == next.topY);
        GD_ASSERT(bottomY == next.bottomY);
        if (topX < next.topX && next.bottomX < bottomX) {
            GD_ASSERT(next.dx - dx);
            *y = std::floor((topX - next.topX) / (next.dx - dx) + topY);
            return true;
        }
        return false;
    }

    uint32_t uid;
    int topY, bottomY; //! is it needed?
    Float topX, bottomX;
    Float dx;
    int direction = 0;
};

inline std::ostream& operator<<(std::ostream& os, const Segment& s)
{
    return os << s.topX << "," << s.topY << ((s.direction < 0) ? "<" : ((s.direction > 0) ? ">" : "=")) << s.bottomX << "," << s.bottomY;
}

inline bool operator<(const Segment& lhs, const Segment& rhs)
{
    GD_ASSERT(lhs.topY == rhs.topY);
    return (lhs.topX < rhs.topX
            || (lhs.topX == rhs.topX
                && (lhs.dx < rhs.dx
                    || (lhs.dx == rhs.dx && lhs.uid < rhs.uid))));
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
