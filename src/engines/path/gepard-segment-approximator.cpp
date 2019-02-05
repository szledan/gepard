/* Copyright (C) 2019, Gepard Graphics
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

#include "gepard-segment-approximator.h"

#include "gepard-defs.h"
#include "gepard-float-point.h"
#include "gepard-float.h"
#include "gepard-logging.h"
#include "gepard-segment.h"
#include "gepard-transform.h"
#include <cmath>
#include <list>
#include <set>

namespace gepard {

SegmentApproximator::SegmentApproximator(const int antiAliasLevel)
    : kAntiAliasLevel(antiAliasLevel > 0 ? antiAliasLevel : GD_ANTIALIAS_LEVEL)
    , kTolerance(1.0)
{
}

void SegmentApproximator::insertLine(const FloatPoint& from, const FloatPoint& to)
{
    if (from.y == to.y)
        return;

    GD_LOG(TRACE) << "Insert line: " << from << "->" << to;
    insertSegment(FloatPoint(from.x * kAntiAliasLevel, std::floor(from.y * kAntiAliasLevel)), FloatPoint(to.x * kAntiAliasLevel, std::floor(to.y * kAntiAliasLevel)));
}

void SegmentApproximator::insertSegment(const FloatPoint& from, const FloatPoint& to)
{
    if (from.y == to.y)
        return;

    // Insert segment.
    _segments.addSegment(Segment(from, to));
}

} // namespace gepard
