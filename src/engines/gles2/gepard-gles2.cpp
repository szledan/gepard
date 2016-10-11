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

namespace gepard {
namespace gles2 {

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

    //! \todo the GLES2 backend rendering into 'fbo' in the future, so this separation is temporary.
    if (display) {
        GD_LOG2("Initialize EGL.");
        eglDisplay = eglGetDisplay((EGLNativeDisplayType) display);

        if (eglDisplay == EGL_NO_DISPLAY) {
            GD_CRASH("eglGetDisplay returned EGL_DEFAULT_DISPLAY");
        }
        if (eglInitialize(eglDisplay, NULL, NULL) != EGL_TRUE) {
            GD_CRASH("eglInitialize returned with EGL_FALSE");
        }
        if (eglChooseConfig(eglDisplay, configAttribs, &eglConfig, 1, &numOfConfigs) != EGL_TRUE) {
            GD_CRASH("eglChooseConfig returned with EGL_FALSE");
        }
        EGLNativeWindowType window = context.surface->getWindow();
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, window, NULL);
        if (eglSurface == EGL_NO_SURFACE) {
            GD_CRASH("eglCreateWindowSurface returned EGL_NO_SURFACE");
        }
        _eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs);
        if (_eglContext == EGL_NO_CONTEXT) {
            GD_CRASH("eglCreateContext returned EGL_NO_CONTEXT");
        }
        if (eglMakeCurrent(eglDisplay, eglSurface, eglSurface, _eglContext) != EGL_TRUE) {
            GD_CRASH("eglMakeCurrent returned EGL_FALSE");
        }

        // Set EGL display & surface.
        _eglDisplay = eglDisplay;
        _eglSurface = eglSurface;
        GD_LOG3("EGL display and surface are: " << _eglDisplay  << " and " << _eglSurface << ".");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    } else {
        // 1. Get the default display.
        eglDisplay = eglGetDisplay((EGLNativeDisplayType) 0);

        // 2. Initialize EGL.
        eglInitialize(eglDisplay, 0, 0);

        // 3. Make OpenGL ES the current API.
        eglBindAPI(EGL_OPENGL_ES_API);

        // 4. Find a config that matches all requirements.
        int iConfigs;
        eglChooseConfig(eglDisplay, configAttribs, &eglConfig, 1, &iConfigs);

        if (iConfigs != 1) {
            GD_CRASH("Error: eglChooseConfig(): config not found.");
        }

        // 5. Create a surface to draw to.
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, (EGLNativeWindowType)NULL, NULL);

        // 6. Create a context.
        _eglContext = eglCreateContext(eglDisplay, eglConfig, NULL, contextAttribs);

        // 7. Bind the context to the current thread
        eglMakeCurrent(eglDisplay, eglSurface, eglSurface, _eglContext);

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
    }

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
}

GepardGLES2::~GepardGLES2()
{
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

//! \brief Fill rectangle vertex shader.
static const std::string s_fillRectVertexShader = "\
        uniform vec2 in_size;                           \
                                                        \
        attribute vec2 in_position;                     \
        attribute vec4 in_color;                        \
                                                        \
        varying vec4 v_color;                           \
                                                        \
        void main(void)                                 \
        {                                               \
            v_color = in_color;                         \
            vec2 coords = (2.0 * in_position.xy / in_size.xy) - 1.0; \
            gl_Position = vec4(coords, 1.0, 1.0);       \
        }                                               \
                                     ";

//! \brief Fill rectangle fragment shader.
static const std::string s_fillRectFragmentShader = "\
        precision highp float;                          \
                                                        \
        varying vec4 v_color;                           \
                                                        \
        void main(void)                                 \
        {                                               \
            gl_FragColor = v_color;                     \
        }                                               \
                                       ";

static ShaderProgram s_fillRectProgram(s_fillRectVertexShader, s_fillRectFragmentShader);

/*!
 * \brief Fill rect with GLES2 backend.
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \internal
 */
void GepardGLES2::fillRect(Float x, Float y, Float w, Float h)
{
    GD_LOG1("Fill rect with GLES2 (" << x << ", " << y << ", " << w << ", " << h << ")");

    const Color fillColor = _context.currentState().fillColor;
    const uint32_t width = _context.surface->width();
    const uint32_t height = _context.surface->height();
    const int quadCount = 2;
    const int numberOfAttributes = 3 * quadCount;

    GD_LOG2("1. Compile shaders.");
    ShaderProgram& program = s_fillRectProgram;
    program.compileShaderProgram();

    GLfloat attributes[] = {
        GLfloat(x), GLfloat(y), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x + w), GLfloat(y), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x), GLfloat(y + h), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x + w), GLfloat(y + h), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
    };

    GD_LOG2("1. Set blending.");
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GD_LOG2("2. Use shader programs.");
    glUseProgram(program.id);

    GD_LOG2("3. Binding attributes.");
    {
        GLuint index = glGetUniformLocation(program.id, "in_size");
        glUniform2f(index, width, height);
    }

    const GLsizei stride = numberOfAttributes * sizeof(GL_FLOAT);
    int offset = 0;
    {
        const GLint size = 2;
        GLuint index = glGetAttribLocation(program.id, "in_position");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, attributes + offset);
        offset = size;
    }

    {
        const GLint size = 4;
        GLuint index = glGetAttribLocation(program.id, "in_color");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, attributes + offset);
    }

    GD_LOG2("4. Draw triangles in pairs as quads.");
    glDrawElements(GL_TRIANGLES, quadCount * 6, GL_UNSIGNED_SHORT, nullptr);

    if (_context.surface->getDisplay()) {
        eglSwapBuffers(_eglDisplay, _eglSurface);
    } else if (_context.surface->getBuffer()) {
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) _context.surface->getBuffer());
    } else {
        std::vector<uint32_t> buffer(width * height);
        glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) buffer.data());
        _context.surface->drawBuffer(buffer.data());
    }

    //! \todo: fix id = 0;
    glDeleteProgram(s_fillRectProgram.id);
    s_fillRectProgram.id = 0;
}

} // namespace gles2
} // namespace gepard

#endif // GD_USE_GLES2
