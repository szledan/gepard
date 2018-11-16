/* Copyright (C) 2016-2018, Gepard Graphics
 * Copyright (C) 2016-2018, Szilard Ledan <szledan@gmail.com>
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
#include "gepard-gles2-defs.h"
#include "gepard-gles2-shader-factory.h"

namespace gepard {
namespace gles2 {

static const std::string s_fillRectVertexShader = GD_GLES2_SHADER_PROGRAM(
    precision highp float;

    uniform vec2 u_size;

    attribute vec2 a_position;
    attribute vec4 a_color;

    varying vec4 v_color;

    void main(void)
    {
        vec2 coords = (2.0 * a_position.xy / u_size.xy) - 1.0;
        gl_Position = vec4(coords, 1.0, 1.0);

        v_color = a_color;
    }
);

static const std::string s_fillRectFragmentShader = GD_GLES2_SHADER_PROGRAM(
    precision highp float;

    varying vec4 v_color;

    void main(void)
    {
        gl_FragColor = v_color;
    }
);

/*!
 * \brief Fill rect with GLES2 backend.
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \internal
 */
void GepardGLES2::fillRect(const Float x, const Float y, const Float w, const Float h, const Color& fillColor)
{
    makeCurrent();
    glBindFramebuffer(GL_FRAMEBUFFER, _fboId);

    GD_LOG(DEBUG) << "Fill rect with GLES2 (" << x << ", " << y << ", " << w << ", " << h << ")";

    const uint32_t width = _context.surface->width();
    const uint32_t height = _context.surface->height();
    const int quadCount = 2;
    const int numberOfAttributes = 3 * quadCount;

    ShaderProgram& program = _shaderProgramManager.getProgram("fillRectProgram", s_fillRectVertexShader, s_fillRectFragmentShader);

    const GLfloat attributes[] = {
        GLfloat(x), GLfloat(y), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x + w), GLfloat(y), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x), GLfloat(y + h), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x + w), GLfloat(y + h), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
    };

    GD_LOG(TRACE) << "Set blending.";
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GD_LOG(TRACE) << "Use shader programs with ID '" << program.id << "'";
    glUseProgram(program.id);

    GD_LOG(TRACE) << "Binding attributes.";
    {
        const GLuint index = glGetUniformLocation(program.id, "u_size");
        glUniform2f(index, width, height);
    }

    const GLsizei stride = numberOfAttributes * sizeof(GL_FLOAT);
    int offset = 0;
    {
        const GLint size = 2;
        const GLuint index = glGetAttribLocation(program.id, "a_position");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, attributes + offset);
        offset += size;
    }

    {
        const GLint size = 4;
        const GLuint index = glGetAttribLocation(program.id, "a_color");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, attributes + offset);
        offset += size;
    }

    GD_LOG(TRACE) << "Draw '" << quadCount << "' quads with triangles in pairs";
    glDrawElements(GL_TRIANGLES, quadCount * 6, GL_UNSIGNED_SHORT, nullptr);

    render();
}

} // namespace gles2
} // namespace gepard
