/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2013, Zoltan Herczeg
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

#ifdef GD_USE_GLES2

#include "gepard-gles2.h"

#include "gepard-defs.h"
#include "gepard-engine.h"
#include "gepard-gles2-defs.h"
#include "gepard-gles2-shader-factory.h"
#include "gepard-gles2-trapezoid-tessellator.h"

#define _GD_PROGRAM_STR(...)  #__VA_ARGS__
#define GD_PROGRAM(...) _GD_PROGRAM_STR(__VA_ARGS__)
#define GD_CREATE_PROGRAM(_NAME, _VERTEX_SHADER, _FRAGMENT_SHADER) static gepard::gles2::ShaderProgram _NAME(_VERTEX_SHADER, _FRAGMENT_SHADER, #_NAME)

#define GD_ANTIALIAS_LEVEL 16

static const std::string s_textureVertexShader = GD_PROGRAM(
    precision highp float;

    attribute vec4 a_position;
    varying vec2 v_texturePosition;
    uniform vec2 u_viewportSize;

    void main()
    {
        v_texturePosition = a_position.zw;
        gl_Position = vec4((2.0 * a_position.xy / u_viewportSize) - 1.0, 0.0, 1.0);
    }
);

static const std::string s_textureFragmentShader = GD_PROGRAM(
    precision highp float;

    uniform sampler2D u_texture;
    varying vec2 v_texturePosition;

    void main()
    {
        gl_FragColor = texture2D(u_texture, v_texturePosition);
    }
);

GD_CREATE_PROGRAM(s_textureProgram, s_textureVertexShader, s_textureFragmentShader);

//! \brief Fill rectangle vertex shader.
static const std::string s_fillRectVertexShader = GD_PROGRAM(
    uniform vec2 u_size;

    attribute vec2 a_position;
    attribute vec4 a_color;

    varying vec4 v_color;

    void main(void)
    {
        v_color = a_color;
        vec2 coords = (2.0 * a_position.xy / u_size.xy) - 1.0;
        gl_Position = vec4(coords, 1.0, 1.0);
    }
);


//! \brief Fill rectangle fragment shader.
static const std::string s_fillRectFragmentShader = GD_PROGRAM(
    precision highp float;

    varying vec4 v_color;

    void main(void)
    {
        gl_FragColor = v_color;
    }
);

GD_CREATE_PROGRAM(s_fillRectProgram, s_fillRectVertexShader, s_fillRectFragmentShader);

