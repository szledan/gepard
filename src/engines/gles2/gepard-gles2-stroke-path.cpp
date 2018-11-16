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
 */

#include "gepard-gles2.h"

#include "gepard-color.h"
#include "gepard-float.h"
#include "gepard-logging.h"
#include "gepard-path.h"
#include "gepard-state.h"
#include "gepard-stroke-builder.h"
#include <memory>

namespace gepard {
namespace gles2 {

void GepardGLES2::strokePath()
{
    PathData* pathData = _context.path.pathData();
    GepardState& state = _context.currentState();

    GD_LOG(DEBUG) << "Path: " << pathData->firstElement();
    if (!pathData || pathData->isEmpty())
        return;

    Float miterLimit = state.miterLimit ? state.miterLimit : 10;

    StrokePathBuilder sPath(state.lineWitdh, miterLimit, state.lineJoinMode, state.lineCapMode);
    sPath.convertStrokeToFill(pathData);

    Color tempColor = state.fillColor;
    state.fillColor = state.strokeColor;
    fillPath(sPath.pathData(), state);
    state.fillColor = tempColor;
}

} // namespace gles2
} // namespace gepard
