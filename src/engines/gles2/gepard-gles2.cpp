/* Copyright (C) 2016-2018, Gepard Graphics
 * Copyright (C) 2013, Zoltan Herczeg
 * Copyright (C) 2013, 2016-2018, Szilard Ledan <szledan@gmail.com>
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

#include "gepard-defs.h"
#include "gepard-engine.h"
#include "gepard-gles2-defs.h"
#include "gepard-gles2-shader-factory.h"
#include "gepard-logging.h"

namespace gepard {
namespace gles2 {

static const std::string s_textureVertexShader = GD_GLES2_SHADER_PROGRAM(
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

static const std::string s_textureFragmentShader = GD_GLES2_SHADER_PROGRAM(
    precision highp float;

    uniform sampler2D u_texture;

    varying vec2 v_texturePosition;

    void main()
    {
        gl_FragColor = texture2D(u_texture, v_texturePosition);
    }
);

const int GepardGLES2::kMaximumNumberOfAttributes = GLushort(-1) + 1;
const int GepardGLES2::kMaximumNumberOfUshortQuads = GepardGLES2::kMaximumNumberOfAttributes / 6;

GepardGLES2::GepardGLES2(GepardContext& context)
    : _context(context)
{
    GD_LOG(INFO) << "Create GepardGLES2 with surface: " << context.surface;

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

    GD_LOG(INFO) << "Get and set EGL display and surface";
    EGLDisplay eglDisplay = 0;
    EGLSurface eglSurface = 0;
    EGLint numOfConfigs = 0;
    EGLConfig eglConfig = 0;
    void* display = context.surface->getDisplay();
    GD_LOG(DEBUG) << "Display is: " << display;

    GD_LOG(INFO) << "Initialize EGL";
    // 1. Get the default display.
    eglDisplay = eglGetDisplay((EGLNativeDisplayType)display);

    if (eglDisplay == EGL_NO_DISPLAY) {
        GD_CRASH("eglGetDisplay returned EGL_NO_DISPLAY");
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

    // Set EGL display & surface.
    _eglDisplay = eglDisplay;
    _eglSurface = eglSurface;
    GD_LOG(DEBUG) << "EGL display and surface are: " << _eglDisplay  << " and " << _eglSurface;

    makeCurrent();

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

    glViewport(0, 0, context.surface->width(), context.surface->height());
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

    if (_eglDisplay != EGL_NO_DISPLAY) {
        eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        eglDestroyContext(_eglDisplay, _eglContext);
        eglDestroySurface(_eglDisplay, _eglSurface);
        eglTerminate(_eglDisplay);
        GD_LOG(DEBUG) << "Destroyed GepardGLES2 (display and surface were: " << _eglDisplay  << " and " << _eglSurface;
    } else if (_eglContext != EGL_NO_CONTEXT) {
        //! \todo destroy EGL Context
    }
}

void GepardGLES2::makeCurrent()
{
    static GepardGLES2* currentGepardGLES2 = nullptr;

    if (this != currentGepardGLES2) {
        GD_LOG(DEBUG) << "Make current binds the EGL rendering context";
        if (eglMakeCurrent(_eglDisplay, _eglSurface, _eglSurface, _eglContext) != EGL_TRUE) {
            GD_CRASH("eglMakeCurrent returned EGL_FALSE");
        }
        currentGepardGLES2 = this;
    }

    GD_LOG(TRACE) << "Current GepardGLES2: " << this;
}

void GepardGLES2::render()
{
    //! \todo(szledan): if needed, call 'makeCurrent();'.

    const uint32_t width = _context.surface->width();
    const uint32_t height = _context.surface->height();

    if (_context.surface->getDisplay()) {
        ShaderProgram& textureProgram = _shaderProgramManager.getProgram("s_textureProgram", s_textureVertexShader, s_textureFragmentShader);
        glUseProgram(textureProgram.id);

        {
            const GLfloat textureCoords[] = {
                0.0, (GLfloat)height, 0.0, 0.0,
                0.0, 0.0, 0.0, 1.0,
                (GLfloat)width, (GLfloat)height, 1.0, 0.0,
                (GLfloat)width, 0.0, 1.0, 1.0,
            };

            const GLint index = glGetAttribLocation(textureProgram.id, "a_position");
            glEnableVertexAttribArray(index);
            glVertexAttribPointer(index, 4, GL_FLOAT, GL_FALSE, 0, textureCoords);
        }

        {
            const GLint index = glGetUniformLocation(textureProgram.id, "u_viewportSize");
            glUniform2f(index, width, height);
        }

        glBindTexture(GL_TEXTURE_2D, _textureId);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE);

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
