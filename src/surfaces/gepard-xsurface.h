/* Copyright (C) 2016, Gepard Graphics
 * Copyright (C) 2016, Kristof Kosztyo <kkristof@inf.u-szeged.hu>
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

#ifndef GEPARD_XSURFACE_H
#define GEPARD_XSURFACE_H

#include "gepard.h"

#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string>

namespace gepard {

/*!
 * \brief A simple XSurface class for _Gepard_
 *
 * \todo: documentation is missing.
 */
class XSurface : public Surface {
public:
    XSurface(uint32_t width = 0, uint32_t height = 0, std::string windowTitle = "XSurface for gepard")
        : Surface(width, height)
    {
        XSetWindowAttributes windowAttributes;
        XWMHints hints;
        Window root = 0;

        _display = XOpenDisplay(NULL);
        if(_display == NULL) {
            //! \todo: LOG: "XOpenDisplay returned NULL");
        }
        root = XDefaultRootWindow(_display);
        windowAttributes.event_mask = ExposureMask | PointerMotionMask | KeyPressMask;
        _window = XCreateWindow ( // create a window with the provided parameters
            _display, root, 0, 0, width, height, 0,
            CopyFromParent, InputOutput, CopyFromParent, CWEventMask, &windowAttributes);
        hints.input = True;
        hints.flags = InputHint;
        XSetWMHints(_display, _window, &hints);

        XMapWindow(_display, _window); // make the window visible on the screen
        XStoreName(_display, _window, windowTitle.c_str());
    }
    ~XSurface()
    {
        XDestroyWindow(_display, _window);
        XCloseDisplay(_display);
    }

    virtual void* getDisplay() { return (void*)_display; }
    virtual unsigned long getWindow() { return _window; }
    virtual void* getBuffer() { return nullptr; }
    virtual void drawBuffer(void* rgba)
    {
        Pixmap pixMap = XCreatePixmapFromBitmapData(_display, _window, (char *)rgba, width(), height(), 0x01, 0x00, 1);

        GC gc = XCreateGC(_display, _window, 0, NULL);
        XSetClipMask(_display, gc, pixMap);
        XSetClipOrigin(_display, gc, 0*width(), 0*height());

//        XImage* xImage = XGetImage(_display, _window, 0, 0, width(), height(), AllPlanes, XYPixmap);
        XImage* image = image = XCreateImage(_display, g_visual, 1, XYBitmap,
                                             0, (char *)bitmap, 256, 256, 32, 0);XGetImage(_display, _window, 0, 0, width(), height(), AllPlanes, XYPixmap);
        image->width = width();
        image->height = height();
        image->data = (char *)rgba;

        for (uint32_t left_x = 0; left_x < width() / 2; ++left_x)
            for (uint32_t y = 0; y < height(); ++y) {
                int right_x = width() - left_x;
                if (left_x != right_x)
                    XPutPixel(xImage, left_x, y, XGetPixel(image, right_x, y));
                XPutPixel(xImage, right_x, y, XGetPixel(image, left_x, y));
            }
        XPutImage(_display, _window, gc, xImage, 0, 0, 0, 0, width(), height());
    }

private:
    Display* _display;
    Window _window;
};

} // namespace gepard

#endif // GEPARD_XSURFACE_H