//! \brief Fill path vertex shader.
static const std::string s_fillPathVertexShader = GD_PROGRAM(
    precision highp float;
    attribute vec4 a_position;
    attribute vec4 a_x1x2Cdx1dx2;

    // To reduce the rounding issues of float16 variables,
    // the numbers are spearated for integer and fractional parts.
    // On the downside, this doubles the required arguments.
    varying vec4 v_y1y2;
    varying vec4 v_x1x2;
    varying vec2 v_dx1dx2;

    uniform vec2 u_size;

    void main(void)
    {
        // a_position[2] = trapezoid.bottomY    < a_position[1]
        // a_position[3] = trapezoid.topY       > a_position[1]
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

//! \brief Fill path vertex shader.
static const std::string s_fillPathFragmentShader = GD_PROGRAM(
    precision highp float;

    varying vec4 v_y1y2;
    varying vec4 v_x1x2;
    varying vec2 v_dx1dx2;

    void main(void)
    {
        const float step = 1.0 / GD_ANTIALIAS_LEVEL.0;
        const float rounding = 0.5 / GD_ANTIALIAS_LEVEL.0;

        float y = floor(v_y1y2[0]);
        float from = max(-y + v_y1y2[2], 0.0);
        float to = min(v_y1y2[1] - y + v_y1y2[3], 1.0) - from;

        vec2 x1x2 = (y + from) * (v_dx1dx2 * GD_ANTIALIAS_LEVEL.0);

        float x = floor(v_x1x2[0]);
        x1x2[0] = (-x) + (x1x2[0] + v_x1x2[2]);
        x1x2[1] = (v_x1x2[1] - x) + (x1x2[1] + v_x1x2[3]);

        // Alpha value to must be > 0.
        float alpha = 1.0;

        float sum = (clamp(x1x2[1], 0.0, 1.0) - clamp(x1x2[0], 0.0, 1.0));
        if (to > 1.0 - rounding) {
            vec2 last = x1x2 + v_dx1dx2 * (GD_ANTIALIAS_LEVEL.0 - 1.0);
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

GD_CREATE_PROGRAM(s_fillPathProgram, s_fillPathVertexShader, s_fillPathFragmentShader);

static const std::string s_copyPathVertexShader = GD_PROGRAM(
    precision highp float;

    attribute vec4 a_position;

    varying vec2 v_texturePosition;

    uniform vec2 u_viewportSize;

    void main()
    {
        v_texturePosition = a_position.zw;
        gl_Position = vec4((2.0 * a_position.xy / u_viewportSize) - 1.0, 0.0, 1.0);
    }
);

static const std::string s_copyPathFragmentShader = GD_PROGRAM(
    precision highp float;

    uniform sampler2D u_texture;
    uniform vec4 u_color;

    varying vec2 v_texturePosition;

    void main()
    {
        gl_FragColor = vec4(u_color.rgb, u_color.a * texture2D(u_texture, v_texturePosition).a);
    }
);

GD_CREATE_PROGRAM(s_copyPathProgram, s_copyPathVertexShader, s_copyPathFragmentShader);

namespace gepard {
namespace gles2 {

const int GepardGLES2::kMaximumNumberOfAttributes = GLushort(-1) + 1;
const int GepardGLES2::kMaximumNumberOfUshortQuads = GepardGLES2::kMaximumNumberOfAttributes / 6;

GepardGLES2::GepardGLES2(GepardContext& context)
    : _context(context)
{
    GD_LOG1("Create GepardGLES2 with surface: " << context.surface);

    const EGLint configAttribs[] = {
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_ALPHA_SIZE, 8,
        EGL_DEPTH_SIZE, 8,
        EGL_STENCIL_SIZE, 8,

        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_SAMPLE_BUFFERS, 0,
        EGL_SAMPLES, 0,
        EGL_NONE
    };

    const EGLint contextAttribs[] = {
        // Identify OpenGL 2 ES context
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE,
    };

    GD_LOG2("Get and set EGL display and surface.");
    EGLDisplay eglDisplay = 0;
    EGLSurface eglSurface = 0;
    EGLint numOfConfigs = 0;
    EGLConfig eglConfig = 0;
    void* display = context.surface->getDisplay();
    GD_LOG3("Display is: " << display  << ".");

    GD_LOG2("Initialize EGL.");
    // 1. Get the default display.
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)display);

    if (eglDisplay == EGL_NO_DISPLAY) {
        GD_CRASH("eglGetDisplay returned EGL_DEFAULT_DISPLAY");
    }

    // 2. Initialize EGL.
    if (eglInitialize(eglDisplay, NULL, NULL) != EGL_TRUE) {
        GD_CRASH("eglInitialize returned with EGL_FALSE");
    }

    if (!display) {
        // 3. Make OpenGL ES the current API.
        eglBindAPI(EGL_OPENGL_ES_API);
    }

    // 4. Find a config that matches all requirements.
    if (eglChooseConfig(eglDisplay, configAttribs, &eglConfig, 1, &numOfConfigs) != EGL_TRUE) {
        GD_CRASH("eglChooseConfig returned with EGL_FALSE");
    }

    // 5. Create a surface to draw to.
    EGLNativeWindowType window = context.surface->getWindow();
    eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType)window, NULL);
    if (display && eglSurface == EGL_NO_SURFACE) {
        GD_CRASH("eglCreateWindowSurface returned EGL_NO_SURFACE");
    }

    // 6. Create a context.
    _eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs);
    if (_eglContext == EGL_NO_CONTEXT) {
        GD_CRASH("eglCreateContext returned EGL_NO_CONTEXT");
    }

    // 7. Bind the context to the current thread
    if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, _eglContext) != EGL_TRUE) {
        GD_CRASH("eglMakeCurrent returned EGL_FALSE");
    }

    // Set EGL display & surface.
    _eglDisplay = eglDisplay;
    _eglSurface = eglSurface;
    GD_LOG3("EGL display and surface are: " << _eglDisplay  << " and " << _eglSurface << ".");

    glGenFramebuffers(1, &_fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, _fboId);

    glGenTextures(1, &_textureId);
    glBindTexture(GL_TEXTURE_2D, _textureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, context.surface->width(), context.surface->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textureId, 0);

    const GLfloat red = 0.0f;
    const GLfloat green = 0.0f;
    const GLfloat blue = 0.0f;
    const GLfloat alpha = 0.0f;
    GD_LOG2("Set clear color (" << float(red) << ", " << float(green) << ", " << float(blue) << ", " << float(alpha) << ")");
    glClearColor(red, green, blue, alpha);
    glViewport(0, 0, context.surface->width(), context.surface->height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_BLEND);

    GLushort* quadIndexes;
    int bufferSize = kMaximumNumberOfUshortQuads * 6;

    quadIndexes = reinterpret_cast<GLushort*>(malloc(bufferSize * sizeof(GLushort)));

    GLushort* currentQuad = quadIndexes;
    GLushort* currentQuadEnd = quadIndexes + bufferSize;

    GLushort index = 0;
    while (currentQuad < currentQuadEnd) {
        currentQuad[0] = index;
        currentQuad[1] = index + 1;
        currentQuad[2] = index + 2;
        currentQuad[3] = index + 2;
        currentQuad[4] = index + 1;
        currentQuad[5] = index + 3;
        currentQuad += 6;
        index += 4;
    }

    glGenBuffers(1, &_indexBufferObject);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, bufferSize * sizeof(GLushort), quadIndexes, GL_STATIC_DRAW);

    free(quadIndexes);

    _attributes = reinterpret_cast<GLfloat*>(malloc(kMaximumNumberOfAttributes * sizeof(GLfloat)));
}

