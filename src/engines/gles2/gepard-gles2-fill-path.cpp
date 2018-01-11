/* Copyright (C) 2017-2018, Gepard Graphics
 * Copyright (C) 2013, Zoltan Herczeg
 * Copyright (C) 2013, 2017-2018, Szilard Ledan <szledan@gmail.com>
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

#ifdef GD_USE_GLES2

#include "gepard-gles2.h"

#include "gepard-defs.h"
#include "gepard-gles2-defs.h"
#include "gepard-gles2-shader-factory.h"
#include "gepard-gles2-trapezoid-tessellator.h"
#include <string>

namespace gepard {
namespace gles2 {

static const std::string s_fillPathVertexShader = GD_GLES2_SHADER_PROGRAM(
    precision highp float;

    uniform vec2 u_size;

    attribute vec4 a_position;
    attribute vec4 a_x1x2Cdx1dx2;

    // To reduce the rounding issues of float16 variables,
    // the numbers are spearated for integer and fractional parts.
    // On the downside, this doubles the required arguments.
    varying vec4 v_y1y2;
    varying vec4 v_x1x2;
    varying vec2 v_dx1dx2;

    void main(void)
    {
        float y = floor(a_position[2]);
        v_y1y2[0] = a_position[1] - y;
        v_y1y2[1] = floor(a_position[3]) - y;
        v_y1y2[2] = fract(a_position[2]);
        v_y1y2[3] = fract(a_position[3]);

        float x = floor(a_x1x2Cdx1dx2[0]);
        v_x1x2[0] = a_position[0] - x;
        v_x1x2[1] = floor(a_x1x2Cdx1dx2[1]) - x;
        v_x1x2[2] = fract(a_x1x2Cdx1dx2[0]);
        v_x1x2[3] = fract(a_x1x2Cdx1dx2[1]);

        // slopeLeft
        v_dx1dx2[0] = a_x1x2Cdx1dx2[2];
        // slopeRight
        v_dx1dx2[1] = a_x1x2Cdx1dx2[3];

        gl_Position = vec4((2.0 * a_position.xy / u_size.xy) - 1.0, 0.0, 1.0);
    }
);

static const std::string s_fillPathFragmentShader = GD_GLES2_SHADER_PROGRAM(
    precision highp float;

    varying vec4 v_y1y2;
    varying vec4 v_x1x2;
    varying vec2 v_dx1dx2;

    void main(void)
    {
        const float step = 1.0 / float(GD_GLES2_ANTIALIAS_LEVEL);
        const float rounding = 0.5 / float(GD_GLES2_ANTIALIAS_LEVEL);

        float y = floor(v_y1y2[0]);
        float from = max(-y + v_y1y2[2], 0.0);
        float to = min(v_y1y2[1] - y + v_y1y2[3], 1.0) - from;

        vec2 x1x2 = (y + from) * (v_dx1dx2 * float(GD_GLES2_ANTIALIAS_LEVEL));

        float x = floor(v_x1x2[0]);
        x1x2[0] = (-x) + (x1x2[0] + v_x1x2[2]);
        x1x2[1] = (v_x1x2[1] - x) + (x1x2[1] + v_x1x2[3]);

        // Alpha value to must be > 0.
        float alpha = 1.0;

        float sum = (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0));
        if (to > 1.0 - rounding) {
            vec2 last = x1x2 + v_dx1dx2 * (float(GD_GLES2_ANTIALIAS_LEVEL) - 1.0);
            sum += (clamp(last[1], 0.0, 1.0) - clamp(last[0], 0.0, 1.0));
            to -= step;
        }

        if (sum <= 2.0 - rounding) {
            x1x2 += v_dx1dx2;

            while (to >= rounding) {
                sum += (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0));
                x1x2 += v_dx1dx2;
                to -= step;
            }

            alpha = sum * (step);
        }

        gl_FragColor = vec4(0.0, 0.0, 0.0, alpha);
    }
);

static const std::string s_copyPathVertexShader = GD_GLES2_SHADER_PROGRAM(
    precision highp float;

    uniform vec2 u_viewportSize;

    attribute vec4 a_position;

    varying vec2 v_texturePosition;

    void main()
    {
        v_texturePosition = a_position.zw;
        gl_Position = vec4((2.0 * a_position.xy / u_viewportSize) - 1.0, 0.0, 1.0);
    }
);

static const std::string s_copyPathFragmentShader = GD_GLES2_SHADER_PROGRAM(
    precision highp float;

    uniform vec4 u_color;
    uniform sampler2D u_texture;

    varying vec2 v_texturePosition;

    void main()
    {
        gl_FragColor = vec4(u_color.rgb, u_color.a * texture2D(u_texture, v_texturePosition).a);
    }
);

static void setupAttributes(Trapezoid& trapezoid, GLfloat* attributes, const int antiAliasingLevel)
{
    GLfloat slopeLeft = (trapezoid.bottomLeftX - trapezoid.topLeftX) / (trapezoid.bottomY - trapezoid.topY);
    GLfloat slopeRight = (trapezoid.bottomRightX - trapezoid.topRightX) / (trapezoid.bottomY - trapezoid.topY);
    const GLfloat bottomY = std::floor(trapezoid.topY);
    const GLfloat topY = std::ceil(trapezoid.bottomY);
    // The fraction is stored in a temporary variable.
    GLfloat temp, x1, x2;
    GLfloat bottomLeftX, bottomRightX, topLeftX, topRightX;

    temp = std::ceil(trapezoid.bottomY) - (trapezoid.bottomY);
    x1 = trapezoid.bottomLeftX - temp * slopeLeft;
    x2 = trapezoid.bottomRightX - temp * slopeRight;
    topLeftX = std::floor(x1 - fabs(slopeLeft));
    topRightX = std::ceil(x2 + fabs(slopeRight));

    temp = trapezoid.topY - std::floor(trapezoid.topY);
    x1 = trapezoid.topLeftX - temp * slopeLeft;
    x2 = trapezoid.topRightX - temp * slopeRight;
    bottomLeftX = std::floor(x1 - std::fabs(slopeLeft));
    bottomRightX = std::ceil(x2 + std::fabs(slopeRight));

    slopeLeft /= antiAliasingLevel;
    slopeRight /= antiAliasingLevel;

    GD_ASSERT(topY != bottomY);
    for (int i = 0; i < 4; i++) {
        // Absolute coordinates are transformed to the [-1,+1] space.
        switch (i) {
        case 0:
            *attributes++ = (bottomLeftX);
            *attributes++ = (bottomY);
            break;

        case 1:
            *attributes++ = (topLeftX);
            *attributes++ = (topY);
            break;

        case 2:
            *attributes++ = (bottomRightX);
            *attributes++ = (bottomY);
            break;

        case 3:
            *attributes++ = (topRightX);
            *attributes++ = (topY);
            break;
        }

        *attributes++ = (trapezoid.topY);
        *attributes++ = (trapezoid.bottomY);

        *attributes++ = (x1);
        *attributes++ = (x2);
        *attributes++ = (slopeLeft);
        *attributes++ = (slopeRight);
    }
}

void GepardGLES2::fillPath(PathData* pathData, const Color& fillColor)
{
    makeCurrent();
    if (!pathData->firstElement())
        return;

    const uint32_t width = _context.surface->width();
    const uint32_t height = _context.surface->height();

    GLuint textureId;
    {
        GLuint fboId;
        glGenFramebuffers(1, &fboId);
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);

        glGenTextures(1, &textureId);
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureId, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, fboId);

        glBlendFunc(GL_ONE, GL_ONE);
    }

    {
        ShaderProgram& fillProgram = _shaderProgramManager.getProgram("fillPathProgram", s_fillPathVertexShader, s_fillPathFragmentShader);
        glUseProgram(fillProgram.id);

        constexpr int strideLength = 8 * sizeof(GLfloat);

        {
            const GLint index = glGetUniformLocation(fillProgram.id, "u_size");
            glUniform2f(index, width, height);
        }

        int offset = 0;
        {
            const GLint size = 4;
            const GLint index = glGetAttribLocation(fillProgram.id, "a_position");
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, strideLength, _attributes + offset);
            offset += size;
        }

        {
            const GLint size = 4;
            const GLint index = glGetAttribLocation(fillProgram.id, "a_x1x2Cdx1dx2");
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, strideLength, _attributes + offset);
            offset += size;
        }

        TrapezoidTessellator::FillRule fillRule = TrapezoidTessellator::FillRule::NonZero;

        TrapezoidTessellator tt(*pathData, fillRule, GD_GLES2_ANTIALIAS_LEVEL);
        const TrapezoidList trapezoidList = tt.trapezoidList();

        int trapezoidIndex = 0;
        for (Trapezoid trapezoid : trapezoidList) {
            GD_ASSERT(trapezoid.topY < trapezoid.bottomY);
            GD_ASSERT(trapezoid.topLeftX <= trapezoid.topRightX);
            GD_ASSERT(trapezoid.bottomLeftX <= trapezoid.bottomRightX);

            if (!trapezoid.leftId || !trapezoid.rightId)
                continue;

            setupAttributes(trapezoid, _attributes + trapezoidIndex * 32, tt.antiAliasingLevel());
            trapezoidIndex++;
            if (trapezoidIndex >= min(kMaximumNumberOfUshortQuads, kMaximumNumberOfAttributes / 32)) {
                GD_LOG2("Draw '" << trapezoidIndex << "' trapezoids with triangles in pairs.");
                glDrawElements(GL_TRIANGLES, 6 * trapezoidIndex, GL_UNSIGNED_SHORT, nullptr);
                trapezoidIndex = 0;
            }
        }

        if (trapezoidIndex) {
            GD_LOG2("Draw '" << trapezoidIndex << "' trapezoids with triangles in pairs.");
            glDrawElements(GL_TRIANGLES, 6 * trapezoidIndex, GL_UNSIGNED_SHORT, nullptr);
        }
    }

    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
        glClearColor(0.0, 0.0, 0.0, 0.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        ShaderProgram& copyProgram = _shaderProgramManager.getProgram("copyPathProgram", s_copyPathVertexShader, s_copyPathFragmentShader);
        glUseProgram(copyProgram.id);

        {
            const GLfloat textureCoords[] = {
                0.0, 0.0, 0.0, 0.0,
                (GLfloat)width, 0.0, 1.0, 0.0,
                0.0, (GLfloat)height, 0.0, 1.0,
                (GLfloat)width, (GLfloat)height, 1.0, 1.0,
            };

            const GLint index = glGetAttribLocation(copyProgram.id, "a_position");
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, textureCoords);
        }

        {
            const GLint index = glGetUniformLocation(copyProgram.id, "u_color");
            glUniform4f(index, ((Float)fillColor.r), ((Float)fillColor.g), ((Float)fillColor.b), fillColor.a);
        }

        {
            const GLint index = glGetUniformLocation(copyProgram.id, "u_viewportSize");
            glUniform2f(index, width, height);
        }

        glBindTexture(GL_TEXTURE_2D, textureId);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }


    render();
}

} // namespace gles2
} // namespace gepard

#endif // GD_USE_GLES2
