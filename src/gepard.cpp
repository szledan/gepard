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

#include "gepard.h"

namespace gepard {

//#include "shader.c"
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

//#include "fbo.c"
GLuint createFrameBuffer(GLuint texture)
{
    GLuint frameBufferObject;

    // Create a framebuffer object.
    glGenFramebuffers(1, &frameBufferObject);
    if (glGetError() != GL_NO_ERROR) {
        printf("Cannot allocate a frame buffer object\n");
        return 0;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferObject);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return frameBufferObject;
}

// triangle.c

const GLchar* simpleVertexShader = PROGRAM(

    attribute vec2 a_position;
    attribute vec4 a_color;

    varying vec4 v_color;

    void main(void)
    {
        v_color = a_color;
        gl_Position = vec4(a_position, 1.0, 1.0);
    }
);

const GLchar* simpleFragmentShader = PROGRAM(
    precision mediump float;

    varying vec4 v_color;

    void main(void)
    {
        gl_FragColor = v_color;
    }
);

void Gepard::fillPath(void)
{
    static GLuint simpleShader = 0;
    GLint intValue;
    static GLubyte indicies[] = { 0, 1, 2, 3, 4, 5 };
    static GLfloat allAttributes[] = {
        -1, -1,               // position_1
        1.0, 0.0, 0.0, 0.3,     // color_1
        -0.3, 1,               // position_2
        0.0, 1.0, 0.0, 0.3,     // color_2
        0.4, -1,               // position_3
        0.0, 0.0, 1.0, 0.3,     // color_3
        -0.4, -1,               // position_4
        1.0, 0.0, 0.0, 0.3,     // color_4
        0.3,  1,                // position_5
        0.0, 1.0, 0.0, 0.3,     // color_5
        1, -1,                  // position_6
        0.0, 0.0, 1.0, 0.3,     // color_6
    };

    compileShaderProg(&simpleShader, "SimpleShader", simpleVertexShader, simpleFragmentShader);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(simpleShader);

    /* Triangle 1 and 2 */
    intValue = glGetAttribLocation(simpleShader, "a_position");
    glEnableVertexAttribArray(intValue);
    glVertexAttribPointer(intValue, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), allAttributes);

    intValue = glGetAttribLocation(simpleShader, "a_color");
    glEnableVertexAttribArray(intValue);
    glVertexAttribPointer(intValue, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), allAttributes + 2);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, indicies);
    eglSwapBuffers(_surface->eglDisplay(), _surface->eglSurface());
}


} // namespace gepard
