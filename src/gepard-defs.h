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

#ifndef gepard_defs_h
#define gepard_defs_h

#include <assert.h>
#include <iostream>
#include <math.h>
#include <memory.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <EGL/egl.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>

#define GL_GLEXT_PROTOTYPES 1

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
//#include <dlfcn.h>

#define ASSERT0(condition, message) \
    if (!(condition)) {\
        printf("ERROR: " message "\n"); \
        exit(-1); \
    }

#define ASSERT1(condition, message, value) \
    if (!(condition)) { \
        printf("ERROR: " message "\n", value); \
        exit(-1); \
    }

#define PROGRAM_STR(...)  #__VA_ARGS__
#define PROGRAM(...) PROGRAM_STR(__VA_ARGS__)

#ifdef ASSERT
#undef ASSERT
#endif
#define ASSERT(...) assert(__VA_ARGS__);

#define ERR_MSG_EMPTY_STRING "Empty string!"

#ifdef LOG_ERR
#undef LOG_ERR
#endif
#define LOG_ERR(MSG_TYPE) std::cerr << MSG_TYPE << std::endl;

#define ATTR_UNUSED __attribute__((unused))

namespace gepard {
} // namespace gepard

#endif // gepard_defs_h
