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

#ifndef GEPARD_GLES2_DEFS_H
#define GEPARD_GLES2_DEFS_H

#include "gepard-defs.h"

#define GL_GLEXT_PROTOTYPES 1

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

namespace gepard {

namespace gles2 {

#define TEXTURE_SIZE 512

void compileShaderProg(GLuint* result, const char* name, const GLchar *vertexShaderSource, const GLchar *fragmentShaderSource)
{
    GLuint shaderProgram = 0;
    GLuint vertexShader = 0;
    GLuint fragmentShader = 0;
    GLint intValue;
    GLchar *log = NULL;
    GLsizei length;

    if (*result)
        return;

    // Shader programs are zero terminated
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &intValue);
    if (intValue != GL_TRUE) {
        glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &intValue);
        log = (GLchar*)malloc(intValue);
        glGetShaderInfoLog(vertexShader, intValue, &length, log);
        printf("Vertex shader compilation failed with: %s\n", log);
        goto error;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &intValue);
    if (intValue != GL_TRUE) {
        glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &intValue);
        log = (GLchar*)malloc(intValue);
        glGetShaderInfoLog(fragmentShader, intValue, &length, log);
        printf("Fragment shader compilation failed with: %s\n", log);
        goto error;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &intValue);
    if (intValue != GL_TRUE) {
        GLchar *log;
        GLsizei length = -13;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &intValue);
        log = (GLchar*)malloc(intValue);
        glGetProgramInfoLog(fragmentShader, intValue, &length, log);
        printf("Shader program link failed with: %s\n", log);
        goto error;
    }

    // According to the specification, the shaders are kept
    // around until the program object is freed (reference counted).
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    *result = shaderProgram;
    return;

error:
    printf("Warning: Shader program cannot be compiled!\n");
    if (log)
        free(log);
    if (vertexShader)
        glDeleteShader(vertexShader);
    if (fragmentShader)
        glDeleteShader(fragmentShader);
    if (shaderProgram)
        glDeleteProgram(shaderProgram);
}

} // namespace gles2

} // namespace gepard

#endif // GEPARD_GLES2_DEFS_H

#endif // USE_GLES2
