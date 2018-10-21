/* Copyright (C) 2015-2016, 2018, Gepard Graphics
 * Copyright (C) 2015, 2018, Szilard Ledan <szledan@gmail.com>
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

#ifndef GEPARD_BOUNDING_BOX_H
#define GEPARD_BOUNDING_BOX_H

#include "gepard-float-point.h"
#include "gepard-float.h"
#include <ostream>

namespace gepard {

/*!
 * \brief The BoundingBox struct
 *
 * The BoundignBox struct describes a rectangle with four Float values.
 * Each value determines either minimum or maximum values from
 * the given points.
 *
 * \internal
 */
struct BoundingBox {
    BoundingBox();

    void stretchX(const Float x);
    void stretchY(const Float y);
    void stretch(const FloatPoint& p);

    Float minX, minY, maxX, maxY;
};

inline std::ostream& operator<<(std::ostream& os, const BoundingBox& bb)
{
    return os << bb.minX << "," << bb.minY << "," << bb.maxX << "," << bb.maxY;
}

} // namespace gepard

#endif // GEPARD_BOUNDING_BOX_H
