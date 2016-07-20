/* Copyright (C) 2016, Gepard Graphics
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

#include "gepard-gles2-shader-factory.h"

#include "gepard-defs.h"
#include "gepard-gles2-defs.h"
#include <string>
#include <vector>

namespace gepard {
namespace gles2 {

// ShaderProgram

void ShaderProgram::logShaderCompileError(const GLuint shader)
{
    GLint maxLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    GLsizei length = 0;
    glGetShaderInfoLog(shader, maxLength, &length, &errorLog[0]);

    GD_LOG_ERR("Shader compilation failed with: " << std::string(errorLog.begin(), errorLog.end()));
}

void ShaderProgram::logProgramLinkError(const GLuint program)
{
    GLint maxLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<GLchar> errorLog(maxLength);
    GLsizei length = 0;
    glGetProgramInfoLog(program, maxLength, &length, &errorLog[0]);

    GD_LOG_ERR("Shader program link failed with: " << std::string(errorLog.begin(), errorLog.end()));
}

GLuint ShaderProgram::compileShader(GLenum type, const GLchar* shaderSource)
{
    GD_LOG1("Compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader program.");

    const GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &shaderSource, NULL);
    glCompileShader(shader);

    GLint isCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

    if (isCompiled != GL_TRUE) {
        logShaderCompileError(shader);
        return 0;
    }

    return shader;
}

GLuint ShaderProgram::linkPrograms(GLuint vertexShader, GLuint fragmentShader)
{
    GD_LOG1("Link shader programs; vertex shader: " << vertexShader << ", fragment shader: " << fragmentShader);
    GD_ASSERT(vertexShader && "Vertex shader program doesn't exist!");
    GD_ASSERT(fragmentShader && "Fragment shader program doesn't exist!");

    const GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    GLint isLinked;
    glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
    if (isLinked != GL_TRUE) {
        logProgramLinkError(program);
        glDeleteProgram(program);
        return 0;
    }
    return program;
}

void ShaderProgram::compileShaderProgram()
{
    if (id)
        return;

    GD_LOG1("Compile '" << _name << "' shader program.");

    GLuint vertexShader = 0;
    vertexShader = compileShader(GL_VERTEX_SHADER, (const GLchar*)_vertexShaderSource.c_str());

    GLuint fragmentShader = 0;
    fragmentShader = compileShader(GL_FRAGMENT_SHADER, (const GLchar*)_fragmentShaderSource.c_str());

    if (vertexShader && fragmentShader) {
        id = linkPrograms(vertexShader, fragmentShader);
        GD_LOG2("The " << _name << " linked program is: " << id << ".");
    }

    // According to the specification, the shaders are kept
    // around until the program object is freed (reference counted).
    if (vertexShader) {
        glDeleteShader(vertexShader);
    }
    if (fragmentShader) {
        glDeleteShader(fragmentShader);
    }
}

} // namespace gles2
} // namespace gepard

#endif // GD_USE_GLES2
