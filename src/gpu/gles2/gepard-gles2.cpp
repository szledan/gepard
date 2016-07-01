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

#ifdef USE_GLES2

#include "gepard-gles2.h"

#include "gepard-defs.h"
#include "gepard-gles2-defs.h"
#include "gepard-gles2-shader-factory.h"

namespace gepard {
namespace gles2 {

GepardGLES2::GepardGLES2(Surface* surface)
    : _surface(surface)
{
    GD_LOG1("Create GepardGLES2 with surface: " << surface);

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
    void* display = _surface->getDisplay();
    GD_LOG3("Display is: " << display  << ".");
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
        EGLNativeWindowType window = _surface->getWindow();
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

        const GLfloat red = 0.0f;
        const GLfloat green = 0.0f;
        const GLfloat blue = 0.0f;
        const GLfloat alpha = 0.0f;
        GD_LOG2("Set clear color (" << float(red) << ", " << float(green) << ", " << float(blue) << ", " << float(alpha) << ")");
        glClearColor(red, green, blue, alpha);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
}

GepardGLES2::~GepardGLES2()
{
    eglMakeCurrent(_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(_eglDisplay, _eglContext);
    eglDestroySurface(_eglDisplay, _eglSurface);
    eglTerminate(_eglDisplay);
    GD_LOG1("Destroyed GepardGLES2 (display and surface were: " << _eglDisplay  << " and " << _eglSurface << ".");
}

/*!
 * \brief GepardGLES2::closePath
 *
 * \todo unimplemented function
 */
void GepardGLES2::closePath()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::moveTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardGLES2::moveTo(Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::lineTo
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardGLES2::lineTo(Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::quadraticCurveTo
 * \param cpx  X-axis value of _control_ poin
 * \param cpy  Y-axis value of _control_ poin
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardGLES2::quadraticCurveTo(Float cpx, Float cpy, Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::bezierCurveTo
 * \param cp1x  X-axis value of _first control_ point
 * \param cp1y  Y-axis value of _first control_ point
 * \param cp2x  X-axis value of _second control_ point
 * \param cp2y  Y-axis value of _second control_ point
 * \param x  X-axis value of _end_ point
 * \param y  Y-axis value of _end_ point
 *
 * \todo unimplemented function
 */
void GepardGLES2::bezierCurveTo(Float cp1x, Float cp1y, Float cp2x, Float cp2y, Float x, Float y)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::arcTo
 * \param x1  X-axis value of _tangent_ point
 * \param y1  Y-axis value of _tangent_ point
 * \param x2  X-axis value of _end_ point
 * \param y2  Y-axis value of _end_ point
 * \param radius  size of arc
 *
 * \todo unimplemented function
 */
void GepardGLES2::arcTo(Float x1, Float y1, Float x2, Float y2, Float radius)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::rect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \todo unimplemented function
 */
void GepardGLES2::rect(Float x, Float y, Float w, Float h)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::arc
 * \param x  X-axis value of _center_ point
 * \param y  Y-axis value of _center_ point
 * \param radius  size of arc
 * \param startAngle  specify the _start_ point on arc
 * \param endAngle  specify the _end_ point on arc
 * \param counterclockwise  specify the draw direction on arc
 *
 * \todo unimplemented function
 */
void GepardGLES2::arc(Float x, Float y, Float radius, Float startAngle, Float endAngle, bool counterclockwise)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::beginPath
 *
 * \todo unimplemented function
 */
void GepardGLES2::beginPath()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::fill
 *
 * \todo unimplemented function
 */
void GepardGLES2::fill()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::stroke
 *
 * \todo unimplemented function
 */
void GepardGLES2::stroke()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::drawFocusIfNeeded
 *
 * \todo unimplemented function
 */
void GepardGLES2::drawFocusIfNeeded(/*Element element*/)
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::clip
 *
 * \todo unimplemented function
 */
void GepardGLES2::clip()
{
    NOT_IMPLEMENTED();
}

/*!
 * \brief GepardGLES2::isPointInPath
 * \param x  X-axis value of the given point
 * \param y  Y-axis value of the given point
 * \return  true if the given _point_ is in the current path
 *
 * \todo unimplemented function
 */
bool GepardGLES2::isPointInPath(Float x, Float y)
{
    NOT_IMPLEMENTED();
    return false;
}

//! \brief Fill rectangle vertex shader.
static const GLchar* fillRectVertexShader = "\
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
            coords.y = -coords.y;                       \
            gl_Position = vec4(coords, 1.0, 1.0);       \
        }                                               \
                                     ";

//! \brief Fill rectangle fragment shader.
static const GLchar* fillRectFragmentShader = "\
        precision highp float;                          \
                                                        \
        varying vec4 v_color;                           \
                                                        \
        void main(void)                                 \
        {                                               \
            gl_FragColor = v_color;                     \
        }                                               \
                                       ";

/*!
 * \brief GepardGLES2::fillRect
 * \param x  X-axis value of _start_ and _end_ point
 * \param y  Y-axis value of _start_ and _end_ point
 * \param w  size on X-axis
 * \param h  size on Y-axis
 *
 * \todo unimplemented function
 */
void GepardGLES2::fillRect(Float x, Float y, Float w, Float h)
{
    GD_LOG1("Fill rect with GLES2 (" << x << ", " << y << ", " << w << ", " << h << ")");

    // 0. Rect attributes.
    const Color fillColor = state.fillColor;
    const GLubyte rectIndexes[] = {0, 1, 2, 2, 1, 3};
    const GLfloat attributes[] = {
        GLfloat(x), GLfloat(y), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x + w), GLfloat(y), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x), GLfloat(y + h), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
        GLfloat(x + w), GLfloat(y + h), GLfloat(fillColor.r), GLfloat(fillColor.g), GLfloat(fillColor.b), GLfloat(fillColor.a),
    };

    GD_LOG2("1. Set and enable blending.");
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GD_LOG2("2. Compile shaders.");
    GLuint& fillRectProgram = _programs["FillRectProgram"];
    ShaderProgram::compileShaderProg(&fillRectProgram, "FillRectProgram", fillRectVertexShader, fillRectFragmentShader);

    GD_LOG2("3. Use shader programs.");
    glUseProgram(fillRectProgram);

    GD_LOG2("4. Binding attributes.");
    {
        GLuint index = glGetUniformLocation(fillRectProgram, "in_size");
        glUniform2f(index, _surface->width(), _surface->height());
    }

    const GLsizei stride = 6 * sizeof(GL_FLOAT);
    int offset = 0;
    {
        const GLint size = 2;
        GLuint index = glGetAttribLocation(fillRectProgram, "in_position");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, attributes + offset);
        offset = size;
    }

    {
        const GLint size = 4;
        GLuint index = glGetAttribLocation(fillRectProgram, "in_color");
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, attributes + offset);
    }

    GD_LOG2("5. Draw two triangles as rect.");
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, rectIndexes);

    //! Swap buffers. \todo: Temporary call here.
    eglSwapBuffers(_eglDisplay, _eglSurface);
}

} // namespace gles2
} // namespace gepard

#endif // USE_GLES2
