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

#ifndef GEPARD_VEC4_H
#define GEPARD_VEC4_H

#include "gepard-float.h"

namespace gepard {

/*!
 * \brief The Vec4 struct
 *
 * \internal
 */
struct Vec4 {
    Vec4();
    Vec4(const Float x_, const Float y_, const Float z_, const Float w_);
    Vec4(const Vec4& vec4);

    Float& operator[](std::size_t idx);

    /*!
     * \brief The x/r/s component union
     */
    union {
        Float x; /*!< the x coordinate */
        Float r; /*!< red channel */
        Float s; /*!< the s parameter */
    };
    /*!
     * \brief The y/g/t component union
     */
    union {
        Float y; /*!< the y coordinate */
        Float g; /*!< green channel */
        Float t; /*!< the t parameter */
    };
    /*!
     * \brief The z/b/p component union
     */
    union {
        Float z; /*!< the z coordinate */
        Float b; /*!< blue channel */
        Float p; /*!< the p parameter */
    };
    /*!
     * \brief The w/a/q component union
     */
    union {
        Float w; /*!< the w coordinate */
        Float a; /*!< alpha channel */
        Float q; /*!< the q parameter */
    };
};

} // namespace gepard

#endif // GEPARD_VEC4_H
