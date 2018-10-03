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

#ifndef GEPARD_COLOR_H
#define GEPARD_COLOR_H

#include "gepard-float.h"
#include "gepard-vec4.h"
#include <string>

namespace gepard {

/*!
 * \brief The Color struct
 *
 * Describes a simple RGBA color chanel struct
 * where each chanel is a gepard::Float [0.0, 1.0].
 *
 * \todo reference: https://www.w3.org/TR/css-color-3/
 * \internal
 */
struct Color : public Vec4 {
    Color();
    Color(const Float red, const Float green, const Float blue, const Float alpha);
    Color(const std::string& color);
    Color(const Color& color);

    /*!
     * \brief fromRawDataABGR
     * \param raw
     * \return
     *
     * \internal
     * \todo doc missing
     * \todo unit-test missing
     */
    static Color fromRawDataABGR(const uint32_t raw);
    /*!
     * \brief toRawDataABGR
     * \param color
     * \return
     *
     * \internal
     * \todo doc missing
     * \todo unit-test missing
     */
    static uint32_t toRawDataABGR(const Color& color);

    /*!
     * \brief operator *=
     * \param rhs
     * \return
     *
     * \internal
     * \todo doc missing
     * \todo unit-test missing
     */
    Color& operator*=(const Float& rhs);

    static const Color BLACK;
    static const Color WHITE;
};

/*!
 * \internal
 * \todo unit-test missing
 */
inline Color operator+(const Color& lhs, const Color& rhs)
{
    return Color(lhs.r + rhs.r, lhs.g + rhs.g, lhs.b + rhs.b, lhs.a + rhs.a);
}

} // namespace gepard

#endif // GEPARD_COLOR_H