GepardGLES2::~GepardGLES2()
{
    if (_attributes) {
        free(_attributes);
    }

    glDeleteProgram(s_textureProgram.id);
    s_textureProgram.id = GLuint(-1);
    glDeleteProgram(s_fillRectProgram.id);
    s_fillRectProgram.id = GLuint(-1);
    glDeleteProgram(s_fillPathProgram.id);
    s_fillPathProgram.id = GLuint(-1);
    glDeleteProgram(s_copyPathProgram.id);
    s_copyPathProgram.id = GLuint(-1);

    if (_eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(_eglDisplay, _eglContext);
        eglDestroySurface(_eglDisplay, _eglSurface);
        eglTerminate(_eglDisplay);
        GD_LOG1("Destroyed GepardGLES2 (display and surface were: " << _eglDisplay  << " and " << _eglSurface << ".");
    } else if (_eglContext != EGL_NO_CONTEXT) {
        //! \todo destroy EGL Context
    }
}

/*!
 * \brief Fill rect with GLES2 backend.
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \internal
 */
void GepardGLES2::fillRect(const Float x, const Float y, const Float w, const Float h)
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fboId);

    GD_LOG1("Fill rect with GLES2 (" << x << ", " << y << ", " << w << ", " << h << ")");

    const Color fillColor = _context.currentState().fillColor;
    const uint32_t width = _context.surface->width();
    const uint32_t height = _context.surface->height();
    const int quadCount = 2;
    const int numberOfAttributes = 3 * quadCount;

    ShaderProgram& program = s_fillRectProgram;
    program.compileShaderProgram();

    const GLfloat attributes[] = {
        GLfloat(x), GLfloat(y), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x + w), GLfloat(y), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x), GLfloat(y + h), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x + w), GLfloat(y + h), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
    };

    GD_LOG2("1. Set blending.");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GD_LOG2("2. Use shader programs with '" << program.id << "' ID.");
    glUseProgram(program.id);

    GD_LOG2("3. Binding attributes.");
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

    GD_LOG2("4. Draw '" << quadCount << "' quads with triangles in pairs.");
    glDrawElements(GL_TRIANGLES, quadCount * 6, GL_UNSIGNED_SHORT, nullptr);

    render();
}

