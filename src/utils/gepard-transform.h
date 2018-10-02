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

#ifndef GEPARD_TRANSFORM_H
#define GEPARD_TRANSFORM_H

#include "gepard-float-point.h"
#include "gepard-float.h"

namespace gepard {

struct Transform {
    Float data[6] = { 1.0, 0.0, 0.0, 1.0, 0.0, 0.0 };

    Transform (const Float a = 1.0, const Float b = 0.0, const Float c = 0.0, const Float d = 1.0, const Float e = 0.0, const Float f = 0.0);

    Transform& rotate(const Float angle);
    Transform& scale(const Float sx, const Float sy);
    Transform& translate(const Float x, const Float y);

    const FloatPoint apply(const FloatPoint& p) const;

    Transform& multiply(const Transform& transform);
    void operator*=(const Transform& transform);

    const Transform inverse() const;
};

Transform operator*(const Transform& lhs , const Transform& rhs);

} // namespace gepard

#endif // GEPARD_TRANSFORM_H
