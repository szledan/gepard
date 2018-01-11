/* Copyright (C) 2017, Gepard Graphics
 * Copyright (C) 2017, Szilard Ledan <szledan@gmail.com>
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

#include "gepard-context.h"

#include "gepard-defs.h"
#include "gepard-types.h"

namespace gepard {

GepardContext::GepardContext(Surface *surface_)
    : surface(surface_)
{
    states.push_back(GepardState());
}

void GepardContext::setFillStyle(const std::string& color)
{
    currentState().fillColor = Color(color);
}

void GepardContext::setStrokeStyle(const std::string& color)
{
    currentState().strokeColor = Color(color);
}

void GepardContext::setLineWidth(const std::string& width)
{
    currentState().lineWitdh = strToFloat(width);
}

void GepardContext::setLineCap(const std::string& capMode)
{
    currentState().lineCapMode = strToLineCap(capMode);
}

void GepardContext::setLineJoin(const std::string& joinMode)
{
    currentState().lineJoinMode = strToLineJoin(joinMode);;
}

void GepardContext::setMiterLimit(const std::string& limit)
{
    currentState().miterLimit = strToFloat(limit);
}

} // namespace gepard