static void setupAttributes(Trapezoid& trapezoid, GLfloat* attributes, const int antiAliasingLevel)
{
    GLfloat slopeLeft = (trapezoid.bottomLeftX - trapezoid.topLeftX) / (trapezoid.bottomY - trapezoid.topY);
    GLfloat slopeRight = (trapezoid.bottomRightX - trapezoid.topRightX) / (trapezoid.bottomY - trapezoid.topY);
    const GLfloat bottomY = floor(trapezoid.topY);
    const GLfloat topY = ceil(trapezoid.bottomY);
    // The fraction is stored in a temporary variable.
    GLfloat temp, x1, x2;
    GLfloat bottomLeftX, bottomRightX, topLeftX, topRightX;

    temp = ceil(trapezoid.bottomY) - (trapezoid.bottomY);
    x1 = trapezoid.bottomLeftX - temp * slopeLeft;
    x2 = trapezoid.bottomRightX - temp * slopeRight;
    topLeftX = floor(x1 - fabs(slopeLeft));
    topRightX = ceil(x2 + fabs(slopeRight));

    temp = trapezoid.topY - floor(trapezoid.topY);
    x1 = trapezoid.topLeftX - temp * slopeLeft;
    x2 = trapezoid.topRightX - temp * slopeRight;
    bottomLeftX = floor(x1 - fabs(slopeLeft));
    bottomRightX = ceil(x2 + fabs(slopeRight));

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

void GepardGLES2::fill()
{
    if (!_context.path.pathData()->firstElement())
        return;

    const uint32_t width = _context.surface->width();
    const uint32_t height = _context.surface->height();

    TrapezoidTessellator::FillRule fillRule = TrapezoidTessellator::FillRule::NonZero;

    TrapezoidTessellator tt(&_context.path, fillRule, GD_ANTIALIAS_LEVEL);
    TrapezoidList trapezoidList = tt.trapezoidList();

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
        ShaderProgram& fillProgram = s_fillPathProgram;
        fillProgram.compileShaderProgram();
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
    }

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

    glBindFramebuffer(GL_FRAMEBUFFER, _fboId);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    {
        ShaderProgram& copyProgram = s_copyPathProgram;
        copyProgram.compileShaderProgram();
        glUseProgram(copyProgram.id);

        const GLfloat textureCoords[] = {
            0.0, (GLfloat)height, 0.0, 0.0,
            0.0, 0.0, 0.0, 1.0,
            (GLfloat)width, (GLfloat)height, 1.0, 0.0,
            (GLfloat)width, 0.0, 1.0, 1.0,
        };

        GLint index = glGetAttribLocation(copyProgram.id, "a_position");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, textureCoords);

        index = glGetUniformLocation(copyProgram.id, "u_viewportSize");
        glUniform2f(index, width, height);

        glBindTexture(GL_TEXTURE_2D, textureId);

        index = glGetUniformLocation(copyProgram.id, "u_color");
        const Color fillColor = _context.currentState().fillColor;
        glUniform4f(index, ((Float)fillColor.r), ((Float)fillColor.g), ((Float)fillColor.b), fillColor.a);
    }

    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    render();
}

void GepardGLES2::stroke()
{
    GD_NOT_IMPLEMENTED();
}

void GepardGLES2::render()
{
    const uint32_t width = _context.surface->width();
    const uint32_t height = _context.surface->height();

    if (_context.surface->getDisplay()) {
        ShaderProgram& textureProgram = s_textureProgram;
        textureProgram.compileShaderProgram();
        glUseProgram(textureProgram.id);

        static const GLfloat textureCoords[] = {
            0, (GLfloat)height, 0, 0,
            0, 0, 0, 1,
            (GLfloat)width, (GLfloat)height, 1, 0,
            (GLfloat)width, 0, 1, 1
        };

        GLint intValue = glGetAttribLocation(textureProgram.id, "a_position");
        glEnableVertexAttribArray(intValue);
        glVertexAttribPointer(intValue, 4, GL_FLOAT, GL_FALSE, 0, textureCoords);

        intValue = glGetUniformLocation(textureProgram.id, "u_viewportSize");
        glUniform2f(intValue, width, height);

        glBindTexture(GL_TEXTURE_2D, _textureId);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        eglSwapBuffers(_eglDisplay, _eglSurface);
    } else if (_context.surface->getBuffer()) {
        glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) _context.surface->getBuffer());
    } else {
        glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
        std::vector<uint32_t> buffer(width * height);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) buffer.data());
        _context.surface->drawBuffer(buffer.data());
    }
}

} // namespace gles2
} // namespace gepard

#endif // GD_USE_GLES2
