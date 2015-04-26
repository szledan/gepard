/* Copyright (C) 2015, Kristof Kosztyo
 * Copyright (C) 2015, Szilard Ledan <szledan@gmail.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
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

#include "EgWindow.h"
#include <EGL/egl.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

namespace example {

static const EGLint s_configAttribs[] = {
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

static const EGLint s_contextAttribs[] = {
    // Identify OpenGL 2 ES context
    EGL_CONTEXT_CLIENT_VERSION, 3,
    EGL_NONE,
};

void EgWindow::create()
{
    EGLint numOfConfigs;
    EGLConfig eglConfig;
    EGLContext eglContext;

    Display *display;
    XSetWindowAttributes windowAttributes;
    XWMHints hints;
    Window root;
    Window window;
//        XEvent xEvent;

    display = XOpenDisplay(NULL);
    ASSERT0(display != NULL, "XOpenDisplay returned NULL");
    root = XDefaultRootWindow(display);
    windowAttributes.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
    window = XCreateWindow ( // create a window with the provided parameters
        display, root, 0, 0, m_width, m_height, 0,
        CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &windowAttributes);
    hints.input = True;
    hints.flags = InputHint;
    XSetWMHints(display, window, &hints);

    XMapWindow(display, window); // make the window visible on the screen
    XStoreName(display, window, "OpenVG tests with EGL" );
    m_eglDisplay = eglGetDisplay((EGLNativeDisplayType)display);
    //eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    // Initialize EGL
    ASSERT0(m_eglDisplay != EGL_NO_DISPLAY, "eglGetDisplay returned EGL_DEFAULT_DISPLAY");
    ASSERT0(eglInitialize(m_eglDisplay, NULL, NULL) == EGL_TRUE, "eglInitialize returned with EGL_FALSE");

    ASSERT0(eglChooseConfig(m_eglDisplay, s_configAttribs, &eglConfig, 1, &numOfConfigs) == EGL_TRUE, "eglChooseConfig returned with EGL_FALSE");
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, eglConfig, window, NULL);
    ASSERT0(m_eglSurface != EGL_NO_SURFACE, "eglCreateWindowSurface returned EGL_NO_SURFACE");
    eglContext = eglCreateContext(m_eglDisplay, eglConfig, EGL_NO_CONTEXT, s_contextAttribs);
    ASSERT0(eglContext != EGL_NO_CONTEXT, "eglCreateContext returned EGL_NO_CONTEXT");
    ASSERT0(eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, eglContext) == EGL_TRUE, "eglMakeCurrent returned EGL_FALSE");

    //initScene();

    //drawScene();
    eglSwapBuffers(m_eglDisplay, m_eglSurface);

//leave:

    eglDestroyContext(m_eglDisplay, eglContext);
    eglDestroySurface(m_eglDisplay, m_eglSurface);
    eglTerminate(m_eglDisplay);

    XDestroyWindow(display, window);
    XCloseDisplay(display);

}

} // namespace example
