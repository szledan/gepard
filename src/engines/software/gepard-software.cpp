/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2016, Szilard Ledan <szledan@gmail.com>
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

#include "gepard-software.h"

#include "gepard-color.h"
#include "gepard-float.h"
#include "gepard-logging.h"

namespace gepard {
namespace software {

GepardSoftware::GepardSoftware(GepardContext& context)
    : _context(context)
{
    _buffer.reserve(context.surface->width() * context.surface->height());
}

GepardSoftware::~GepardSoftware()
{
}

/*!
 * \brief Fill rect with Software backend.
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 */
void GepardSoftware::fillRect(const Float x, const Float y, const Float w, const Float h)
{
    GD_LOG(DEBUG) << "Fill rect with Software backend (" << x << ", " << y << ", " << w << ", " << h << ")";

    const Color fillColor = _context.currentState().fillColor;
    const uint32_t width = _context.surface->width();

    //! \todo (szledan): anti-aliassing
    //! \fixme (szledan): checking boundary
    GD_LOG(TRACE) << "Fill destination buffer";
    for (int j = y; j < y + h; ++j)
        for (int i = x; i < x + w; ++i) {
            uint32_t& dstRaw = _buffer[j * width + i];
            Color dst = Color::fromRawDataABGR(dstRaw);
            Color src = fillColor;

            // Apply src-alpha, one-minus-src-alpha blending mode.
            // Use one-minus-src-alpha on dst.
            dst *= (1.0f - src.a);

            // Use src-alpha on src.
            src *= src.a;

            dstRaw = Color::toRawDataABGR(src + dst);
        }

    GD_LOG(TRACE) << "Call drawBuffer method of surface";
    //! \fixme: unused result
}

/*!
 * \brief GepardSoftware::fillPath
 *
 * \internal
 * \todo unimplemented function
 */
void GepardSoftware::fillPath(PathData*, const GepardState&)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::strokePath
 *
 * \internal
 * \todo unimplemented function
 */
void GepardSoftware::strokePath()
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::drawImage
 * \param imagedata
 * \param sx
 * \param sy
 * \param sw
 * \param sh
 * \param dx
 * \param dy
 * \param dw
 * \param dh
 *
 * \internal
 * \todo unimplemented function
 */
void GepardSoftware::drawImage(const Image& imagedata, const Float sx, const Float sy, const Float sw, const Float sh, const Float dxconst, const Float dy, const Float dw, const Float dh)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::putImage
 * \param imagedata
 * \param dx
 * \param dy
 * \param dirtyX
 * \param dirtyY
 * \param dirtyWidth
 * \param dirtyHeight
 *
 * \internal
 * \todo unimplemented function
 */
void GepardSoftware::putImage(const Image& imagedata, const Float dx, const Float dy, const Float dirtyX, const Float dirtyY, const Float dirtyWidth, const Float dirtyHeight)
{
    GD_NOT_IMPLEMENTED();
}

/*!
 * \brief GepardSoftware::getImage
 * \param sx
 * \param sy
 * \param sw
 * \param sh
 * \return
 *
 * \internal
 * \todo unimplemented function
 */
const Image GepardSoftware::getImage(const Float sx, const Float sy, const Float sw, const Float sh)
{
    GD_NOT_IMPLEMENTED();
    return Image();
}

} // namespace software
} // namespace gepard
