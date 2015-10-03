/* Copyright (C) 2015, Szilard Ledan <szledan@gmail.com>
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
 *
 * The views and conclusions contained in the software and documentation are those
 * of the authors and should not be interpreted as representing official policies,
 * either expressed or implied, of the FreeBSD Project.
 */

#ifndef gepard_surface_h
#define gepard_surface_h

#include "config.h"

namespace gepard {

class GepardSurface {
public:
    GepardSurface()
        : _eglDisplay(0)
        , _eglSurface(0)
    {}
    GepardSurface(EGLDisplay eglDisplay, EGLSurface eglSurface)
        : _eglDisplay(eglDisplay)
        , _eglSurface(eglSurface)
    {}

    EGLDisplay eglDisplay() const { return _eglDisplay; }
    EGLSurface eglSurface() const { return _eglSurface; }

    void set(EGLDisplay eglDisplay, EGLSurface eglSurface)
    {
        _eglDisplay = eglDisplay;
        _eglSurface = eglSurface;
    }

private:
    EGLDisplay _eglDisplay;
    EGLSurface _eglSurface;
};

class XGepardSurface : public GepardSurface {
public:
    XGepardSurface(unsigned int width, unsigned int height)
        : _width(width)
        , _height(height)
    {
        const EGLint configAttribs[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 8,
            EGL_STENCIL_SIZE, 8,

            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_SAMPLE_BUFFERS, 1,
            EGL_SAMPLES, 4,
            EGL_NONE
        };

        const EGLint contextAttribs[] = {
            // Identify OpenGL 2 ES context
            EGL_CONTEXT_CLIENT_VERSION, 3,
            EGL_NONE,
        };

        EGLDisplay eglDisplay = 0;
        EGLSurface eglSurface = 0;
        EGLint numOfConfigs = 0;
        EGLConfig eglConfig = 0;

        XSetWindowAttributes windowAttributes;
        XWMHints hints;
        Window root = 0;
        //XEvent xEvent;

        _display = XOpenDisplay(NULL);
        ASSERT0(_display != NULL, "XOpenDisplay returned NULL");
        root = XDefaultRootWindow(_display);
        windowAttributes.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
        _window = XCreateWindow ( // create a window with the provided parameters
            _display, root, 0, 0, width, height, 0,
            CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &windowAttributes);
        hints.input = True;
        hints.flags = InputHint;
        XSetWMHints(_display, _window, &hints);

        XMapWindow(_display, _window); // make the window visible on the screen
        XStoreName(_display, _window, "OpenGLES tests with EGL" );
        eglDisplay = eglGetDisplay((EGLNativeDisplayType)_display);
        //eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        // Initialize EGL
        ASSERT0(eglDisplay != EGL_NO_DISPLAY, "eglGetDisplay returned EGL_DEFAULT_DISPLAY");
        ASSERT0(eglInitialize(eglDisplay, NULL, NULL) == EGL_TRUE, "eglInitialize returned with EGL_FALSE");

        ASSERT0(eglChooseConfig(eglDisplay, configAttribs, &eglConfig, 1, &numOfConfigs) == EGL_TRUE, "eglChooseConfig returned with EGL_FALSE");
        eglSurface = eglCreateWindowSurface(eglDisplay, eglConfig, _window, NULL);
        ASSERT0(eglSurface != EGL_NO_SURFACE, "eglCreateWindowSurface returned EGL_NO_SURFACE");
        _eglContext = eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, contextAttribs);
        ASSERT0(_eglContext != EGL_NO_CONTEXT, "eglCreateContext returned EGL_NO_CONTEXT");
        ASSERT0(eglMakeCurrent(eglDisplay, eglSurface, eglSurface, _eglContext) == EGL_TRUE, "eglMakeCurrent returned EGL_FALSE");

        set(eglDisplay, eglSurface);
    }
    ~XGepardSurface()
    {
        eglDestroyContext(eglDisplay(), _eglContext);
        eglDestroySurface(eglDisplay(), eglSurface());
        eglTerminate(eglDisplay());

        XDestroyWindow(_display, _window);
        XCloseDisplay(_display);
    }

    Display* display() const { return _display; }

private:
    unsigned int _width;
    unsigned int _height;
    Display* _display;
    Window _window;
    EGLContext _eglContext;
};

} // namespace gepard

#endif // gepard_surface_h
