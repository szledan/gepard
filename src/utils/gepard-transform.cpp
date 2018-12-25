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

#include "gepard-transform.h"

#include "gepard-float-point.h"
#include "gepard-float.h"
#include <cmath>

namespace gepard {

Transform::Transform(const Float a, const Float b, const Float c, const Float d, const Float e, const Float f)
{
    data[0] = a;
    data[1] = b;
    data[2] = c;
    data[3] = d;
    data[4] = e;
    data[5] = f;
}

Transform& Transform::rotate(const Float angle)
{
    const Float cosAngle = std::cos(angle);
    const Float sinAngle = std::sin(angle);
    Transform matrix(cosAngle, sinAngle, -sinAngle, cosAngle, 0.0, 0.0);
    multiply(matrix);
    return *this;
}

Transform& Transform::scale(const Float sx, const Float sy)
{
    Transform matrix(sx, 0.0, 0.0, sy, 0.0, 0.0);
    multiply(matrix);
    return *this;
}

Transform& Transform::translate(const Float x, const Float y)
{
    Transform matrix(1.0, 0.0, 0.0, 1.0, x, y);
    multiply(matrix);
    return *this;
}

const FloatPoint Transform::apply(const FloatPoint& p) const
{
    const Float x = p.x * data[0] + p.y * data[2] + data[4];
    const Float y = p.x * data[1] + p.y * data[3] + data[5];
    return FloatPoint(x, y);
}

Transform& Transform::multiply(const Transform& transform)
{
    const Float a = data[0];
    const Float b = data[1];
    const Float c = data[2];
    const Float d = data[3];

    data[0] = a * transform.data[0] + c * transform.data[1];
    data[1] = b * transform.data[0] + d * transform.data[1];
    data[2] = a * transform.data[2] + c * transform.data[3];
    data[3] = b * transform.data[2] + d * transform.data[3];
    if (transform.data[4] || transform.data[5]) {
        data[4] += a * transform.data[4] + c * transform.data[5];
        data[5] += b * transform.data[4] + d * transform.data[5];
    }
    return *this;
}

void Transform::operator*=(const Transform& transform)
{
    multiply(transform);
}

const Transform Transform::inverse() const
{
    Transform result;
    Float determinant = data[0] * data[3] - data[1] * data[2];
    if (!determinant)
        return result;

    result.data[0] = data[3] / determinant;
    result.data[1] = -data[1] / determinant;
    result.data[2] = -data[2] / determinant;
    result.data[3] = data[0] / determinant;
    result.data[4] = (data[2] * data[5] - data[3] * data[4]) / determinant;
    result.data[5] = (data[1] * data[4] - data[0] * data[5]) / determinant;

    return result;
}

Transform operator*(const Transform& lhs, const Transform& rhs)
{
    Transform transform = lhs;
    transform.multiply(rhs);
    return transform;
}

} // namespace gepard
